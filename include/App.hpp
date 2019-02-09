#ifndef GVIEW_APP
#define GVIEW_APP

#include <Frontier.hpp>

#include "CacheManager.hpp"
#include "ViewPlotter.hpp"
#include "LayoutGen.hpp"
#include "Importer.hpp"
#include "RKDesc.hpp"
#include "Random.hpp"

struct App {
    App(Delegate<void,string> logFunc);
    virtual ~App();
    
    GuiWindow win;
    Importer *m_importer;
    ViewPlotter *plotter;
    LayoutGen *m_gen;
    CacheManager m_cache;
    Clock m_genclk;
    Time m_gentimeout;
    CacheId m_currCacheId;
    Delegate<void,string> logFunc;
    
    virtual Importer *importer() {return m_importer;}
    virtual LayoutGen *layoutGen() {return m_gen;}
    virtual void init();
    virtual void update();
    void run();
    
    void loadCurrentView(CacheId cacheId = "");
    void updateLayout();
};

#endif