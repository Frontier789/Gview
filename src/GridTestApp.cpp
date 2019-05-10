#include "GridTestApp.hpp"
#include "LayeredLayout.hpp"
#include "TreeImporter.hpp"
#include "FDGpuMat.hpp"
#include <iostream>
using namespace std;

GridTestApp::GridTestApp(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache) :
    App(logFunc,enable_gpu,enable_cache)
{
    m_gentimeout = 0;
    m_cache.setFile("cache/grid_test.cache");
}

void GridTestApp::writeHelp()
{
    cout << "Welcome to Grid Test app! Usage:" << endl
         << "\tc - center" << endl
         << "\tl - toggle labels" << endl
         << "\t+- - change grid size" << endl;
}

void GridTestApp::createImporter() {
    m_importer = new GridImporter(3);
}

void GridTestApp::createLayoutGen() {
    // App::m_gen = new LayeredLayout(logFunc);
    App::createLayoutGen();
}

fm::Result GridTestApp::init() {
    Widget *widget = new Widget(win);
    win.addElement(widget);
    
    widget->onkeypress = [&](Keyboard::Key key){
        if (key == Keyboard::R) {
            m_cache.clear();
            reloadCurrentView();
        }
        if (key == Keyboard::Plus || key == Keyboard::Minus) {
            int delta = key == Keyboard::Plus ? 1 : -1;
            importer()->setN(importer()->getN() + delta);
            
            loadCurrentView();
        }
        if (key == Keyboard::L) {
            plotter->enableLabels(!plotter->areLabelsEnabled());
        }
        if (key == Keyboard::K) {
            delete m_gen;
            m_gen = new LayeredLayout(logFunc);
            m_gen->startWorkerThread();
            reloadCurrentView();
        }
        if (key == Keyboard::F) {
            delete m_gen;
            m_gen = new FDGpuMat(RKDesc::fehlberg(),false,logFunc);
            m_gen->startWorkerThread();
            reloadCurrentView();
        }
    };
    
    plotter->enableLabelOverlap(true);
    
    loadCurrentView();
    
    // layoutGen()->wait();
    
    // m_genclk.pause();
    // logFunc("Generation done 🦆");
    // logFunc("\tt = " + fm::toString(m_genclk.s()*1000).str() + "ms");
    
    // updateLayout();
    
    return {};
}
