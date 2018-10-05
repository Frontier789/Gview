#ifndef GVIEW_VERTPROPS_HPP
#define GVIEW_VERTPROPS_HPP
#include <Frontier.hpp>
#include <vector>
using std::vector;

struct VertProps
{
    typedef int IdType;
    struct EdgeType {IdType target;};
    
    String label;
    vector<EdgeType> outEdges;
};

#endif // GVIEW_VERTPROPS_HPP