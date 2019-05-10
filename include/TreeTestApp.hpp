#ifndef GVIEW_TREETESTAPP
#define GVIEW_TREETESTAPP

#include <Frontier.hpp>

#include "App.hpp"
#include "TreeImporter.hpp"

struct TreeTestApp : public App {
    TreeTestApp(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache);
    
    TreeImporter *importer() override {return (TreeImporter*)m_importer;}
    
    fm::Result init() override;
    
    void createImporter() override;
    void createLayoutGen() override;
};

#endif