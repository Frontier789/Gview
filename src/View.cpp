#include "View.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &out,const View &v) {
    out << v.graph << "\n";
    for (size_t i=0;i<v.graph.nodeCount();++i) {
        out << i << " --> " << v.globalIds[i] << "\n";
    }
    return out;
}

bool View::hasNode(GlobalNodeId globalId) {
    return globToLocal.find(globalId) != globToLocal.end();
}

void View::addNode(GlobalNodeId globalId,VertProps props) {
    graph.vertices.push_back(props);
    globalIds.push_back(globalId);
    globToLocal[globalId] = globalIds.size()-1;
}

void View::addEdge(NodeId locA,NodeId locB) {
    graph.vertices[locA].outEdges.push_back(OutEdge{locB});
}

View::NodeId View::getLocal(GlobalNodeId id) const {
    return globToLocal[id];
}
