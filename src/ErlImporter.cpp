#include "ErlImporter.hpp"
#include <FRONTIER/System/util/OS.h>
#include <fstream>
#include <iostream>

using std::cin;
using std::cout;
using std::string;
using std::vector;

#ifdef FRONTIER_OS_WINDOWS
	#include <io.h>
	#include <fcntl.h>
#endif


ErlImporter::ErlImporter(Delegate<void,LogLevel,string> logfunc) : 
    m_logfunc(std::move(logfunc)),
    m_input(false)
{
    prepareStdio();
    
    m_closed = std::async(std::launch::async, [&]{
        int c;
        
        do {
            if (!m_input) {
                std::lock_guard<std::mutex> guard(m_cin_mut);
                c = cin.peek();
                m_input = true;
            }
            fm::Sleep(milliseconds(10));
        } while (c != std::char_traits<char>::eof());
    });
}

bool ErlImporter::closed() const
{
    return m_closed.wait_for(std::chrono::duration<double>(0)) == std::future_status::ready;
}

void ErlImporter::prepareStdio()
{
    std::ios_base::sync_with_stdio(false);
    #ifdef FRONTIER_OS_WINDOWS
        _setmode(_fileno(stdin), _O_BINARY);
        _setmode(_fileno(stdout), _O_BINARY);
    #endif
}

namespace
{
    Byte getByte(size_t integer,int byte)
    {
        integer >>= (byte*8);
        return (Byte)integer;
    }
    
    size_t intFromBytes(Byte *bytes)
    {
        return ((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0));
    }
    
    void bytesFromInt(size_t num,Byte *bytes)
    {
        bytes[0] = Byte((num >> 24) & 0xff);
        bytes[1] = Byte((num >> 16) & 0xff);
        bytes[2] = Byte((num >>  8) & 0xff);
        bytes[3] = Byte((num >>  0) & 0xff);
    }
}

View::Edge::Visuals loadEdgeVisuals(const vector<size_t> &arr = vector<size_t>())
{
    View::Edge::Visuals props{3,NodeShape::None,NodeShape::None,vec4::Black};
    
    if (arr.size() < 2) return props;
    props.width = arr[1] / float(arr[0]);
    
    if (arr.size() < 5) return props;
    props.color = vec3(arr[2],arr[3],arr[4])/255;
    
    if (arr.size() < 6) return props;
    props.end1 = NodeShape(arr[5]);
    
    if (arr.size() < 7) return props;
    props.end2 = NodeShape(arr[6]);
    
    return props;
}

View::Node::Visuals loadNodeVisuals(const vector<size_t> &arr = vector<size_t>())
{
    View::Node::Visuals props{10,NodeShape::Circle};
    
    if (arr.size() < 2) return props;
    props.size = arr[1] / float(arr[0]);
    
    if (arr.size() < 3) return props;
    props.shape = NodeShape(arr[2]);
    
    return props;
}

void ErlImporter::recv(View &view,Delegate<void,LogLevel,string> logFunc)
{
    vector<size_t> counts = recv<vector<size_t>>();
    
    logFunc(LOG_DATA,"Receiving view from Erlang");
    size_t countReq = 4;
    
    if (counts.size() != countReq)
    {
        m_logfunc(LOG_WARNING,"Recevied " + fm::toString(counts.size()) + " numbers for header instead of " + fm::toString(countReq).str());
        counts.resize(countReq,0);
    }
    else
    {
        logFunc(LOG_DATA,"Received " + fm::toString(counts.size()).str() + " numbers as header (should be " + fm::toString(countReq).str() + ")");
    }
    
    size_t nodes = counts[0];
    size_t npalS = counts[1];
    size_t epalS = counts[2];
    size_t seleN = counts[3];
    
    size_t nodeBase = view.size();
    
    view.graph.resize(nodes + nodeBase);
    
    logFunc(LOG_DATA,"Node palette size: " + fm::toString(npalS).str());
    node_palette.resize(npalS);
    C(npalS)
    {
        auto palData = recv<vector<size_t>>();
        
        string palStr;
        for (auto j : palData) palStr += " " + fm::toString(j).str();
        logFunc(LOG_DATA,"received visual " + fm::toString(i).str() + ":" + palStr);
        
        node_palette[i] = loadNodeVisuals(palData);
    }
    
    logFunc(LOG_DATA,"Edge palette size: " + fm::toString(epalS).str());
    edge_palette.resize(epalS);
    C(epalS)
    {
        auto palData = recv<vector<size_t>>();
        
        string palStr;
        for (auto j : palData) palStr += " " + fm::toString(j).str();
        logFunc(LOG_DATA,"received visual " + fm::toString(i).str() + ":" + palStr);
        
        edge_palette[i] = loadEdgeVisuals(palData);
    }
    
    logFunc(LOG_DATA,"Vertex count: " + fm::toString(nodes).str());
    C(nodes) {
        auto &node = view.graph[i + nodeBase];
        node.label.text = recv<string>();
        logFunc(LOG_DEBUG, "Label #" + fm::toString(i).str() + " is "  + node.label.text);
    }
    C(nodes) {
        auto &node = view.graph[i + nodeBase];
        node.label.tooltip = recv<string>();
        logFunc(LOG_DEBUG, "Tooltip #" + fm::toString(i).str() + " is "  + node.label.tooltip);
    }
    
    vector<size_t> SelCnt = recv<vector<size_t>>(); logFunc(LOG_DATA, "Selector counts received");
    vector<string> selLabels(seleN);
    for (auto &label : selLabels) {
        label = recv<string>();
    }
    
    logFunc(LOG_DATA, "Received " + fm::toString(SelCnt.size()).str() + " sel counts:");
    for (auto n : SelCnt) {
        logFunc(LOG_DATA, fm::toString(n).str());
    }
    logFunc(LOG_DATA, "Their " + fm::toString(seleN).str() + " labels: ");
    for (auto &label : selLabels) {
        logFunc(LOG_DATA, label);
    }
    
    
    
    vector<size_t> Edges  = recv<vector<size_t>>(); logFunc(LOG_DATA, "Edges received");
    vector<float>  Nwts   = recv<vector<float>>();  logFunc(LOG_DATA, "Nwts received");
    vector<size_t> Ntypes = recv<vector<size_t>>(); logFunc(LOG_DATA, "Ntypes received");
    vector<float>  Ewts   = recv<vector<float>>();  logFunc(LOG_DATA, "Ewts received");
    vector<size_t> Etypes = recv<vector<size_t>>(); logFunc(LOG_DATA, "Etypes received");
    
    if (Edges.size() % 2 != 0) {
        logFunc(LOG_WARNING,"Warning: number edge ends is odd");
        Edges.pop_back();
    }
    
    size_t edges = Edges.size()/2;
    
    Nwts.resize(nodes, 1);
    Ntypes.resize(nodes, 0);
    Ewts.resize(edges, 1);
    Etypes.resize(edges, 0);
    
    logFunc(LOG_DATA,"Building graph structure");
    {
        auto selEnds = SelCnt;
        
        for (size_t i=1;i<selEnds.size();i++) {
            selEnds[i] += selEnds[i-1];
        }
        
        C(nodes) {
            auto &node = view.graph[i + nodeBase];
            node.body.mass = Nwts[i];
            
            auto type = Ntypes[i];
            if (type < node_palette.size())
                node.visuals = node_palette[type];
            else node.visuals = loadNodeVisuals();
            
            if (type < SelCnt.size()) {
                node.selectors.resize(SelCnt[type]);
                size_t id = selEnds[type] - SelCnt[type];
                for (auto &selector : node.selectors) {
                    selector.id = id;
                    selector.label = selLabels[id];
                    id++;
                }
            }
        }
    }
    
    logFunc(LOG_DATA,"Adding edges");
    C(edges) {
        auto a = Edges[i*2+0];
        auto b = Edges[i*2+1];
        
        logFunc(LOG_DATA, fm::toString(a).str() + " --> " + fm::toString(b).str());
        
        auto type = Etypes[i];
        auto vis  = type < edge_palette.size() ? edge_palette[type] : loadEdgeVisuals();
        view.graph[a + nodeBase].edges.push_back({vis,b + nodeBase,Ewts[i]});
    }
    
    logFunc(LOG_DATA,"");
}

void ErlImporter::recv(size_t &length)
{
    Byte bytes[4];
    recv((void*)bytes,4);
    length = intFromBytes(bytes);
}

void ErlImporter::recv(vector<float> &arr)
{
    size_t byteLen;
    recv(byteLen);
    
    arr.resize(byteLen/4);
    recv(&arr[0],byteLen);
}

void ErlImporter::recv(vector<size_t> &arr)
{
    size_t byteLen;
    recv(byteLen);
    
    vector<Byte> raw(byteLen);
    recv(&raw[0],byteLen);
    
    arr.resize(byteLen/4);
    for (size_t i=0;i<byteLen/4;++i)
        arr[i] = intFromBytes(&raw[4*i]);
}

void ErlImporter::recv(string &str)
{
    size_t len;
    recv(len);
    
    str.resize(len);
    recv(&str[0],len);
}

void ErlImporter::recv(void *data,size_t bytes)
{
    bool is_eof,is_bad,is_fail;
    {
        std::lock_guard<std::mutex> guard(m_cin_mut);
        cin.read((char*)data,bytes);
        is_eof = cin.eof();
        is_bad = cin.bad();
        is_fail = cin.fail();
        m_input = false;
    }
    
    if (is_eof || is_bad || is_fail) {
        string problem = "";
        if (is_eof) problem += "eof";
        if (is_bad) problem += " bad";
        if (is_fail) problem += " fail";
        
        m_res += fm::Result("IOError",fm::Result::OPFailed,"ReadFailed","recv()",__FILE__,__LINE__,"reading from stdin set bit(s): " + problem);
    }
}

void ErlImporter::send(size_t num)
{
    Byte bytes[4];
    bytesFromInt(num,bytes);
    
    send((void*)bytes,4);
}

void ErlImporter::send(string str)
{
    send(&str[0],str.size());
}

void ErlImporter::send(const vector<float> &arr)
{
    send(&arr[0],arr.size() * sizeof(arr[0]));
}

void ErlImporter::send(const void *data,size_t bytes)
{
    // (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3] << 0);
    
    Byte bytesArray[4] = {getByte(bytes,3),getByte(bytes,2),getByte(bytes,1),getByte(bytes,0)};
    cout.write((char*)bytesArray,4);
    cout.write((char*)data,bytes);
    cout.flush();
    
    if (!cout.good()) m_res += fm::Result("IOError",fm::Result::OPFailed,"WriteFailed","send()",__FILE__,__LINE__,"writting to stdout set resulted in error");
}
