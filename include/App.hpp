#ifndef GVIEW_APP
#define GVIEW_APP

#include <Frontier.hpp>

#include "CacheManager.hpp"
#include "ViewPlotter.hpp"
#include "LayoutGen.hpp"
#include "LogLevel.hpp"
#include "Importer.hpp"
#include "RKDesc.hpp"

struct App {
    App(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache);
    virtual ~App();
    
protected:
    GuiWindow win;
    Importer *m_importer;
    ViewPlotter *plotter;
    LayoutGen *m_gen;
    CacheManager m_cache;
    Clock m_genclk;
    Time m_gentimeout;
    CacheId m_currCacheId;
    Delegate<void,LogLevel,string> logFunc;
    bool enable_gpu;
    bool enable_cache;
    View m_curView;
    
    virtual void createImporter();
    virtual void createLayoutGen();
    void saveCurrCache();

    void addKeyHandler();
    void updateLayout();
    void waitLayoutGen(Time t = Time::Inf);
    
public:
    virtual Importer *importer() {return m_importer;}
    virtual LayoutGen *layoutGen() {return m_gen;}
    virtual fm::Result init();
    virtual void update();
    void run();
    void stop();
    
    virtual Layout fetchCache(CacheId id) const;
    
    void loadCurrentView();
    void reloadCurrentView();
};

#endif