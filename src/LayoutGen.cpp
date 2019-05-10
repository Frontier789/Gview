#include "LayoutGen.hpp"
#include <iostream>
using namespace std;

LayoutGen::LayoutGen(Delegate<void,LogLevel,string> logfun) :
    m_stopped(false),
    m_killed(false),
    m_input(false),
    m_layoutSize(1),
    m_logfunc(logfun)
{
    
}
    
void LayoutGen::startWorkerThread()
{
    if (!m_thread.joinable()) {
        m_thread = thread([&]{
            if (!initWorker()) return;
            
            while (true)
            {
                unique_lock<mutex> lock(m_inputMut); // wait for input
                m_inputCond.wait(lock, [&]{
                    return m_killed || m_input;
                });
                
                if (m_killed) break; // break if killed
                
                m_input = false;
                
                // run until stopped or finished
                this->run();
                m_genPromise.set_value();
            }
            
            finishWorker();
        });
    }
}

template<class T>
bool ready(T &future)
{
    return future.wait_for(std::chrono::duration<double>(0)) == std::future_status::ready;
}
    
void LayoutGen::killWorker()
{
    if (m_killed) return;
    
    m_killed = true;
    m_inputCond.notify_all();
    
    stop();
    
    m_thread.join();
}

LayoutGen::~LayoutGen()
{
    killWorker();
}

fm::Result LayoutGen::setTarget(const View &view)
{
    return setTarget(view,Layout());
}

fm::Result LayoutGen::setTarget(const View &view,Layout cached)
{
    stop();
    fm::Result res;
    {
        lock_guard<mutex> lock(m_inputMut);

        res = setup(view,std::move(cached));
        
        m_input      = true;
        m_stopped    = false;
        m_genPromise = promise<void>();
        m_genFut     = m_genPromise.get_future();
    }
    m_inputCond.notify_all();
    
    return res;
}

bool LayoutGen::stopped() const
{
    return m_stopped;
}

bool LayoutGen::ready() const
{
    return m_genFut.valid() && ::ready(m_genFut);
}

void LayoutGen::stop()
{
	m_stopped = true;
    
    wait();
}

void LayoutGen::wait(fm::Time timeout)
{
	if (m_genFut.valid() && !m_killed) {
        if (timeout == fm::Time::Inf)
		    m_genFut.wait();
        else
		    m_genFut.wait_for(std::chrono::duration<double>(timeout.s()));
    }
}

Layout LayoutGen::layout()
{
    lock_guard<mutex> guard(m_layoutMut);
    return m_layout;
}

fm::Result LayoutGen::setup(const View &view,Layout cached)
{
    m_layoutSize = view.size();
    
    bool cache = cached.size() == nodeCount();
    if (cache)
        m_layout = std::move(cached);
    else
        m_layout = view.getLayout();
    
    return init(view,cache);
}
