#include <fstream>
#include <iostream>
#include <Frontier.hpp>
using namespace std;

#include "FDCpu.hpp"
#include "ViewPlotter.hpp"
#include "Importer.hpp"
#include "GridImporter.hpp"
#include "ErlImporter.hpp"

#ifdef FRONTIER_OS_WINDOWS
	#include <io.h>
	#include <fcntl.h>
#endif

struct App {
    App(Delegate<void,string> logFunc);
    virtual ~App();
    
    GuiWindow win;
    Importer *m_importer;
    ViewPlotter *plotter;
    LayoutGen *m_gen;
    Delegate<void,string> logFunc;
    
    virtual Importer *importer() {return m_importer;}
    virtual void prepare();
    virtual void update();
    void run();
};

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
}

void App::update() {
    plotter->setLayout(m_gen->layout());
}

void App::run() {
    win.open(vec2(640,480),"win");
    
    prepare(); // prepare may need a GL context
    
    plotter = new ViewPlotter(win);
    plotter->setView(m_importer->view());
    
    win.addElement(plotter);
    
    m_gen = new FDCpu(RKDesc::fehlberg());
    m_gen->start(plotter->view());
    m_gen->wait(fm::milliseconds(100));
    
    Clock clk;
    logFunc("Generation done");
    logFunc("\tt = " + fm::toString(clk.s()*1000).str() + "ms");
    
    win.runGuiLoop(Delegate<void>(App::update,this));
    
    // ofstream out_log("out.log");
    // bool fst;
    
    // for (auto &vec : {m_gen->log.h,m_gen->log.e,m_gen->log.s}) {
    //     fst = true;
    //     for (auto f : vec) {
    //         if (!fst) out_log << ","; fst = false;
    //         out_log << f;
    //     } out_log << endl;
    // }
}

struct ErlApp : public App {
    ErlApp(Delegate<void,string> logFunc);
    
    ErlImporter *importer() override {return (ErlImporter*)App::importer();}
    void prepare() override;
    void update() override;
};

ErlApp::ErlApp(Delegate<void,string> logFunc) :
    App(logFunc)
{
    
}

void ErlApp::prepare() {
    m_importer = new ErlImporter(logFunc);
}

void ErlApp::update() {
    App::update();
    
    // if (importer()->m_input) {
    //     if (importer()->closed()) {
    //         logFunc("Erl closed");
    //         win.stopGuiLoop();
            
    //     } else {
    //         logFunc("Erl importer has input!");
            
    //         string s = importer()->recv<string>();
    //         logFunc(" it was " + s);
            
    //         if (s == "center") {
    //             plotter->center();
    //         }
    //     }
    // }
}

struct Gview {
    Gview(int argc,char *argv[]);
    
    void run();
    void create_app();
    
    bool erl_input;
    ofstream log_file;
    unique_ptr<App> app;
};

Gview::Gview(int argc,char *argv[]) : 
    erl_input(false),
    log_file("gview.log"),
    app(nullptr)
{
    for (int i=1;i<argc;++i) {
        string arg = argv[i];
        if (arg == "-i") {
            ++i;
            
            if (i == argc) {
                log_file << "Error: method not specified after -i" << endl;
                exit(1);
            }
            string method = argv[i];
            
            if (method == "erlang") {
                erl_input = true;
            } else {
                log_file << "Error: unknown method \"" << method << "\"" << endl;
                exit(1);
            }
        }
    }
}

void Gview::create_app() {
    if (erl_input) {
        app = unique_ptr<App>(new ErlApp([&](string s){log_file << "[Erl]: " << s << endl;}));
        
    } else {
        app = unique_ptr<App>(new App([&](string s){log_file << "[App]: " << s << endl;}));
    }
}

void Gview::run() {
    create_app();
    app->run();
}


int main(int argc,char *argv[]) {
    
    Gview gview(argc,argv);
    
    gview.run();
}
