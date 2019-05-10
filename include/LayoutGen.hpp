#ifndef GVIEW_LAYOUTGEN
#define GVIEW_LAYOUTGEN

#include <future>
#include <mutex>

#include <Frontier.hpp>
using namespace std;

#include "LogLevel.hpp"
#include "Layout.hpp"
#include "View.hpp"

struct LayoutGen
{
    LayoutGen(Delegate<void,LogLevel,string> logfun);
    
    fm::Result setTarget(const View &view);
    fm::Result setTarget(const View &view,Layout cached);
    void stop();
    bool ready() const;
    bool stopped() const;
    void wait(Time timeout = Time::Inf);
    size_t nodeCount() const {return m_layoutSize;}
    
    void startWorkerThread();
    
    Layout layout();
    
    virtual ~LayoutGen();
    
    virtual string tag() const {return "layout_gen";}
    
private:
    condition_variable m_inputCond;
    promise<void> m_genPromise;
    atomic<bool>  m_stopped;
    atomic<bool>  m_killed;
    future<void>  m_genFut;
    atomic<bool>  m_input;
    mutex  m_inputMut;
    thread m_thread;

    fm::Result setup(const View &view,Layout cached);

protected:
	Layout m_layout;
    mutex  m_layoutMut;
    size_t m_layoutSize;
    Delegate<void,LogLevel,string> m_logfunc;
    
    virtual bool initWorker() {return true;}
    virtual void finishWorker() {};
    virtual fm::Result init(const View &view,bool cached) = 0;
    virtual void run() = 0;
    
    void killWorker();
};

#endif