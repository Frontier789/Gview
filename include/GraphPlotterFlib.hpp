#ifndef GVIEW_GRAPHPLOTTERFLIB_HPP
#define GVIEW_GRAPHPLOTTERFLIB_HPP

#include "GraphPlotter.hpp"

namespace priv
{
	struct StrPlotFlib
	{
		StrPlotFlib();
		
		void layout(GuiLayout *layout);
		void resize(size_t n);
		void set(size_t i,String str);
		void set(size_t i,vec2 pos);
		vec2 size(size_t i);
	private:
		GuiLayout *m_layout;
		size_t m_size;
	};
}

struct GraphPlotterFlib : public GraphPlotter
{
protected:
	Result displayLoop() override;

private:
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
	
	void addDisplayWidget();
	void addStrLayout();
};

#endif // GVIEW_GRAPHPLOTTERFLIB_HPP
