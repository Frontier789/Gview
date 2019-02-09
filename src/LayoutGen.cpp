#include "LayoutGen.hpp"

template<class T>
bool ready(T &future)
{
    return future.wait_for(std::chrono::duration<double>(0)) == std::future_status::ready;
}

LayoutGen::~LayoutGen()
{
    stop();
}

void LayoutGen::start(const View &view)
{
    start(view,Layout());
}

void LayoutGen::start(const View &view,Layout cached)
{
    if (m_fut.valid() && !::ready(m_fut))
        wait();
    
    m_stopped = false;
    
    gen_init(view,std::move(cached));
    init(view);
    
    m_fut = std::async(std::launch::async, [this](){
        this->run();
    });
}

bool LayoutGen::ready() const
{
    return m_fut.valid() && ::ready(m_fut);
}

void LayoutGen::stop()
{
	m_stopped = true;
	wait();
}

void LayoutGen::wait(fm::Time timeout)
{
	if (m_fut.valid()) {
        if (timeout == fm::Time::Inf)
		    m_fut.wait();
        else
		    m_fut.wait_for(std::chrono::duration<double>(timeout.s()));
    }
}

Layout LayoutGen::layout() const
{
    std::lock_guard<std::mutex> guard(m_layoutMut);
    return m_layout;
}

void LayoutGen::init_layout(const View &view)
{
    
}

void LayoutGen::gen_init(const View &view,Layout cached)
{
    if (cached.size() == view.size()) {
        m_layout = std::move(cached);
    } else {
        m_layout = view.getLayout();
        init_layout(view);
    }
}
