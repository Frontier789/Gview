#ifndef GVIEW_GROUPPER_HPP
#define GVIEW_GROUPPER_HPP
#include <Frontier.hpp>
#include "Importer.hpp"

struct DerefGroupper
{
    DerefGroupper(const DerefGroupper&) = delete;
    DerefGroupper(DerefGroupper&&) = default;
    DerefGroupper() : m_importer(nullptr) {}
    virtual ~DerefGroupper() = default;
    
    typedef PrivateGlobalNodeId PrivateId;
    
    virtual bool sameGroupGlob(PrivateId a, PrivateId b) const {return a == b;}
    virtual String labelGlob(PrivateId nodeInGroup) const {return getDerefImporter()->labelGlob(nodeInGroup);}
    
    void setImporter(DerefImporter *importer) {m_importer = importer;}
    
protected:
    DerefImporter *getDerefImporter() const {return m_importer;}
    
    DerefImporter *m_importer;
};

typedef DerefGroupper NoGroupper;

template<class NodeIdT>
struct Groupper : public DerefGroupper
{
    Groupper(const Groupper&) = delete;
    Groupper(Groupper&&) = default;
    Groupper() = default;
    virtual ~Groupper() = default;
    
    typedef NodeIdT PublicId;
    
    bool sameGroupGlob(PrivateId a, PrivateId b) const override;
    String labelGlob(PrivateId nodeInGroup) const override;
    
    virtual bool sameGroup(PublicId a, PublicId b) const = 0;
    virtual String label(PublicId nodeInGroup) const {return getImporter()->label(nodeInGroup);}
    
protected:
    Importer<NodeIdT> *getImporter() const {return (Importer<NodeIdT>*)getDerefImporter();}
};

template<class NodeIdT>
inline bool Groupper<NodeIdT>::sameGroupGlob(PrivateId a, PrivateId b) const
{
    if (a == b) return true;
    
    auto imp = getImporter();
    
    return sameGroup(imp->toPub(a), imp->toPub(b));
}

template<class NodeIdT>
inline String Groupper<NodeIdT>::labelGlob(PrivateId nodeInGroup) const
{
    auto imp = getImporter();
    
    return label(imp->toPub(nodeInGroup));
}

#endif // GVIEW_GROUPPER_HPP
