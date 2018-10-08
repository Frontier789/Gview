#ifndef GVIEW_IMPORTER_HPP
#define GVIEW_IMPORTER_HPP
#include <Frontier.hpp>
#include <vector>
using std::vector;

template<class NodeIdT>
class Importer
{
public:
    Importer(const Importer&) = delete;
    Importer(Importer&&) = default;
    Importer() = default;
    
    typedef NodeIdT NodeId;
    struct Edge {NodeId from, to;};
    
    virtual vector<NodeId> outEdges(NodeId id) const = 0;
    virtual String label(NodeId id) const {return "";}
};

#endif // GVIEW_IMPORTER_HPP