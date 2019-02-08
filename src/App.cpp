#include "App.hpp"
#include "GridImporter.hpp"
#include "FDCpu.hpp"

App::App(Delegate<void,string> logFunc) :
    m_importer(nullptr),
    plotter(nullptr),
    logFunc(logFunc)
{
    
}

App::~App() {
    delete m_importer;
}

void App::prepare() {
    m_importer = new GridImporter(5);
    
    plotter->setView(m_importer->view());
    Clock clk;
    m_gen->start(plotter->view());
    m_gen->wait();

    logFunc("Generation done 🦆");
    logFunc("\tt = " + fm::toString(clk.s()*1000).str() + "ms");
}

void App::update() {
    plotter->setLayout(m_gen->layout());
}

void App::run() {
    win.open(vec2(640,480),"win");
    
    plotter = new ViewPlotter(win);
    win.addElement(plotter);
    m_gen = new FDCpu(RKDesc::fehlberg());
    
    prepare(); // prepare may need a GL context and the plotter
    
    
    win.runGuiLoop(Delegate<void>(App::update,this));
}
