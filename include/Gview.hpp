#ifndef GVIEW_GVIEW_HPP
#define GVIEW_GVIEW_HPP
#include "GraphPlotterFlib.hpp"
#include "FDLayoutCPU.hpp"
#include "ViewLoader.hpp"
#include "ViewParams.hpp"
#include "View.hpp"

struct Gview
{
    typedef ViewLoader::GlobalId NodeId;
    
    Gview(DerefImporter *importer) :
        m_viewLo(importer),
        m_lgen(0.000001),
        m_plotter([&](int id){loadSync(toPublicId(id));})
    {
        
    }
    
    void start(NodeId id, ViewParams params);
    void stop() {m_plotter.stop();};
    void wait() {m_plotter.wait();};
    
private:
    ViewLoader m_viewLo;
    View m_curView;
    FDLayoutCPU m_lgen;
    GraphPlotterFlib m_plotter;
    ViewParams m_vpar;
    
    void loadSync(NodeId id);
    NodeId toPublicId(int viewLocId);
};

#endif // GVIEW_GVIEW_HPP