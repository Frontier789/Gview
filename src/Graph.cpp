#include "Graph.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &out,const Graph &g) {
    out << "Node count: " << g.nodeCount() << "\n";
    for (size_t i=0;i<g.nodeCount();++i) {
        out << "Node " << i << ".: ";
        for (auto e : g.vertices[i].outEdges)
            out << e.target << " ";
        
        if (i+1 != g.nodeCount()) out << "\n";
    }
    return out;
}

void Graph::undirect()
{
    vector<vector<OutEdge>> newOutEdges(nodeCount());
    
    for (size_t id = 0;id<nodeCount();++id) {
        for (OutEdge e : vertices[id].outEdges) {
            newOutEdges[e.target].push_back(OutEdge{NodeId(id)});
        }
    }
    
    for (size_t id = 0;id<nodeCount();++id) {
        for (OutEdge e : newOutEdges[id]) {
            vertices[id].outEdges.push_back(e);
        }
    }
}
