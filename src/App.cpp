#include "App.hpp"
#include "GridImporter.hpp"
#include "FDCpu.hpp"
#include "FDGpuMat.hpp"
#include "LayeredLayout.hpp"
#include <fstream>
#include <iostream>
using namespace std;

App::App(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache) :
    m_importer(nullptr),
    m_gen(nullptr),
    plotter(nullptr),
    m_gentimeout(milliseconds(1000)),
    logFunc(logFunc),
    enable_gpu(enable_gpu),
    enable_cache(enable_cache)
{
    // never call virtual methods here
}

App::~App() {
    delete m_gen;
    delete m_importer;
}

void App::updateLayout()
{
    Layout l = layoutGen()->layout();
    plotter->setLayout(std::move(l));
}

Layout App::fetchCache(CacheId id) const
{
    if (enable_cache)
        return m_cache.load(id + "_" + m_gen->tag());
    return {};
}

void App::reloadCurrentView()
{
    plotter->setView(m_curView);
    m_genclk.restart();
    m_genclk.resume();
    
    auto res = layoutGen()->setTarget(plotter->view(),fetchCache(m_currCacheId));
    if (!res) {
        logFunc(LOG_ERROR,"Starting layout generation failed: " + res.toString());
        stop();
        return;
    }
    
    layoutGen()->wait(milliseconds(30));
    
    // updateLayout();
}

void App::saveCurrCache()
{
    m_cache.save(m_currCacheId + "_" + m_gen->tag(),layoutGen()->layout());
}

void App::loadCurrentView()
{
    if (m_currCacheId != CacheId())
        saveCurrCache();
    
    m_curView     = importer()->view();
    m_currCacheId = m_curView.getHash();
    
    logFunc(LOG_DATA, "Current cache id is: " + m_currCacheId);
    
    reloadCurrentView();
}

fm::Result App::init() {return {};}

void App::update() {
    if (m_genclk.getTime() > m_gentimeout && !m_genclk.isPaused()) {
        updateLayout();
    }
    
    if (!m_genclk.isPaused() && layoutGen()->ready()) {
        updateLayout();
        m_genclk.pause();
        saveCurrCache();
        logFunc(LOG_INFO,"Generation done");
        logFunc(LOG_INFO,"\tt = " + fm::toString(m_genclk.s()*1000).str() + "ms");
    }
}

void App::createImporter() {m_importer = new GridImporter(3);}
void App::createLayoutGen() {
    if (enable_gpu && !ComputeShader::isAvailable()) {
        logFunc(LOG_WARNING, "Compute shaders not available, GPU will not be used!");
        enable_gpu = false;
    }
    if (enable_gpu) {
        m_gen = new FDGpuMat(RKDesc::fehlberg(),false,logFunc);
    } else {
        m_gen = new FDCpu(RKDesc::fehlberg(),false,logFunc);
    }
}
    
void App::addKeyHandler()
{
    Widget *ctl = new Widget(win);
    ctl->onkeypress = [&](Keyboard::Key key){
        if (key == Keyboard::Escape)
            stop();
        if (key == Keyboard::C)
            plotter->center();
        if (key == Keyboard::Enter) {
            Image img = win.capture();
            string name = "gview_screenshot0.png";
            bool available = !ifstream(name.c_str()).good();
            
            for (int i=1;!available;++i) {
                name = "gview_screenshot" + fm::toString(i).str() + ".png";
                available = !ifstream(name.c_str()).good();
            }
            
            img.saveToFile(name);
        }
    };
    win.addElement(ctl);
}

void App::stop() {
    win.stopGuiLoop();
}

void App::run() {
    win.open(vec2(640,480),"win");
    addKeyHandler();
    
    
    plotter = new ViewPlotter(win, logFunc);
    win.addElement(plotter);
    
    auto res = plotter->loadShaders();
    
    
    createLayoutGen();
    createImporter();
    
    m_gen->startWorkerThread();
    
    res += init(); // init may need a GL context and the plotter, the gen and importer
    
    if (!res) {
        logFunc(LOG_ERROR,"Init failed: " + res.toString());
        return;
    }
    
    win.runGuiLoop(Delegate<void>(&App::update,this));
    
    logFunc(LOG_INFO,"gui loop finished");
    
    if (!layoutGen()->ready()) {
        updateLayout();
        saveCurrCache();
    }
}

void App::waitLayoutGen(Time t)
{
    layoutGen()->wait(t);
    updateLayout();
}