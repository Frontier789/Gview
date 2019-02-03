#include "ViewPlotter.hpp"

ViewPlotter::ViewPlotter(GuiContext &owner) : 
    GuiElement(owner, owner.getSize()),
    m_empty(true),
	m_labels(owner,false)
{
    setOffset(getSize()/2);
}
    
void ViewPlotter::setView(View view)
{
    m_view = std::move(view);
	m_labels.setView(m_view);
	
    m_empty = false;
    createDD();
}

void ViewPlotter::setLayout(Layout layout)
{
    if (!m_empty) {
        m_view.setLayout(layout);
		m_labels.setLayout(layout);
        
        createDD();
    }
}

void ViewPlotter::onDraw(fg::ShaderManager &shader)
{
    shader.getModelStack().push(getTransformMatrix());
    shader.draw(m_dd);
    shader.getModelStack().pop();
	
	m_labels.onDraw(shader);
}

void ViewPlotter::onTransform()
{
	m_labels.setTransform(getTransformMatrix());
}

void ViewPlotter::createDD()
{
    Mesh m;
	
	for (auto &node : m_view.graph) {
        float r = node.visuals.size;
        
		int N = 6*r*getZoom();
		for (int n=0;n<N;++n) {
			pol2 r0(r, deg(n*360.0/(N-1)));
			pol2 r1(r, deg((n+1)*360.0/(N-1)));
			m.pts.push_back(vec2(r0) + node.body.pos);
			m.pts.push_back(vec2(r1) + node.body.pos);
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
