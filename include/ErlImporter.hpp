#ifndef GVIEW_ERLIMPORTER
#define GVIEW_ERLIMPORTER

#include "LogLevel.hpp"
#include "Importer.hpp"
#include <future>
#include <atomic>
#include <mutex>

typedef unsigned char Byte;

struct ErlImporter : public Importer {
    ErlImporter(Delegate<void,LogLevel,string> logfunc);
    
    virtual View view() {return recv<View>(m_logfunc);}
    
    Result check() const {return m_res;}
    bool closed() const;
    bool hasInput() const {return m_input;}

public:
    Delegate<void,LogLevel,string> m_logfunc;
    
    void prepareStdio();
    
    void recv(View &view,Delegate<void,LogLevel,string> logFunc = [](string){});
    void recv(size_t &length);
    void recv(vector<size_t> &arr);
    void recv(vector<float> &arr);
    void recv(string &str);
    void recv(void *data,size_t bytes);
    
	void send(size_t num);
	void send(string str);
	void send(const vector<float> &arr);
	void send(const void *data,size_t bytes);
    
    template<class T>
    T recv() {T t; recv(t); return t;}
    template<class T,class... Extra>
    T recv(Extra... extra) {T t; recv(t,extra...); return t;}

private:
    fm::Result m_res;
    std::mutex m_cin_mut; // Note: reading cin is only thread safe if it is synchronised with stdio 
    std::future<void> m_closed;
    std::atomic<bool> m_input;
    
    std::vector<View::Edge::Visuals> edge_palette;
    std::vector<View::Node::Visuals> node_palette;
};

#endif