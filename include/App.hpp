#ifndef GVIEW_APP
#define GVIEW_APP

#include <Frontier.hpp>

#include "ViewPlotter.hpp"
#include "LayoutGen.hpp"
#include "Importer.hpp"
#include "RKDesc.hpp"
#include "Random.hpp"

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

#endif