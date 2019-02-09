#include "ViewPlotter.hpp"

ViewPlotter::ViewPlotter(GuiContext &owner) : 
    GuiElement(owner, owner.getSize()),
    m_empty(true),
	m_labels(owner,false),
	m_autoCenter(false),
	m_updated(false)
{
    setOffset(getSize()/2);
}
    
void ViewPlotter::setView(View view)
{
    m_view = std::move(view);
	m_labels.setView(m_view);
	
	m_autoCenter = true;
    m_empty = false;
    m_updated = true;
}
    
void ViewPlotter::center()
{
	center(m_view.aabb());
}

void ViewPlotter::center(rect2f area)
{
	float z = (getSize() / area.size).min() * .85;
	
	setOffset(getSize()/2 - (area.pos + area.size/2)*z );
	setZoom(z);
	setRotation(0);
}

void ViewPlotter::setLayout(Layout layout)
{
    if (!m_empty) {
        m_view.setLayout(layout);
		m_labels.setLayout(layout);
		
		if (m_autoCenter)
			center();
        
        m_updated = true;
    }
}

bool ViewPlotter::onEvent(fw::Event &ev)
{
	if (ev.type == Event::KeyPressed) {
		if (ev.key.code == Keyboard::C) {
			m_autoCenter = true;
		}
	}
	
	return GuiElement::onEvent(ev);
}

void ViewPlotter::onDraw(fg::ShaderManager &shader)
{
    shader.getModelStack().push(getTransformMatrix());
    shader.draw(m_dd);
    shader.getModelStack().pop();
	
	m_labels.onDraw(shader);
}

void ViewPlotter::onTransform(bool userAction)
{
	m_labels.setTransform(getTransformMatrix());
	
	m_updated = true;
	
	if (userAction)
		m_autoCenter = false;
}

void ViewPlotter::onUpdate(const fm::Time &dt)
{
	if (m_updated) {
		m_updated = false;
		createDD();
	}
	
	GuiElement::onUpdate(dt);
}

void ViewPlotter::createDD()
{
    Mesh m;
	mat4 t = getTransformMatrix();
	vec2 s = getSize();
	
	for (auto &node : m_view.graph) {
        float r = node.visuals.size;
		float rt = r * getZoom();
        vec2 p = node.body.pos;
		vec2 pt = t * vec4(p,0,1);
		
		if (rect2f(pt-vec2(rt),2*vec2(rt)).intersects(rect2f(vec2(),s))) {
			int N = min<int>(6*rt, 20);
			for (int n=0;n<N;++n) {
				pol2 r0(r, deg(n*360.0/(N-1)));
				pol2 r1(r, deg((n+1)*360.0/(N-1)));
				m.pts.push_back(vec2(r0) + p);
				m.pts.push_back(vec2(r1) + p);
			}
		}
	}
	
	for (auto &node_a : m_view.graph) {
        float radius_a = node_a.visuals.size;
        
		for (auto &edge : node_a.edges) {
            auto &node_b = m_view.graph[edge.to];
            float radius_b = node_b.visuals.size;
            
			vec2 A = node_a.body.pos;
			vec2 B = node_b.body.pos;
            
			float l = (A-B).length();
			if (l > radius_a + radius_b) {
				vec2 v = (B-A).sgn();
				A += v * radius_a;
				B -= v * radius_b;
				m.pts.push_back(A);
				m.pts.push_back(B);
			}
		}
	}
    
	m.clr.resize(m.pts.size(),vec4::Black);
    m.faces.push_back(Mesh::Face(fg::Lines));
    
    m_dd = m;
}
