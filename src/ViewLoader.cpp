#include "ViewLoader.hpp"

ViewLoader::ViewLoader(DerefImporter *importer) : 
    m_importer(importer)
{
    
}

View ViewLoader::load(GlobalId nodeId, ViewParams params) const
{
    vector<GlobalId> stck;
    View view;
    
    stck.push_back(nodeId);
    view.addNode(nodeId,VertProps{m_importer->labelGlob(nodeId)});
    
    for (int depth = 0;depth < params.depth;++depth) {
        vector<GlobalId> next_stck;
        
        while (!stck.empty()) {
            auto id = stck.back(); stck.pop_back();
            auto local_id = view.toLocal(id);
            
            for (auto target : m_importer->outEdgesGlob(id)) {
                if (!view.hasNode(target)) {
                    view.addNode(target,VertProps{m_importer->labelGlob(target)});
                    next_stck.push_back(target);
                }
                
                view.addEdge(local_id, view.toLocal(target));
            }
        }
        stck.swap(next_stck);
    }
    
    view.graph.undirect();
    
    return view;
}
