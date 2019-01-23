#ifndef GVIEW_LAYOUTGEN
#define GVIEW_LAYOUTGEN

#include <future>

#include <Frontier.hpp>
using namespace std;

#include "Layout.hpp"
#include "View.hpp"

struct LayoutGen
{
    void start(View &view);
    void stop();
    bool ready() const;
    void wait(Time timeout = Time::Inf);
    
    Layout layout() const;
    
    virtual ~LayoutGen();
    
private:
	atomic<bool> m_stopped;
	future<void> m_fut;
    void gen_init(View &view);

protected:
	Layout m_layout;
    mutable std::mutex m_layoutMut;
    virtual void init(View &view) = 0;
    virtual void run() = 0;
    
    bool stopped() const {return m_stopped;}
};

#endif