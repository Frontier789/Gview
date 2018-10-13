#ifndef GVIEW_VIEW_HPP
#define GVIEW_VIEW_HPP
#include "Importer.hpp"
#include "Graph.hpp"
#include <vector>
#include <map>
using std::vector;
using std::map;

struct View
{
    typedef Graph::NodeId NodeId;
    typedef Graph::OutEdge OutEdge;
    typedef DerefImporter::PrivateId GlobalId;
    
    View(const View&) = delete;
    View(View&&) = default;
    View() = default;
    View &operator=(View&&) = default;
    
    Graph graph;
    vector<GlobalId> globalIds;
    mutable map<GlobalId,NodeId> globToLocal;
    
    bool   hasNode(GlobalId globalId);
    void   addNode(GlobalId globalId,VertProps props = VertProps());
    void   addEdge(NodeId locA,NodeId locB);
    NodeId toLocal(GlobalId id) const;
    GlobalId toGlobal(NodeId id) const;
};

std::ostream &operator<<(std::ostream &out,const View &v);

#endif // GVIEW_VIEW_HPP