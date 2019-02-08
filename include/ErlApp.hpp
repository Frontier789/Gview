#ifndef GVIEW_ERLAPP
#define GVIEW_ERLAPP

#include "App.hpp"
#include "ErlImporter.hpp"

struct ErlApp : public App {
    ErlApp(Delegate<void,string> logFunc);
    
    ErlImporter *importer() override {return (ErlImporter*)App::importer();}
    void prepare() override;
    void update() override;
};

#endif