#ifndef GVIEW_VERTPROPS_HPP
#define GVIEW_VERTPROPS_HPP
#include <Frontier.hpp>
#include "IdTypes.hpp"
#include <vector>
using std::vector;

struct VertProps
{
    typedef LocalNodeId NodeId;
    struct EdgeType {NodeId target;};
    
    String label;
    vector<EdgeType> outEdges;
};

#endif // GVIEW_VERTPROPS_HPP