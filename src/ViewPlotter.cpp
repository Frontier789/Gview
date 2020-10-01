#include "ViewPlotter.hpp"
#include "ViewTesselator.hpp"
#include <iostream>

ViewPlotter::ViewPlotter(GuiContext &owner,Delegate<void,LogLevel,string> logfunc) : 
    GuiElement(owner, owner.getSize()),
    m_empty(true),
	m_labels(owner,true),
	m_selBtns(owner,logfunc),
	m_tooltip(new Tooltip(owner,logfunc)),
	m_autoCenter(false),
	m_updated(false),
	m_hoveredId(size_t(-1)),
	m_mouseHover(true),
	m_plotLabels(true),
	m_logFunc(logfunc),
	m_canClick(true)
{
	owner.addElement(m_tooltip);
    setOffset(getSize()/2);
}
    
fm::Result ViewPlotter::loadShaders()
{
	auto res = m_aaShader.loadFromFiles("shaders/deaa_vert.glsl","shaders/deaa_frag.glsl");
	
	m_aaShader.setUniform("u_antia",1.1f);
	
	return res;
}
    
void ViewPlotter::setTransfCb(Delegate<void,bool> cb)
{
	m_transfCb = cb;
}

void ViewPlotter::setClickCb(Delegate<void,SelectionData> cb)
{
	m_clickCb = cb;
}

void ViewPlotter::onClick(fw::Mouse::Button button,fm::vec2 p)
{
	if (button == Mouse::Left && m_canClick) {
		if (m_hoveredId != size_t(-1)) {
			m_clickCb(SelectionData::node(m_hoveredId));
		} else {
			auto selId = m_selBtns.getHovered();
			if (selId != size_t(-1)) {
				auto nodeId = m_selBtns.getNodeId();
				auto selGlobId = m_view.graph[nodeId].selectors[selId].id;
				m_clickCb(SelectionData::selector(nodeId,selGlobId));
			}
		}
	}
	
	m_canClick = false;
	
	return TransformListener::onClick(button,p);
}
    
void ViewPlotter::setView(View view)
{
    m_view = std::move(view);
	m_labels.setView(m_view);
	m_selBtns.clearButtons();
	
	m_autoCenter = true;
    m_empty = m_view.size() == 0;
	m_mouseHover = true;
    m_updated = true;
}
    
void ViewPlotter::center()
{
	center(m_view.aabb(mat4()));
}

void ViewPlotter::center(rect2f view_rct)
{
	if (view_rct.size.area() == 0) return;
	
	float z = (getSize() / view_rct.size).min() * .85;
	
	setOffset(getSize()/2 - (view_rct.pos + view_rct.size/2)*z );
	setZoom(z);
	setRotation(0);
}

void ViewPlotter::setLayout(Layout layout)
{
    if (!m_empty) {
        m_view.setLayout(layout);
		
		auto selNode = m_selBtns.getNodeId();
		if (selNode != size_t(-1)) {
			m_selBtns.setButtons(m_view.selectorLabels(selNode),selNode,m_view.graph[selNode]);
		}
		
		if (m_plotLabels)
			m_labels.setLayout(layout);
		
		if (m_autoCenter)
			center();
        
        m_updated = true;
    }
}
    
void ViewPlotter::onMouseMove(fm::vec2 p,fm::vec2 prevP)
{
	m_mouseHover = true;
	
	if (m_canClick && (p - m_grabp).length() > 5) {
		m_canClick = false;
	}
	
	return TransformListener::onMouseMove(p,prevP);
}

bool ViewPlotter::onEvent(fw::Event &ev)
{
	if (ev.type == Event::KeyPressed) {
		if (ev.key.code == Keyboard::C) {
			m_autoCenter = true;
			center();
		}
        if (ev.key.code == Keyboard::L) {
            enableLabels(!areLabelsEnabled());
        }
	}
	
	if (ev.type == Event::Resized) {
		setSize(ev.size);
		m_selBtns.setSize(ev.size);
		center();
	}
	
	if (ev.type == Event::ButtonPressed) {
		if (ev.mouse.button == Mouse::Left) {
			m_canClick = true;
			m_grabp = vec2(ev.mouse);
		}
	}
	
	return GuiElement::onEvent(ev);
}

void ViewPlotter::onDraw(fg::ShaderManager &shader)
{
    shader.getModelStack().push(getTransformMatrix());
    shader.draw(m_ddOutline);
    shader.getModelStack().pop();
	
	m_aaShader.getCamera() = shader.getCamera();
    m_aaShader.getModelStack().top(getTransformMatrix());
    m_aaShader.draw(m_ddFill);
	
	if (m_plotLabels)
		m_labels.onDraw(shader);
	
	m_selBtns.onDraw(shader);
}

void ViewPlotter::enableLabels(bool enable) {
	m_plotLabels = enable;
	
	if (!enable) {
		m_labels.hide();
	} else {
		m_labels.setLayout(m_view.getLayout());
		m_labels.setTransform(getTransformMatrix());
	}
}

void ViewPlotter::onTransform(bool userAction)
{
	m_transfCb(userAction);
	
	m_mouseHover = true;
	
	m_updated = true;
	if (userAction)
		m_autoCenter = false;
}

void ViewPlotter::findHovered(vec2 mp)
{
	auto transf = getTransformMatrix();
	
	auto prevHover = m_hoveredId;
	m_hoveredId = m_view.probe(transf,mp);
	
	if (m_hoveredId == size_t(-1)) {
		m_hoveredId = m_labels.probe(mp);
	}
	
	if (prevHover != m_hoveredId) {
		m_labels.setActiveId(m_hoveredId);
		m_updated = true;
		
		if (m_hoveredId != size_t(-1)) {
			m_selBtns.setButtons(m_view.selectorLabels(m_hoveredId),m_hoveredId,m_view.graph[m_hoveredId]);
		}
		// else {
		// 	m_selBtns.clearButtons();
		// }
	}
	
	if (m_hoveredId == size_t(-1)) {
		m_selBtns.probe(mp);
		m_tooltip->setText("");
	} else {
		m_tooltip->setText(m_view.graph[m_hoveredId].label.tooltip);
	}
}

void ViewPlotter::onUpdate(const fm::Time &dt)
{
	if (m_mouseHover) {
		m_mouseHover = false;
		findHovered(getLastMousePos());
	}
	
	if (m_updated) {
		m_updated = false;
		createDD();
	
		if (m_plotLabels)
			m_labels.setTransform(getTransformMatrix());
		
		m_selBtns.setTransform(getTransformMatrix());
	}
	
	GuiElement::onUpdate(dt);
}

void ViewPlotter::createDD()
{
	ViewTesselator tesselator(m_view, m_logFunc);
	
	tesselator.transf = getTransformMatrix(); 
	tesselator.wsize  = getSize();
	tesselator.zoom   = getZoom();
	tesselator.active = m_hoveredId;
	
	// m_logFunc(LOG_INFO, "View size in DD: " + fm::toString(m_view.size()).str());
	
	tesselator.tess();
	
    m_ddOutline = tesselator.line_mesh;
    m_ddFill    = tesselator.tris_mesh;
}
