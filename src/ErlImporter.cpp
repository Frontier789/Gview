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

ErlImporter::ErlImporter(Delegate<void,string> logfunc) : 
    m_logfunc(std::move(logfunc)),
    m_input(false)
{
    m_logfunc("ErlImporter created");
    prepareStdio();
    
    m_closed = std::async(std::launch::async, [&]{
        int c;
        
        do {
            {
                std::lock_guard<std::mutex> guard(m_cin_mut);
                c = cin.peek();
            }
            m_input = true;
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

namespace
{
    class EdgePalette
    {
    public:
        float width;
        vec3 color;
        NodeShape endShapes[2];
        
        EdgePalette() : width(1), endShapes{NodeShape::None,NodeShape::None} {}
        
        void load(const vector<size_t> &arr)
        {
            if (arr.size() < 2) return;
            width = arr[1] / float(arr[0]);
            
            if (arr.size() < 5) return;
            color = vec3(arr[2],arr[3],arr[4])/255;
            
            if (arr.size() < 6) return;
            endShapes[0] = NodeShape(arr[5]);
            
            if (arr.size() < 7) return;
            endShapes[1] = NodeShape(arr[6]);
        }
    };
}

void ErlImporter::recv(View &view,Delegate<void,string> logFunc)
{
    vector<size_t> counts = recv<vector<size_t>>();
    
    logFunc("Receiving view from Erlang");
    size_t countReq = 2;
    
    if (counts.size() != countReq)
    {
        m_logfunc("Warning: recevied " + fm::toString(counts.size()) + " numbers for header instead of " + fm::toString(countReq).str());
        counts.resize(countReq,0);
    }
    else
    {
        logFunc("Received " + fm::toString(counts.size()).str() + " numbers as header (should be " + fm::toString(countReq).str() + ")");
    }
    
    size_t paleN = counts[0];
    size_t nodeN = counts[1];
    
    size_t nodeBase = view.size();
    
    view.graph.resize(nodeN + nodeBase);
    vector<EdgePalette> palettes(paleN);
    
    logFunc("Palette count: " + fm::toString(paleN).str());
    C(paleN)
    {
        auto palData = recv<vector<size_t>>();
        
        string palStr;
        for (auto j : palData) palStr += " " + fm::toString(j).str();
        logFunc("received palette " + fm::toString(i).str() + ":" + palStr);
        
        palettes[i].load(palData);
    }
    
    logFunc("Receiving vertices: " + fm::toString(nodeN).str());
    C(nodeN) {
        auto &visuals = view.graph[i + nodeBase].visuals;
        visuals.label.text = recv<string>();
    }
    
    vector<size_t> Nwts   = recv<vector<size_t>>();
    vector<size_t> Ntypes = recv<vector<size_t>>();
    vector<size_t> Ndegs  = recv<vector<size_t>>();
    
    logFunc("Building graph structure");
    C(nodeN) {
        auto &node = view.graph[i + nodeBase];
        node.body.mass = i < Nwts.size() ? Nwts[i] : 1;
        
        node.visuals.size  = i < Ntypes.size() ? palettes[Ntypes[i]].width : 10;
        node.visuals.shape = i < Ntypes.size() ? palettes[Ntypes[i]].endShapes[0] : NodeShape::Circle;
        
        node.edges.resize(Ndegs[i]);
    }
    
    logFunc("Directing edges");
    C(nodeN) {
        auto &node = view.graph[i + nodeBase];
        
        vector<size_t> to_ids = recv<vector<size_t>>();
        Cx(to_ids.size())
            node.edges[x].to = to_ids[x];
    }
    
    C(nodeN) {
        auto &node = view.graph[i + nodeBase];
        
        logFunc("Node #" + fm::toString(i).str());
        logFunc("  is of degree " + fm::toString(Ndegs[i]).str());
        logFunc("  has label " + node.visuals.label.text);
        Cf(node.edges.size(),j) {
            logFunc("  --> " + fm::toString(node.edges[j].to).str());
            if (j > 5) {
                logFunc("  ...");
            }
        }
        
        if (i > 5) {
            logFunc("...");
            i = nodeN;
        }
    }
    
    C(nodeN) {
        auto &node = view.graph[i + nodeBase];
        
        vector<size_t> Ewts = recv<vector<size_t>>();
    }
    
    C(nodeN) {
        auto &node = view.graph[i + nodeBase];
        
        vector<size_t> Etypes = recv<vector<size_t>>();
        Cx(Etypes.size())
            node.edges[x].visuals.width = palettes[Etypes[x]].width;
    }
    
    logFunc("");
}

void ErlImporter::recv(size_t &length)
{
    Byte bytes[4];
    recv((void*)bytes,4);
    length = intFromBytes(bytes);
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
    }
    m_input = false;
    
    if (is_eof || is_bad || is_fail) {
        string problem = "";
        if (is_eof) problem += "eof";
        if (is_bad) problem += " bad";
        if (is_fail) problem += " fail";
        
        m_res += fm::Result("IOError",fm::Result::OPFailed,"ReadFailed","recv()",__FILE__,__LINE__,"reading from stdin set bit(s): " + problem);
    }
}
