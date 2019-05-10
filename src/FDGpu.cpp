#include "FDGpu.hpp"
#include <FRONTIER/OpenGL.hpp>
#include <FRONTIER/System/util/file_content.hpp>
#include <algorithm>
#include <iostream>

namespace {
    size_t glget(GLenum id)
    {
        GLint ret;
        glGetIntegerv(id,&ret);
        return ret;
    }

    size_t glget(GLenum id,int index)
    {
        GLint ret;
        glGetIntegeri_v(id,index,&ret);
        return ret;
    }
        
    string to_str(size_t n) {
        return fm::toString(n).str();
    }

    string to_str(double x) {
        return fm::toString(x,12).str();
    }

    template<class T>
    string to_str(vector<T> vec) {
        string s;
        for (size_t i=0;i<vec.size();++i) {
            if (i) s += ",";
            s += to_str(vec[i]);
        }
        return s;
    }
}

string FDGpu::getVal(string constant)
{
    if (constant == "RK_S") return to_str(rk().A.size());
    if (constant == "RK_A") return to_str(rk().A);
    if (constant == "RK_C_H") return to_str(rk().C_H);
    if (constant == "RK_C_E") return to_str(rk().C_E);
    if (constant == "NODES") return to_str(nodeCount());
    if (constant == "LOCAL_SIZE") return to_str(m_max_locals);
    
    if (constant == "SHARED_MEMORY_SIZE") return to_str(glget(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE));
    if (constant == "MAX_WORK_GROUP_INVOCATIONS") return to_str(glget(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS));
    if (constant == "MAX_WORK_GROUP_COUNT_X") return to_str(glget(GL_MAX_COMPUTE_WORK_GROUP_COUNT,0));
    if (constant == "MAX_WORK_GROUP_COUNT_Y") return to_str(glget(GL_MAX_COMPUTE_WORK_GROUP_COUNT,1));
    if (constant == "MAX_WORK_GROUP_COUNT_Z") return to_str(glget(GL_MAX_COMPUTE_WORK_GROUP_COUNT,2));
    if (constant == "MAX_WORK_GROUP_SIZE_X") return to_str(glget(GL_MAX_COMPUTE_WORK_GROUP_SIZE,0));
    if (constant == "MAX_WORK_GROUP_SIZE_Y") return to_str(glget(GL_MAX_COMPUTE_WORK_GROUP_SIZE,1));
    if (constant == "MAX_WORK_GROUP_SIZE_Z") return to_str(glget(GL_MAX_COMPUTE_WORK_GROUP_SIZE,2));
    
    return "";
}

string FDGpu::setConstants(string &source)
{
    string news;
    news.reserve(source.size()+6);
    
    C(source.size()) {
        if (source[i] == '#' && source.substr(i+1,6) == "define") {
            i += 7;
            size_t defEnd = i;
            i = source.find_first_not_of(" \t",i);
            if (source[i] != '\n' && source[i] != '\r') {
                auto k = source.find_first_of(" \t\n\r",i);
                string name = source.substr(i,k-i);
                string val = getVal(name);
                // cout << name << " --> " << val << endl;
                if (val == "") {
                    news += "#define";
                    i = defEnd-1;
                } else {
                    news += "#define " + name + " " + val;
                    i = source.find_first_of("\n\r",i)-1;
                }
            }
        } else {
            news += source[i];
        }
    }
    
    return news;
}

fm::Result FDGpu::loadCshader() {
    
    m_max_locals = min({nodeCount(),glget(GL_MAX_COMPUTE_WORK_GROUP_SIZE,0),glget(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS)});
    // m_max_locals = 2;
    m_logfunc(LOG_DATA,"Max local size is: " + toString(m_max_locals).str());
    
    string source_k,source_e,source_s;
    auto res = fm::file_content("shaders/fdgpu_k.glsl",source_k);
        res += fm::file_content("shaders/fdgpu_e.glsl",source_e);
        res += fm::file_content("shaders/fdgpu_rk_sum.glsl",source_s);
    
    if (!res) return res;
    
    source_k = setConstants(source_k);
    source_e = setConstants(source_e);
    source_s = setConstants(source_s);
    
    m_logfunc(LOG_DEBUG,"Source loaded (k): \n" + source_k);
    m_logfunc(LOG_DEBUG,"Source loaded (e): \n" + source_e);
    m_logfunc(LOG_DEBUG,"Source loaded (s): \n" + source_s);
    
    res += m_cshader_k.loadFromMemory(source_k);
    res += m_cshader_e.loadFromMemory(source_e);
    res += m_cshader_s.loadFromMemory(source_s);
    
    if (res) {
        m_logfunc(LOG_DATA,"Shaders loaded fine");
    }
    
    return res;
}

template<class T>
void FDGpu::dumpBuffer(Buffer &buf,string name,size_t w,string sep) {
    const T *ptr = buf.map<T>(fg::BufferReadOnly);
    cout << "Buffer \'" << name << "\' items: " << buf.getItemCount() << endl;
    C(buf.getItemCount()) {
        if (i % w == 0) {
            cout << i/w << ".: ";
        }
        cout << ptr[i] << sep;
        if ((i % w)+1 == w) {
            cout << endl;
        }
    }
    buf.unMap();
}

void FDGpu::prepareRun()
{
    auto s = nodeCount();
    
    m_posAcc = m_layout.positions;
    
    fm::Result res;
    res += m_posbuf.setData(&m_layout.positions[0],s);
    res += m_rkErrorbuf.setData((float*)nullptr,s);
    res += m_posOutbuf.setData((vec2*)nullptr,s);
    res += m_kbuf.setData((vec2*)nullptr,s * rk().A.size());
    res += initEdgeBuf();
    res += loadCshader(); // dependant on node count
    if (!res) {
        m_logfunc(LOG_ERROR,"prepareRun failed: " + res.toString());
        exit(1);
    }
}

fm::Result FDGpu::init_memory(const View &view)
{
    vector<float> nodeWts(view.size());
    C(view.size())
        nodeWts[i] = view.graph[i].body.mass;
    
    auto res = m_nodeWtBuf.setData(&nodeWts[0],nodeWts.size());
    res += createEdgeBufData(view);
    
    return res;
}

fm::Result FDGpu::dispatch()
{
    if (nodeCount() == 0) return {};
    
    fm::Result res;
    
    res += m_cshader_k.setUniform("h",(float)m_h);
    res += m_cshader_k.setUniform("squeeze",(float)m_squeeze);
    
    res += m_cshader_k.setStorageBuf(0, m_posbuf);
    res += m_cshader_k.setStorageBuf(1, m_edgebuf);
    res += m_cshader_k.setStorageBuf(2, m_kbuf);
    res += m_cshader_k.setStorageBuf(5, m_nodeWtBuf);
    
    for (int i=0;i<rk().A.size();++i) {
        res += m_cshader_k.setUniform("k_i",i);
        
        for (size_t acc_start=0;acc_start < nodeCount(); acc_start += m_max_locals) {
            
            res += m_cshader_k.setUniform("acc_start",int(acc_start));
        
            res += m_cshader_k.dispatch(vec3s(nodeCount(),1,1));
            
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }
    
    // vector<vec2> k0s(nodeCount() * rk().A.size());
    // m_kbuf.getData(&k0s[0],0,k0s.size());
    // for (auto f : k0s) m_logfunc(LOG_DATA,fm::toString(f.x,5).str() + ", " + fm::toString(f.y,5).str());
    // exit(0);
    
    
    
    res += m_cshader_s.setUniform("h",(float)m_h);
    
    res += m_cshader_s.setStorageBuf(0, m_posbuf);
    res += m_cshader_s.setStorageBuf(2, m_kbuf);
    res += m_cshader_s.setStorageBuf(3, m_posOutbuf);
    res += m_cshader_s.setStorageBuf(4, m_rkErrorbuf);
    
    res += m_cshader_s.dispatch(vec3s(nodeCount(),1,1));
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    
    
    res += m_cshader_e.setStorageBuf(4, m_rkErrorbuf);
    
    size_t times = (nodeCount() + m_max_locals - 1) / m_max_locals;
    
    for (size_t scale = 1;times > 0;scale *= m_max_locals) {
        res += m_cshader_e.setUniform("scale",(int)scale);
        res += m_cshader_e.dispatch(vec3s(times,1,1));
        
        if (times == 1) break;
        times = (times + m_max_locals - 1) / m_max_locals;
    }
    

    return res;
}

void FDGpu::sync_layout() {
    const vec2 *ptr = m_posbuf.map<vec2>(fg::BufferReadOnly);
    
    m_layout.positions.assign(ptr, ptr + m_layout.size());
    
    m_posbuf.unMap();
}

bool FDGpu::initWorker() {
    auto res = m_context.create();
    res += m_context.setActive();
    
    if (!res) {
        m_logfunc(LOG_ERROR,"Failed to set worker context up: " + res.toString());
    } else {
        m_logfunc(LOG_INFO,"Worker context created with OGL version: " + string((char*)glGetString(GL_VERSION)));
    }
    
    return res;
}

void FDGpu::finishWorker() {
    // tricky as the main thread might not have a gl context
    m_cshader_k.destroy();
    m_cshader_e.destroy();
    m_cshader_s.destroy();
    m_posbuf.destroy();
    m_edgebuf.destroy();
    m_kbuf.destroy();
    m_posOutbuf.destroy();
    m_rkErrorbuf.destroy();
    m_nodeWtBuf.destroy();
    m_context.destroy();
}

double distance(const vector<vec2> &bodiesFrom,const vector<vec2> &bodiesTo)
{
    double dist = -1;
    
    C(bodiesFrom.size()) {
        double d = (bodiesFrom[i] - bodiesTo[i]).LENGTH();
        if (d > dist) dist = d;
    }
    
    return sqrt(dist);
}

void FDGpu::calc_error()
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    
    float e;
    m_rkErrorbuf.getData(&e,0,1);
    m_e = e;
    
    m_logfunc(LOG_DEBUG, "m_e = " + fm::toString(m_e,12).str());
    
    double N = rk().eps >= m_e ? rk().N_h : rk().N_e;
    
    m_h = min<float>(1,0.9*pow(rk().eps / m_e,1.0/N)*m_h);
    m_logfunc(LOG_DEBUG, "m_h = " + fm::toString(m_h,12).str());
}

void FDGpu::update_bodies()
{
    m_logfunc(LOG_DEBUG, "starting update loop");
    do {
        ++m_its;
        dispatch();
        calc_error();
        
    } while (m_e >= rk().eps);
    
    m_logfunc(LOG_DEBUG, "update loop finished");
    
    if (m_e == 0) {
        finished = true;
        return;
    }
    
    const vec2 *nptr = m_posOutbuf.map<vec2>(fg::BufferReadOnly);
    const vec2 *optr = m_posbuf.map<vec2>(fg::BufferReadOnly);
    
    float travel = 0;
    float all_travel = 0;
    C(nodeCount()) {
        m_posAcc[i] = m_posAcc[i]*.6 + (nptr[i] - optr[i]);
        travel = max(travel, m_posAcc[i].length());
        all_travel += (nptr[i] - optr[i]).length();
    }
    m_s = travel;
    // cout << err/m_h << endl;
    
    m_posbuf.unMap();
    m_posOutbuf.unMap();
    
    // m_logfunc(LOG_INFO, fm::toString(travel,5).str());
    
    m_posbuf.swap(m_posOutbuf);
}