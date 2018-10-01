#ifndef GVIEW_FDLAYOUTCPU
#define GVIEW_FDLAYOUTCPU
#include "LayoutGenerator.hpp"

struct FDLayoutCPU : public LayoutGenerator
{
    FDLayoutCPU(float epsilon = 0.0001) : m_epsilon(epsilon) {}
    
    typedef float LFloat;
    typedef vector2<LFloat> vec2lf;
    typedef rect<LFloat> rect2lf;
    
protected:
	LayoutDesc generate() override;
	void handleNewGraph() override;
    
    void calcBoundingRect();
    vec2lf calcNodeForce(NodeId id);
    vec2lf calcEdgeForce(NodeId id);
    void do_iteration();
    void setInitialPoses();

private:
    vector<vec2lf> m_positions;
    rect2lf m_aabb;
    LFloat m_largestDiff;
    LFloat m_epsilon;
    
    size_t nodeCount() const {return getGraph().nodeCount();}
    vec2lf &point(NodeId id) {return m_positions[id];}
    
    void setLargestDiff(LFloat d) {m_largestDiff = d;}
    void resetLargestDiff() {m_largestDiff = m_epsilon + 1;}
    bool finished() const {return m_largestDiff < m_epsilon;}
    void forEachOutEdge(NodeId id,Delegate<void, OutEdge> f);
};

#endif // GVIEW_FDLAYOUTCPU