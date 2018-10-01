#ifndef GVIEW_GRAPH_HPP
#define GVIEW_GRAPH_HPP
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
    
    typedef int NodeId;
    struct OutEdge {NodeId target;};
    vector<vector<OutEdge>> outEdges;
    
    size_t nodeCount() const {return outEdges.size();}
    
    void undirect();
};

std::ostream &operator<<(std::ostream &out,const Graph &g);

#endif // GVIEW_GRAPH_HPP