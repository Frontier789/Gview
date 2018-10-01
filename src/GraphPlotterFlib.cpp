#include "GraphPlotterFlib.hpp"
#include <FRONTIER/OpenGL.hpp>
#include "Logger.hpp"
#include <iostream>
using namespace std;

Mesh *GraphPlotterFlib::tomesh(Plot *p) {
	Mesh *m = new Mesh();
	
	m->pts.assign(p->positions.begin(), p->positions.end());
	m->clr.resize(m->pts.size(),vec4::Red);
	
	return m;
}

GraphPlotterFlib::Result GraphPlotterFlib::displayLoop()
{
	m_win = GuiWindow(vec2(640,480),"win");
	m_win.setClearColor(vec4(.2,1));
	
	addDisplayWidget();
	
	m_win.runGuiLoop([&]() {
		if (stopped()) {
			m_win.stopGuiLoop();
		}
		checkUpdate();
	});
	
	m_dd.reset();
	m_win.close();
	
	return 0;
}

void GraphPlotterFlib::checkUpdate()
{
	Plot *plot = getplot();
	if (!plot) return;
	
	log_info("GraphPlotterFlib", "Updated plot");
	
	Mesh *mesh = tomesh(plot);
	delete plot;
	
	m_dd = *mesh;
	delete mesh;
	m_dd.clearDraws();
	m_dd.addDraw(fg::Points);
}

void GraphPlotterFlib::addDisplayWidget()
{
	Widget *w = new Widget(m_win,m_win.getSize());
	w->ondraw = [&](ShaderManager &shader){
		shader.draw(m_dd);
	};
	
	m_win.getMainLayout().addChildElement(w);
}