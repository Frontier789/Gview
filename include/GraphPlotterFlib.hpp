#ifndef GVIEW_GRAPHPLOTTERFLIB_HPP
#define GVIEW_GRAPHPLOTTERFLIB_HPP

#include "GraphPlotter.hpp"

namespace priv
{
	struct StrPlotFlib
	{
		StrPlotFlib();
		
		void layout(GuiLayout *layout);
		void resize(::size_t n);
		void set(::size_t i,String str);
		void set(::size_t i,vec2 pos);
		vec2 size(::size_t i);
	private:
		GuiLayout *m_layout;
		::size_t m_size;
	};
	
	struct MouseHandlerFlib : public GuiElement, public ClickListener
	{
		MouseHandlerFlib(GuiContext &cont,Delegate<void,int> cb,float r);
		
		void adapt(Plot *plot);
		void transf(mat4 m);
	
		void onMouseMove(fm::vec2 p,fm::vec2 prevP) override;
		
		void onDraw(ShaderManager &shader) override;
		bool onEvent(fw::Event &ev) override;
		void onPress(fw::Mouse::Button button,fm::vec2 p) override;
		
	private:
		Plot *m_plot;
		mat4 m_transf;
		float m_radius;
		Delegate<void,int> m_cb;
		DrawData m_dd;
		int m_active;
		
		void mouse(vec2 p);
		void setupDD();
		void clearDD() {m_dd.reset();}
	};
}

struct GraphPlotterFlib : public GraphPlotter
{
	GraphPlotterFlib(Delegate<void,NodeId> click_cb = nullptr);
protected:
	Result displayLoop() override;

private:
	::priv::MouseHandlerFlib *m_mHandler;
	::priv::StrPlotFlib m_strPlot;
	TransformListener m_tr;
	GuiWindow m_win;
	DrawData m_dd;
	Plot m_plot;
	
	Mesh *tomesh();
	void focus(rect2f area);
	
	void setup();
	void clean();
	void guiLoop();
	void checkNewPlot();
	void updateLabelp();
	void updateLabels();
	void handleUpdate();
	
	void addMouseHandler();
	void addDisplayWidget();
	void addStrLayout();
};

#endif // GVIEW_GRAPHPLOTTERFLIB_HPP
