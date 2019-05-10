#include "LabelHandler.hpp"
#include <iostream>

LabelHandler::LabelHandler(GuiContext &cont,bool do_unintersect) :
    GuiElement(cont, cont.getSize()),
	m_unintersect(do_unintersect),
	m_activeId(-1)
{
    
}

struct Rct
{
	vec2 p,s;
	size_t i;
};
void unintersect(vector<Rct> &rcts);
const vec3 activeClr = vec3(.5,0,0);


void LabelHandler::applyLayout()
{
	size_t s = m_layout.size();
	
	vector<Rct> rcts(s);
	vector<vec4> clr(s,vec4::Black);
	
	C(s) {
		vec2 p = m_layout.positions[i];
		p = m_transf * vec4(p);
		p.y += (m_transf * vec4(0,1,0,0)).length() * m_radii[i];
		rcts[i] = Rct{p, m_texts[i]->getSize(), i};
		
		rcts[i].p -= rcts[i].s * vec2(.5,.05);
	}
	
	if (m_unintersect) unintersect(rcts);
	else {
		Cx(s) {
			rect2f rct_x(rcts[x].p, rcts[x].s);
			
			if (!rct_x.intersects(rect2f(vec2(),getOwnerContext().getSize()))) continue;
			
			float area  = rct_x.size.area();
			float covmx = 0;
			float rx = m_radii[rcts[x].i];
			Cy(s) if (x != y) {
				float ry = m_radii[rcts[y].i];
				float ay = rcts[y].s.area();
				// if (rx < ry || (rx == ry && (ay > area || ay == area && x < y))) 
				// if (rx <= ry) 
				if (rx < ry || (rx == ry && ay > area)) 
				{
					rect2f rct_y(rcts[y].p, rcts[y].s);
					covmx = max(covmx, rct_x.intersection(rct_y).size.area());
				}
			}
			
			const float lim = .3;
			 
			float a = covmx / area;
			if (a > lim)
				clr[rcts[x].i].a = 0;
			else {
				clr[rcts[x].i].a = 1 - (3*pow(a/lim,2)-2*pow(a/lim,3));
			}
		}
	}
    
	for (auto r : rcts) {
        m_texts[r.i]->setPosition(r.p);
        m_texts[r.i]->setColor(clr[r.i]);
	}
	
	applyActiveId();
}
    
void LabelHandler::clear()
{
    for (auto txt : m_texts)
        delete txt;
	
	m_texts.clear();
	m_radii.clear();
	m_layout.positions.clear();
	m_activeId = size_t(-1);	
}

void LabelHandler::hide()
{
	setTransform(mat4{0,0,0,0, 0,0,0,50000, 0,0,0,0, 0,0,0,1});
}

size_t LabelHandler::probe(vec2 p) const
{
	size_t cand = size_t(-1);
	float  best = 0;
	C(m_texts.size()) {
		auto t = m_texts[i];
		if (rect2f(t->getPosition(), t->getSize()).contains(p)) {
			float a = t->getColor().a;
			if (a > best) {
				best = a;
				cand = i;
				if (best == 1) break;
			}
		}
	}
	
	return cand;
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
	
	m_activeId = size_t(-1);
    
    m_texts.resize(view.size());
    m_radii.resize(view.size());
	
    C(m_texts.size()) {
        auto node = view.graph[i];
		
        m_texts[i]->setText(node.label.text);
        m_texts[i]->setCharacterSize(3.f/4 * (node.visuals.size - 6) + 13);
		m_radii[i] = node.visuals.size;
	}
    
    setLayout(view.getLayout());
}
    
void LabelHandler::setActiveId(size_t id)
{
	if (m_activeId != size_t(-1)) {
		auto t = m_texts[m_activeId];
		auto c = t->getColor();
		c.xyz() = vec4::Black;
		t->setColor(c);
	}
	
	m_activeId = id;
}

void LabelHandler::applyActiveId() {
	if (m_activeId != size_t(-1)) {
		auto t = m_texts[m_activeId];
		auto c = t->getColor();
		c.xyz() = activeClr;
		t->setColor(c);
	}
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
