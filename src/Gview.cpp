#include "Gview.hpp"
#include <iostream>
using namespace std;

Gview::Gview(DerefImporter *importer,LayoutGenerator *layoutgen,DerefGroupper *groupper) :
    m_viewLo(importer,groupper ? groupper : new NoGroupper()),
    m_lgen(layoutgen),
    m_plotter([&](int id){loadSync(toPublicId(id));})
{
    
}

void Gview::start(NodeId id, ViewParams params)
{
    m_vpar = params;
    m_plotter.start();
    loadSync(id);
    m_plotter.wait();
}

void Gview::loadSync(NodeId id)
{
    m_curView = std::move(m_viewLo.load(id,m_vpar));
    m_lgen->start(std::move(m_curView.graph));
    m_lgen->wait(seconds(7));
    if (!m_lgen->ready()) {
        cout << "Layout gen timed out" << endl;
        m_lgen->stop();
    }
    
    LayoutDesc desc = m_lgen->layout();
    
    Plot p;
    p.graph = std::move(desc.graph);
    p.positions = std::move(desc.positions);
    p.aabb = desc.aabb;
    
    m_plotter.plot(std::move(p));
}

Gview::NodeId Gview::toPublicId(int viewLocId)
{
    return m_curView.toGlobal(viewLocId);
}
