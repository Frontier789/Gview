#include "LabelHandler.hpp"

LabelHandler::LabelHandler(GuiContext &cont,bool do_unintersect) :
    GuiElement(cont, cont.getSize()),
	m_unintersect(do_unintersect)
{
    
}

struct Rct
{
	vec2 p,s;
	size_t i;
};
void unintersect(vector<Rct> &rcts);

void LabelHandler::applyLayout()
{
	size_t s = m_layout.size();
	
	vector<Rct> rcts(s);
	
	C(s) {
		vec2 p = m_layout.positions[i];
		p = m_transf * vec4(p);
		rcts[i] = Rct{p, m_texts[i]->getSize(), i};
	}
	
	if (m_unintersect) unintersect(rcts);
	
    vec2 zoom_offset = vec2(0,(m_transf * vec4(1,0,0,0)).length()*10);
    
	for (auto r : rcts)
        m_texts[r.i]->setPosition(r.p - r.s * vec2(.5,0) + zoom_offset);
}

namespace
{
	bool smallerY(Rct a,Rct b) {
		if (a.p.y != b.p.y) return a.p.y < b.p.y;
		if (a.p.x != b.p.x) return a.p.x < b.p.x;
		return a.i < b.i;
	}
}

void unintersect(vector<Rct> &rcts)
{
	if (rcts.size() < 2) return;
	
	sort(rcts.begin(),rcts.end(),smallerY);
	Rct *from = &rcts[0];
	int i1=0,i2=0;
	
	for (Rct &cur : rcts)
	{
		while (from->p.y + from->s.h <= cur.p.y) i2++,++from;
		
		rect2f r1(cur.p,cur.s);
		for (auto c = from;c != &cur;++c) {
			rect2f r2(c->p,c->s);
			if (r1.intersects(r2)) {
				cur.p.y = c->p.y + c->s.h;
				r1.pos = cur.p;
			}
		}
		i1++;
	}
}

void LabelHandler::setView(const View &view)
{
    while (view.size() > m_texts.size())
        m_texts.push_back(new GuiText(getOwnerContext()));
    
    for (size_t i=view.size();i<m_texts.size();++i)
        delete m_texts[i];
    
    m_texts.resize(view.size());
    
    C(m_texts.size()) {
        auto label = view.graph[i].visuals.label;
        m_texts[i]->setText(label.text);
    }
    
    setLayout(view.getLayout());
}

void LabelHandler::setLayout(Layout layout)
{
    m_layout = std::move(layout);
    
    applyLayout();
}

void LabelHandler::setTransform(mat4 transf)
{
    m_transf = transf;
    applyLayout();
}
