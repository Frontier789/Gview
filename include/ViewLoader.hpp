#ifndef GVIEW_VIEWLOADER_HPP
#define GVIEW_VIEWLOADER_HPP
#include "ViewParams.hpp"
#include "View.hpp"
#include <iosfwd>

template<class T>
struct ViewLoader
{
    typedef int PrivateNodeId; 
    typedef typename T::NodeId PublicNodeId;
    typedef typename T::Edge   PublicEdge;
    
    template<class... Params>
    ViewLoader(Params&&... params) : m_importer(std::forward<Params>(params)...) {}
    
    View load(PublicNodeId nodeId, ViewParams params) const;
    
    ViewLoader(const ViewLoader&) = delete;
    ViewLoader(ViewLoader&&) = default;
    ViewLoader() = default;
    
private:
    T m_importer;
    mutable map<PublicNodeId,PrivateNodeId> m_pubToPriv;
    mutable map<PrivateNodeId,PublicNodeId> m_privToPub;
    
    PrivateNodeId pubToPriv(PublicNodeId pub) const;
};

template<class T>
inline typename ViewLoader<T>::PrivateNodeId ViewLoader<T>::pubToPriv(PublicNodeId pub) const
{
    auto it = m_pubToPriv.find(pub);
    
    if (it == m_pubToPriv.end()) {
        auto s = m_pubToPriv.size();
        m_pubToPriv[pub] = s;
        m_privToPub[s] = pub;
        return s;
    }
    
    return it->second;
}
    
template<class T>
inline View ViewLoader<T>::load(PublicNodeId nodeId, ViewParams params) const
{
    vector<PublicNodeId> stck;
    View view;
    
    stck.push_back(nodeId);
    view.addNode(pubToPriv(nodeId),VertProps{m_importer.label(nodeId)});
    
    for (int depth = 0;depth < params.depth;++depth) {
        vector<PublicNodeId> next_stck;
        
        while (!stck.empty()) {
            auto public_id  = stck.back(); stck.pop_back();
            auto private_id = pubToPriv(public_id);
            auto local_id   = view.getLocal(private_id);
            
            for (auto target : m_importer.outEdges(public_id)) {
                auto priv_target = pubToPriv(target);
                if (!view.hasNode(priv_target)) {
                    view.addNode(priv_target,VertProps{m_importer.label(target)});
                    next_stck.push_back(target);
                }
                
                view.addEdge(local_id, view.getLocal(priv_target));
            }
        }
        stck.swap(next_stck);
    }
    
    view.graph.undirect();
    
    return view;
}

#endif // GVIEW_VIEWLOADER_HPP