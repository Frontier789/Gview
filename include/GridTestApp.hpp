#ifndef GVIEW_GRIDTESTAPP
#define GVIEW_GRIDTESTAPP

#include <Frontier.hpp>

#include "App.hpp"
#include "GridImporter.hpp"

struct GridTestApp : public App {
    GridTestApp(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache);
    
    GridImporter *importer() override {return (GridImporter*)m_importer;}
    
    fm::Result init() override;

private:
    void writeHelp();
    void createImporter() override;
    
    void createLayoutGen() override;
};

#endif