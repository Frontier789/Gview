#ifndef GVIEW_VIEWLOADER_HPP
#define GVIEW_VIEWLOADER_HPP
#include "ViewParams.hpp"
#include "Groupper.hpp"
#include "Logger.hpp"
#include "View.hpp"
#include <iosfwd>

struct ViewLoader
{
    typedef View::NodeId LocalId;
    typedef DerefImporter::PrivateId GlobalId;
    
    View load(GlobalId nodeId, ViewParams params) const;
    
    ViewLoader(const ViewLoader&) = delete;
    ViewLoader(ViewLoader&&) = default;
    ViewLoader(DerefImporter *importer,DerefGroupper *groupper);
    
private:
    std::unique_ptr<DerefImporter> m_importer;
    std::unique_ptr<DerefGroupper> m_groupper;
};

#endif // GVIEW_VIEWLOADER_HPP