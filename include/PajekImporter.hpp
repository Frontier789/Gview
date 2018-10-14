#ifndef GVIEW_PAJEKIMPORTER_HPP
#define GVIEW_PAJEKIMPORTER_HPP
#include "Importer.hpp"

struct PajekImporter : public Importer<size_t>
{
    PajekImporter(std::string file);
    vector<NodeId> outEdges(NodeId id) const override;
    String label(NodeId id) const override;
private:
    vector<vector<NodeId>> m_graph;
    vector<std::string> m_labels;
};

#endif // GVIEW_PAJEKIMPORTER_HPP
