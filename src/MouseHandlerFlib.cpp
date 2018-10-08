#include "GraphPlotterFlib.hpp"
#include "Utility.hpp"
#include "Logger.hpp"

namespace priv
{
	MouseHandlerFlib::MouseHandlerFlib(GuiContext &cont,Delegate<void,int> cb,float r) :
		GuiElement(cont,cont.getSize()),
		m_plot(nullptr),
		m_radius(r),
		m_cb(cb),
		m_active(-1)
	{
		
	}
	
	void MouseHandlerFlib::mouse(vec2 p)
	{
		if (!m_plot) return;
		
		auto &pts = m_plot->positions;
		float R = m_radius*m_radius;
		
		vec2 pt = m_transf.inverse() * vec4(p,0,1);
		
		m_active = -1;
		for (size_t i=0;i < pts.size();++i) {
			if ((pts[i] - pt).LENGTH() < R) {
				m_active = i;
			}
		}
		for (size_t i=0;i < pts.size();++i) {
			if ((pts[i] - pt).LENGTH() < R*4) {
				m_active = i;
			}
		}
		
		if (m_active != -1) setupDD();
		else clearDD();
	}
	
	void MouseHandlerFlib::adapt(Plot *plot)
	{
		m_plot = plot;
		mouse(getLastMousePos());
	}
	
	void MouseHandlerFlib::transf(mat4 m)
	{
		m_transf = m;
		mouse(getLastMousePos());
	}
	
	void MouseHandlerFlib::onMouseMove(fm::vec2 p,fm::vec2)
	{
		mouse(p);
	}
	
	void MouseHandlerFlib::onDraw(ShaderManager &shader)
	{
		if (!m_plot) return;
		
		shader.getModelStack().push(m_transf).mul(MATRIX::translation(m_plot->positions[m_active]));
		shader.draw(m_dd);
		shader.getModelStack().pop();
	}
	
	bool MouseHandlerFlib::onEvent(fw::Event &ev)
	{
		if (ev.type == Event::Resized)
			setSize(ev.size);
		
		return false;
	}
	
	void MouseHandlerFlib::onPress(fw::Mouse::Button button,fm::vec2 p)
	{
		if (m_active != -1)
			m_cb(m_active);
	}
	
	void MouseHandlerFlib::setupDD()
	{
		Mesh m = Mesh::getCircle(m_radius);
		m.clr.resize(m.pts.size(),vec4::Red);
		
		m_dd = m;
	}
}