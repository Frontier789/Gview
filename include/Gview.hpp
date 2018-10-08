#ifndef GVIEW_GVIEW_HPP
#define GVIEW_GVIEW_HPP
#include "GraphPlotterFlib.hpp"
#include "FDLayoutCPU.hpp"
#include "ViewLoader.hpp"
#include "ViewParams.hpp"
#include "View.hpp"

template<class T>
struct Gview
{
    typedef typename T::NodeId NodeId;
    
    template<class... Params>
    Gview(Params&&... params) :
        m_viewLo(std::forward<Params>(params)...),
        m_lgen(0.000001),
        m_plotter([&](int id){loadSync(toPublicId(id));})
    {
        
    }
    
    void start(NodeId id, ViewParams params);
    void stop() {m_plotter.stop();};
    void wait() {m_plotter.wait();};
private:
    ViewLoader<T> m_viewLo;
    View m_curView;
    FDLayoutCPU m_lgen;
    GraphPlotterFlib m_plotter;
    ViewParams m_vpar;
    
    void loadSync(NodeId id);
    NodeId toPublicId(int viewLocId);
};

template<class T>
inline void Gview<T>::start(NodeId id, ViewParams params)
{
    m_vpar = params;
    m_plotter.start();
    loadSync(id);
    m_plotter.wait();
}

template<class T>
inline void Gview<T>::loadSync(NodeId id)
{
    m_curView = std::move(m_viewLo.load(id,m_vpar));
    m_lgen.start(std::move(m_curView.graph));
    m_lgen.wait();
    
    LayoutDesc desc = m_lgen.layout();
    
    Plot p;
    p.graph = std::move(desc.graph);
    p.positions = std::move(desc.positions);
    p.aabb = desc.aabb;
    
    m_plotter.plot(std::move(p));
}

template<class T>
inline typename Gview<T>::NodeId Gview<T>::toPublicId(int viewLocId)
{
    auto glob = m_curView.toGlobal(viewLocId);
    auto publ = m_viewLo.privToPub(glob);
    return publ;
}

#endif // GVIEW_GVIEW_HPP