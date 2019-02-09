#ifndef GVIEW_GRIDTESTAPP
#define GVIEW_GRIDTESTAPP

#include <Frontier.hpp>

#include "App.hpp"
#include "GridImporter.hpp"

struct GridTestApp : public App {
    GridTestApp(Delegate<void,string> logFunc);
    
    GridImporter *importer() override {return (GridImporter*)m_importer;}
    
    void init() override;
};

#endif