#ifndef GVIEW_VIEW_HPP
#define GVIEW_VIEW_HPP
#include "Graph.hpp"
#include <vector>
#include <map>
using std::vector;
using std::map;

struct View
{
    typedef Graph::NodeId NodeId;
    typedef Graph::OutEdge OutEdge;
    typedef int GlobalNodeId;
    
    View(const View&) = delete;
    View(View&&) = default;
    View() = default;
    View &operator=(View&&) = default;
    
    Graph graph;
    vector<GlobalNodeId> globalIds;
    mutable map<GlobalNodeId,NodeId> globToLocal;
    
    bool   hasNode(GlobalNodeId globalId);
    void   addNode(GlobalNodeId globalId,VertProps props = VertProps());
    void   addEdge(NodeId locA,NodeId locB);
    NodeId toLocal(GlobalNodeId id) const;
    GlobalNodeId toGlobal(NodeId id) const;
};

std::ostream &operator<<(std::ostream &out,const View &v);

#endif // GVIEW_VIEW_HPP