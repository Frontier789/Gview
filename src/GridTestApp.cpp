#include "GridTestApp.hpp"

GridTestApp::GridTestApp(Delegate<void,string> logFunc) :
    App(logFunc)
{
    
}

void GridTestApp::init() {
    m_importer = new GridImporter(5);
    
    loadCurrentView();
    
    layoutGen()->wait();
    
    m_genclk.pause();
    logFunc("Generation done 🦆");
    logFunc("\tt = " + fm::toString(m_genclk.s()*1000).str() + "ms");
    
    updateLayout();
}
