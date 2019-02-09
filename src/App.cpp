#include "App.hpp"
#include "GridImporter.hpp"
#include "FDCpu.hpp"

App::App(Delegate<void,string> logFunc) :
    m_importer(nullptr),
    plotter(nullptr),
    m_gentimeout(milliseconds(1000)),
    logFunc(logFunc)
{
    
}

App::~App() {
    delete m_importer;
}

void App::updateLayout()
{
    Layout l = layoutGen()->layout();
    plotter->setLayout(std::move(l));
}

void App::loadCurrentView(CacheId cacheId)
{
    m_currCacheId = cacheId;
    
    plotter->setView(importer()->view());
    m_genclk.restart();
    m_genclk.resume();
    
    layoutGen()->start(plotter->view(),m_cache.load(cacheId));
    updateLayout();
}

void App::init() {}

void App::update() {
    if (m_genclk.getTime() > m_gentimeout && !m_genclk.isPaused()) {
        updateLayout();
    }
    
    if (!m_genclk.isPaused() && layoutGen()->ready()) {
        updateLayout();
        m_genclk.pause();
        m_cache.save(m_currCacheId,layoutGen()->layout());
        logFunc("Generation done 🦆");
        logFunc("\tt = " + fm::toString(m_genclk.s()*1000).str() + "ms");
    }
}

void App::run() {
    win.open(vec2(640,480),"win");
    
    plotter = new ViewPlotter(win);
    win.addElement(plotter);
    m_gen = new FDCpu(RKDesc::fehlberg());
    
    init(); // init may need a GL context and the plotter
    
    win.runGuiLoop(Delegate<void>(App::update,this));
}
