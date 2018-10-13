#ifndef GVIEW_IMPORTER_HPP
#define GVIEW_IMPORTER_HPP
#include <Frontier.hpp>
#include <vector>
#include <map>
using std::vector;

class DerefImporter
{
public:
    DerefImporter(const DerefImporter&) = delete;
    DerefImporter(DerefImporter&&) = default;
    DerefImporter() = default;
    
    typedef int PrivateId;
    
    virtual vector<PrivateId> outEdgesGlob(PrivateId id) const  = 0;
    virtual String labelGlob(PrivateId id) const = 0;
};

template<class NodeIdT>
struct Importer : public DerefImporter
{
    Importer(const Importer&) = delete;
    Importer(Importer&&) = default;
    Importer() = default;
    
    typedef NodeIdT NodeId;
    typedef NodeIdT PublicId;
    struct Edge {PublicId from, to;};
    
    vector<PrivateId> outEdgesGlob(PrivateId id) const override;
    String labelGlob(PrivateId id) const override;
    
    virtual vector<PublicId> outEdges(PublicId id)  const = 0;
    virtual String label(PublicId id) const {return "";}
    
    PrivateId toPriv(PublicId pub) const;
    PublicId toPub(PrivateId priv) const;
    
private:
    mutable std::map<PublicId, PrivateId> m_toPriv;
    mutable std::map<PrivateId, PublicId> m_toPub;
};
    
template<class NodeIdT>
inline typename Importer<NodeIdT>::PrivateId Importer<NodeIdT>::toPriv(PublicId pub) const
{
    auto it = m_toPriv.find(pub);
    if (it == m_toPriv.end()) {
        auto s = m_toPriv.size();
        m_toPriv[pub] = s;
        m_toPub[s] = pub;
        return s;
    }
    
    return it->second;
}

template<class NodeIdT>
inline NodeIdT Importer<NodeIdT>::toPub(PrivateId priv) const
{
    auto it = m_toPub.find(priv);
    
    return it->second;
}

template<class NodeIdT>
vector<typename Importer<NodeIdT>::PrivateId> Importer<NodeIdT>::outEdgesGlob(PrivateId id) const 
{
    auto nids = outEdges(toPub(id));
    vector<PrivateId> ret;
    ret.reserve(nids.size());
    
    for (auto id : nids)
        ret.push_back(toPriv(id));
    
    return ret;
}

template<class NodeIdT>
String Importer<NodeIdT>::labelGlob(PrivateId id) const 
{
    return label(toPub(id));
}

#endif // GVIEW_IMPORTER_HPP
