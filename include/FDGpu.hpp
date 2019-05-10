#ifndef GVIEW_FDGPU
#define GVIEW_FDGPU

#include <Frontier.hpp>
using namespace std;

#include "FDLayoutGen.hpp"

struct FDGpu : public FDLayoutGen
{
    FDGpu(RKDesc rk = RKDesc::fehlberg(),bool do_logging = false,Delegate<void,LogLevel,string> logfun = nullptr) : FDLayoutGen(rk,do_logging,logfun) {}
    
    ~FDGpu() {killWorker();}
    
    template<class T>
    void dumpBuffer(Buffer &buf,string name,size_t w,string sep = " ");

protected:
    Buffer m_edgebuf;
    
    fm::Result init_memory(const View &view) override;
    void update_bodies() override;
    
    virtual fm::Result createEdgeBufData(const View &view) = 0;
    virtual fm::Result initEdgeBuf() = 0;
    
private:
    ComputeShader m_cshader_k;
    ComputeShader m_cshader_s;
    ComputeShader m_cshader_e;
    Buffer m_posbuf;
    Buffer m_kbuf;
    Buffer m_posOutbuf;
    Buffer m_rkErrorbuf;
    Buffer m_nodeWtBuf;
    vector<vec2> m_posAcc;
    size_t m_max_locals;
    
    GLContext m_context;
    
    void sync_layout() override;
    bool initWorker() override;
    void finishWorker() override;
    void prepareRun() override;
    
    void calc_error();
    
    string getVal(string constant);
    
    fm::Result dispatch();
    string setConstants(string &source);
    
    fm::Result loadCshader();
};

#endif