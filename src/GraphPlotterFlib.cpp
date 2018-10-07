#include "GraphPlotterFlib.hpp"
#include <FRONTIER/OpenGL.hpp>
#include "Logger.hpp"
#include <iostream>
using namespace std;

GraphPlotterFlib::GraphPlotterFlib(Delegate<void,NodeId> click_cb) :
	m_mHandler(nullptr),
	GraphPlotter(click_cb)
{
	
}
	
Mesh *GraphPlotterFlib::tomesh() {
	Mesh *m = new Mesh();
	
	int N = 15;
	float r = 1;
	for (size_t i=0;i<m_plot.positions.size();++i) {
		for (int n=0;n<N;++n) {
			pol2 r0(r, deg(n*360.0/(N-1)));
			pol2 r1(r, deg((n+1)*360.0/(N-1)));
			m->pts.push_back(vec2(r0) + m_plot.positions[i]);
			m->pts.push_back(vec2(r1) + m_plot.positions[i]);
		}
	}
	
	for (size_t i=0;i<m_plot.positions.size();++i) {
		for (auto e : m_plot.graph.vertices[i].outEdges) {
			size_t j = e.target;
			vec2 A = m_plot.positions[i];
			vec2 B = m_plot.positions[j];
			float l = (A-B).length();
			if (l > 2*r) {
				vec2 v = (B-A).sgn();
				A += v * r;
				B -= v * r;
				m->pts.push_back(A);
				m->pts.push_back(B);
			}
		}
	}
	
	m->clr.resize(m->pts.size(),vec4::Black);
	
	return m;
}

void GraphPlotterFlib::setup()
{
	m_tr.setCallback([&](mat4 m){
		updateLabelp();
		if (m_mHandler)
			m_mHandler->transf(m);
	});
	
	m_win = GuiWindow(vec2(640,480),"win");
	m_win.setClearColor(vec4(.92,1));
	m_tr.setOffset(m_win.getSize()/2);
	
	addStrLayout();
	addMouseHandler();
	addDisplayWidget();
}

void GraphPlotterFlib::handleUpdate()
{
	log_info("GraphPlotterFlib", "Updated plot");
	
	updateLabels();
	
	m_mHandler->adapt(&m_plot);
	
	focus(m_plot.aabb);
	
	Mesh *mesh = tomesh();
	m_dd = *mesh;
	
	delete mesh;
	
	m_dd.clearDraws();
	m_dd.addDraw(fg::Lines);
}

void GraphPlotterFlib::addDisplayWidget()
{
	Widget *w = new Widget(m_win,m_win.getSize());
	w->ondraw = [&](ShaderManager &shader){
		shader.setBlendMode(fg::Alpha);
		shader.getModelStack().push(m_tr.getTransformMatrix());
		shader.draw(m_dd);
		shader.getModelStack().pop();
	};
	w->onevent = [&](Event ev){
		m_tr.forwardToHandlers(ev);
	};
	w->onresize = [&]() {
		focus(m_plot.aabb);
	};
	
	m_win.getMainLayout().addChildElement(w);
}

void GraphPlotterFlib::addStrLayout()
{
	GuiLayout *layout = new GuiLayout(m_win);
	m_win.getMainLayout().addChildElement(layout);
	
	m_strPlot.layout(layout);
}

void GraphPlotterFlib::addMouseHandler()
{
	m_mHandler = new ::priv::MouseHandlerFlib(m_win, [&](int i){callcb(i);}, 1);
	m_win.getMainLayout().addChildElement(m_mHandler);
}

void GraphPlotterFlib::updateLabels()
{
	size_t s = m_plot.positions.size();
	
	m_strPlot.resize(s);
	
	for (size_t i=0;i<s;++i)
		m_strPlot.set(i, m_plot.graph.vertices[i].label);
}

struct Rct
{
	vec2 p,s;
	size_t i;
};
void unintersect(vector<Rct> &rcts);

void GraphPlotterFlib::updateLabelp()
{
	size_t s = m_plot.positions.size();
	
	vector<Rct> rcts(s);
	
	for (size_t i=0;i<s;++i) {
		vec2 p = m_plot.positions[i];
		p = m_tr.getTransformMatrix() * vec4(p,0,1);
		rcts[i] = Rct{p + vec2(0,m_tr.getZoom()), m_strPlot.size(i), i};
	}
	
	unintersect(rcts);
	
	for (auto r : rcts)
		m_strPlot.set(r.i, r.p);
}

/////////////////////////////////////////////////////////

void GraphPlotterFlib::focus(rect2f area)
{
	if (!area.size.area()) return;
	
	vec2 s = m_win.getSize();
	float z = (s / area.size).min() * .9;
	vec2  p = s/2 - area.mid()*z;
	
	m_tr.setZoom(z);
	m_tr.setOffset(p);
}

void GraphPlotterFlib::clean()
{
	m_dd.reset();
	m_win.close();
}

void GraphPlotterFlib::guiLoop()
{
	m_win.runGuiLoop([&]() {
		if (stopped()) {
			m_win.stopGuiLoop();
		}
		checkNewPlot();
	});
}


GraphPlotterFlib::Result GraphPlotterFlib::displayLoop()
{
	setup();
	guiLoop();
	clean();
	log_info("plotter","plot finished");
	
	return 0;
}

void GraphPlotterFlib::checkNewPlot()
{
	Plot *pl = getplot();
	if (pl) {
		m_plot = std::move(*pl);
		handleUpdate();
	}
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
	if (!rcts.size()) return;
	
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

