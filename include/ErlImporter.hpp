#ifndef GVIEW_ERLIMPORTER
#define GVIEW_ERLIMPORTER

#include "Importer.hpp"
#include <future>
#include <atomic>

typedef unsigned char Byte;

struct ErlImporter : public Importer {
    ErlImporter(Delegate<void,std::string> logfunc);
    
    virtual View view() {return recv<View>(m_logfunc);}
    
    Result check() const {return m_res;}
    bool closed() const;

public:
    Delegate<void,std::string> m_logfunc;
    
    void prepareStdio();
    
    void recv(View &view,Delegate<void,std::string> logFunc = [](std::string){});
    void recv(size_t &length);
    void recv(std::vector<size_t> &arr);
    void recv(std::string &str);
    void recv(void *data,size_t bytes);
    
    
    template<class T>
    T recv() {T t; recv(t); return t;}
    template<class T,class... Extra>
    T recv(Extra... extra) {T t; recv(t,extra...); return t;}
    
    fm::Result m_res;
    std::future<void> m_closed;
    std::atomic<bool> m_input;
};

#endif