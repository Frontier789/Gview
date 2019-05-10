#ifndef GVIEW_SELECTORBUTTONS
#define GVIEW_SELECTORBUTTONS

#include <Frontier.hpp>
using namespace std;

#include "LogLevel.hpp"
#include "ViewTesselator.hpp"
#include "LabelHandler.hpp"

struct SelectorButtons : public GuiElement
{
    using Node = View::Node;
    using Edge = View::Edge;
    
    SelectorButtons(GuiContext &owner,Delegate<void,LogLevel,string> logfunc);
    
    void setButtons(const std::vector<std::string> &labels,size_t nodeId,const Node &node);
    void setTransform(mat4 transf);
    
    void clearButtons();
    
    /////////////////////////////////////////////////////////////
    void onDraw(fg::ShaderManager &shader) override;
    
    bool visible() const {return m_visible;}
    
    void probe(vec2 mp);
    void setHovered(size_t i);
    size_t getHovered() const {return m_hoverId;}
    size_t getNodeId() const {return m_nodeId;}
    
private:
    size_t m_nodeId;
    size_t m_hoverId;
    mat4 m_transf;
    View m_tmpView;
    float m_sizeMinimum;
    bool m_visible;
    LabelHandler m_labels;
    DrawData m_ddOutline,m_ddFill;
    Delegate<void,LogLevel,string> m_logFunc;
    
    void createDD();
};

#endif