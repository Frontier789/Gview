#include "Graph.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &out,const Graph &g) {
    out << "Node count: " << g.outEdges.size() << "\n";
    for (size_t i=0;i<g.outEdges.size();++i) {
        out << "Node " << i << ".: ";
        for (auto e : g.outEdges[i])
            out << e.target << " ";
        
        if (i+1 != g.outEdges.size()) out << "\n";
    }
    return out;
}

void Graph::undirect()
{
    vector<vector<OutEdge>> newOutEdges = outEdges;
    
    for (size_t id = 0;id<nodeCount();++id) {
        for (OutEdge e : outEdges[id]) {
            newOutEdges[e.target].push_back(OutEdge{int(id)});
        }
    }
    
    outEdges = std::move(newOutEdges);
}
