#ifndef GVIEW_GVIEW_HPP
#define GVIEW_GVIEW_HPP
#include "GraphPlotterFlib.hpp"
#include "LayoutGenerator.hpp"
#include "ViewLoader.hpp"
#include "ViewParams.hpp"
#include "Groupper.hpp"
#include "Importer.hpp"
#include "IdTypes.hpp"
#include "View.hpp"

struct Gview
{
    typedef LocalNodeId NodeId;
    
    Gview(DerefImporter *importer,LayoutGenerator *layoutgen,DerefGroupper *groupper = nullptr);
    
    void start(NodeId id, ViewParams params);
    void stop() {m_plotter.stop();};
    void wait() {m_plotter.wait();};
    
private:
    ViewLoader m_viewLo;
    View m_curView;
    std::unique_ptr<LayoutGenerator> m_lgen;
    GraphPlotterFlib m_plotter;
    ViewParams m_vpar;
    
    void loadSync(NodeId id);
    NodeId toPublicId(int viewLocId);
};

#endif // GVIEW_GVIEW_HPP