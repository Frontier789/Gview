#ifndef GVIEW_ERLAPP
#define GVIEW_ERLAPP

#include "App.hpp"
#include "ErlImporter.hpp"

struct ErlApp : public App {
    ErlApp(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache);
    
    ErlImporter *importer() override {return (ErlImporter*)App::importer();}
    fm::Result init() override;
    void update() override;
    
    void sendTransf();
};

#endif