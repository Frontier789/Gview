#include "ViewLoader.hpp"

ViewLoader::ViewLoader(DerefImporter *importer,DerefGroupper *groupper) : 
    m_importer(importer),
    m_groupper(groupper)
{
    m_groupper->setImporter(m_importer.get());
}

using GlobalId = ViewLoader::GlobalId;

#include "UnionFind.hpp"
#include <iostream>
using namespace std;

struct DfsHelper
{
    GlobalId nodeId;
    ViewParams params;
    
    vector<GlobalId> stck, next_stck;
    View view;
    
    DerefImporter *importer;
    DerefGroupper *groupper;
    UnionFind<GlobalId> uf;
    
    GlobalId nxt() {
        auto id = stck.back();
        stck.pop_back();
        return id;
    }
    
    void explore_group(GlobalId source, GlobalId target) {
        auto source_group = uf.find(source);
        auto target_group = uf.find(target);
        
        if (!view.hasNode(target_group)) {
            vector<GlobalId> nodesin;
            nodesin.push_back(target);
            
            view.addNode(target_group,VertProps{groupper->labelGlob(target_group)});
            next_stck.push_back(target_group);
            
            while (!nodesin.empty()) {
                GlobalId cur = nodesin.back(); nodesin.pop_back();
                for (auto nxt : importer->outEdgesGlob(cur)) {
                    if (groupper->sameGroupGlob(target, nxt) && uf.find(nxt) != target_group) {
                        uf.unite(target, nxt);
                        // next_stck.push_back(nxt);
                        cout << "united " << target << " with " << nxt << endl;
                    }
                }
            }
        }
        
        if (source_group != target_group)
            view.addEdge(view.toLocal(source), view.toLocal(target_group));
    }
    
    void gen() {
        stck.push_back(nodeId);
        view.addNode(nodeId,VertProps{importer->labelGlob(nodeId)});
        
        for (int depth = 0;depth < params.depth && !stck.empty();++depth) {
            while (!stck.empty()) {
                auto id = nxt();
                
                for (auto target : importer->outEdgesGlob(id))
                    explore_group(id, target);
            }
            stck.swap(next_stck);
        }
        
        view.graph.undirect();
    }
};

View ViewLoader::load(GlobalId nodeId, ViewParams params) const
{
    DfsHelper helper;
    helper.nodeId = nodeId;
    helper.params = std::move(params);
    helper.importer = m_importer.get();
    helper.groupper = m_groupper.get();
    
    helper.gen();
    
    return std::move(helper.view);
}
