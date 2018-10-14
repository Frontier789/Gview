#ifndef GVIEW_GRAPH_HPP
#define GVIEW_GRAPH_HPP
#include "VertProps.hpp"
#include <iosfwd>
#include <vector>
using std::vector;

class Graph
{
public:
    Graph(const Graph&) = delete;
    Graph(Graph&&) = default;
    Graph() = default;
    Graph &operator=(Graph &&) = default;
    
    typedef VertProps::NodeId NodeId;
    typedef VertProps::EdgeType OutEdge;
    
    vector<VertProps> vertices;
    
    size_t nodeCount() const {return vertices.size();}
    
    void undirect();
};

std::ostream &operator<<(std::ostream &out,const Graph &g);

#endif // GVIEW_GRAPH_HPP