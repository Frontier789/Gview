#include "FDLayoutGen.hpp"
#include <iostream>
using namespace std;
#include <queue>

FDLayoutGen::FDLayoutGen(RKDesc rk,bool do_logging,Delegate<void,LogLevel,string> logfun) : 
    LayoutGen(logfun),
    m_h(0),
    m_s(1),
    m_rk(rk),
    m_squeeze(0)
{
    log.do_log = do_logging;
}

fm::Result FDLayoutGen::init_layout(const View &view)
{
    size_t n = view.size();
    if (n == 0) return {};
    
    auto &pos = m_layout.positions;
    
    vector<size_t> seq(n);
    std::iota(seq.begin(), seq.end(), 0);
    
    std::mt19937 g(n);
    std::shuffle(seq.begin(), seq.end(), g);
    
    Cf(n,id)
		pos[id] = pol2(n,fm::deg(float(seq[id]) / n * 360));
		
        // pos[id] = pol2((id*10)%n*30+30,fm::deg(float(id) / n * 360));
        // pos[id] = pol2(n*30,fm::deg(float(id) / n * 360));
        
    return {};
}

fm::Result FDLayoutGen::init(const View &view,bool cached)
{
    if (!cached)
        init_layout(view);
    
    m_h = m_rk.h0;
    m_squeeze = 0;
    if (!view.isJoint()) {
        m_squeeze = 1;
        m_logfunc(LOG_INFO,"View is disjoint, using squeeze force of " + fm::toString(m_squeeze,3).str());
    }
    else
        m_logfunc(LOG_DATA,"View is joint, yay!");
    
    return init_memory(view);
}

void FDLayoutGen::append_log()
{
    if (log.do_log) {
        log.h.push_back(m_h);
        log.e.push_back(m_e);
        log.s.push_back(m_s);
        log.its.push_back(m_its);
    }
}

void FDLayoutGen::run()
{
    if (nodeCount() <= 1) return;
    
    finished = false;
    
    prepareRun();
    
    for (size_t step=0;!stopped() && !finished;++step)
    {
        m_its = 0;
        update_bodies();
        append_log();
        
        if (m_s < 1e-2 && step > 30)
            finished = true;
        
        lock_guard<mutex> guard(m_layoutMut);
        sync_layout();
    }
}
