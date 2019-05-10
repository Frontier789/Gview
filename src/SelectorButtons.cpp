
#include "SelectorButtons.hpp"
#include <iostream>
using namespace std;

SelectorButtons::SelectorButtons(GuiContext &owner,Delegate<void,LogLevel,string> logfunc) :
    GuiElement(owner,owner.getSize()),
    m_nodeId(size_t(-1)),
    m_hoverId(size_t(-1)),
    m_logFunc(logfunc),
    m_sizeMinimum(0),
    m_labels(owner),
    m_visible(false)
{
    
}

void SelectorButtons::setButtons(const std::vector<std::string> &labels,size_t nodeId,const Node &node)
{
    auto s = labels.size();
    if (s == 0) {
        clearButtons();
        m_nodeId = size_t(-1);
        return;
    }
    
    m_nodeId = nodeId;
    m_tmpView.graph.resize(s);
    
    float angle_range = s>1 ? 90 : 0;
    float angle_beg   = 270 - angle_range/2;
    float radius      = node.visuals.size * .2;
    float distance    = node.visuals.size + radius*3;
    float angle_step  = s>1 ? angle_range / (s-1) : 0;
    
    C(s) {
        auto &button_node = m_tmpView.graph[i];
        
        button_node.label = Node::Label{labels[i]};
        button_node.visuals = Node::Visuals{radius,NodeShape::Circle};
        button_node.body.pos = node.body.pos + pol2(distance, deg(angle_beg + angle_step * i));
    }
    
    m_labels.setView(m_tmpView);
    m_labels.hide();
    
    createDD();
}

void SelectorButtons::clearButtons()
{
    m_labels.clear();
    m_tmpView.graph.clear();
    m_ddFill.reset();
    m_ddOutline.reset();
    m_nodeId = size_t(-1);
    m_hoverId = size_t(-1);
    m_visible = false;
}

void SelectorButtons::createDD() {
    
    float zoom = (m_transf * vec4(1,0,0,0)).length();
    bool nowVisible = m_tmpView.graph[0].visuals.size * zoom > m_sizeMinimum;
    
    if (!nowVisible) {
        if (m_visible) {
            m_labels.hide();
            m_ddFill.reset();
            m_ddOutline.reset();
            m_visible = nowVisible;
        }
        return;
    }
    m_visible = nowVisible;
    
    m_labels.setTransform(m_transf);
    
    ViewTesselator tesselator(m_tmpView, m_logFunc);
	
	tesselator.transf = m_transf; 
	tesselator.wsize  = getSize();
	tesselator.zoom   = zoom;
	tesselator.active = m_hoverId;
	
	tesselator.tess();
	
    m_ddOutline = tesselator.line_mesh;
    m_ddFill    = tesselator.tris_mesh;
}

void SelectorButtons::probe(vec2 mp)
{
    size_t i = m_tmpView.probe(m_transf,mp);
    if (i == size_t(-1)) {
        i = m_labels.probe(mp);;
    }
    
    setHovered(i);
}

void SelectorButtons::setHovered(size_t i) {
    if (i != m_hoverId) {
        m_hoverId = i;
        createDD();
    }
}

void SelectorButtons::setTransform(mat4 transf)
{
    m_transf = transf;
    
    if (m_tmpView.graph.size())
        createDD();
}

/////////////////////////////////////////////////////////////
void SelectorButtons::onDraw(fg::ShaderManager &shader)
{
    shader.getModelStack().push(m_transf);
    shader.draw(m_ddOutline);
    shader.draw(m_ddFill);
    shader.getModelStack().pop();
    
	// if (m_plotLabels)
	// 	m_labels.onDraw(shader);
}
