#include "ErlApp.hpp"

ErlApp::ErlApp(Delegate<void,string> logFunc) :
    App(logFunc)
{
    
}

void ErlApp::prepare() {
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
            plotter->setView(importer()->view());
            Clock clk;
            m_gen->start(plotter->view());
            m_gen->wait();

            logFunc("Generation done 🦆");
            logFunc("\tt = " + fm::toString(clk.s()*1000).str() + "ms");
        }
    }
}
