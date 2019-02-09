#include "ErlApp.hpp"

ErlApp::ErlApp(Delegate<void,string> logFunc) :
    App(logFunc)
{
    m_cache.setOutFile("test.cache");
    m_cache.loadFromFile("test.cache");
}

void ErlApp::init() {
    m_importer = new ErlImporter(logFunc);
}

void ErlApp::update() {
    App::update();
    
    if (importer()->m_input) {
        string s = importer()->recv<string>();
        logFunc("Received command: " + s);
        logFunc("Closed returns: " + string() + (importer()->closed() ? "true" : "false")); 
        
        if (s == "center") {
            plotter->center();
        }
        if (s == "stop") {
            win.stopGuiLoop();
        }
        if (s == "set_view") {
            loadCurrentView("test_view_id");
        }
    }
}
