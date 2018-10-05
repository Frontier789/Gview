#include "FDLayoutCPU.hpp"
#include "Logger.hpp"

LayoutDesc FDLayoutCPU::generate()
{
    Clock itclk;
    size_t its = 0;
    for (;!finished() && !stopped();++its) {
        do_iteration();
    }
    
    log_info("FDLayout","layout generation took its: " + 
                        toString(its).str() + " and time: " + 
                        toString(itclk.getSeconds()*1000) + "ms");
    
    calcBoundingRect();
    LayoutDesc desc = LayoutDesc{{}, Graph(), m_aabb};
    desc.positions.assign(m_positions.begin(), m_positions.end());
    return desc;
}

void FDLayoutCPU::handleNewGraph()
{
    m_positions = vector<vec2lf>(nodeCount());
    
    setInitialPoses();
    resetLargestDiff();
}

void FDLayoutCPU::calcBoundingRect()
{
    if (nodeCount() == 0) {
        m_aabb = rect2lf(vec2(-1),vec2(2));
        return;
    }
    
    vec2lf mn = point(0);
    vec2lf mx = point(0);
		
    for (size_t id = 1;id < nodeCount();++id)
    {
        vec2lf p = point(id);
        
        mn.x = std::min(mn.x,p.x);
        mn.y = std::min(mn.y,p.y);
        mx.x = std::max(mx.x,p.x);
        mx.y = std::max(mx.y,p.y);
    }
    
    vec2lf p = mn;
    vec2lf cover = mx-mn;
    
    if (cover.w < m_epsilon) {
        p.x = p.x + cover.w/2 - m_epsilon/2;
        cover.w = m_epsilon;
    }
    if (cover.h < m_epsilon) {
        p.y = p.y + cover.h/2 - m_epsilon/2;
        cover.h = m_epsilon;
    }
    
    m_aabb = rect2lf(p,cover);
}

using vec2lf = FDLayoutCPU::vec2lf;
using LFloat = FDLayoutCPU::LFloat;

vec2lf coulomb_force(vec2lf from,vec2lf to,LFloat mass1,LFloat mass2) {
    vec2lf delta = to - from;
    vec2lf dir   = -delta.sgn();
    LFloat LEN   = delta.LENGTH();
    LFloat coef  = mass1 * mass2  * 2000;
    
    return dir / LEN * coef;
};

vec2lf hooke_force(vec2lf from,vec2lf to) {
    vec2lf delta = to - from;
    vec2lf dir   = delta.sgn();
    LFloat LEN   = delta.LENGTH();
    
    return dir * std::log(LEN) * 4;
};

vec2lf FDLayoutCPU::calcNodeForce(NodeId id)
{
    auto pos = point(id);
    vec2lf force;

    for (size_t id2 = 0;id2 < nodeCount();++id2)
    {
        auto pos2 = point(id2);
        
        if (id != id2)
            force += coulomb_force(pos,pos2,1,1);
    }
    
    return force;
}

void FDLayoutCPU::forEachOutEdge(NodeId id,Delegate<void, OutEdge> f)
{
    for (auto e : getGraph().vertices[id].outEdges)
        f(e);
}

vec2lf FDLayoutCPU::calcEdgeForce(NodeId id)
{
    vec2lf from = point(id);
    vec2lf force;
    
    forEachOutEdge(id,[&](OutEdge edge) {
        
        vec2lf to = point(edge.target);
        force += hooke_force(from,to);
        
    });
    
    return force;
}

void FDLayoutCPU::do_iteration()
{
    if (finished()) return;
    
    LFloat stepSize = 0.04;


    std::vector<vec2lf> newPoses(nodeCount());
    LFloat diffmx = 0;
    
    for (size_t id = 0;id < nodeCount();++id)
    {
        vec2lf force = calcNodeForce(id) + calcEdgeForce(id);
        
        newPoses[id] = point(id) + force * stepSize;
        
        diffmx = std::max(diffmx, force.length() * stepSize);
    }
    m_positions = std::move(newPoses);
    
    setLargestDiff(diffmx);
}

void FDLayoutCPU::setInitialPoses()
{
    resetLargestDiff();
    
    size_t n = nodeCount();
    fm::Angle<LFloat> base_angle = fm::deg(360.0 / n);
    
    srand(42);
    
    for (size_t id = 0;id < n;++id) {
        float x = (double)rand() / RAND_MAX;
        float y = (double)rand() / RAND_MAX;
        
        point(id) = (vec2(x,y)*2-vec2(1))*n;
    }
}
