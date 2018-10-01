#ifndef GVIEW_GRAPHPLOTTERFLIB_HPP
#define GVIEW_GRAPHPLOTTERFLIB_HPP

#include "GraphPlotter.hpp"

struct GraphPlotterFlib : public GraphPlotter
{
protected:
	Result displayLoop() override;

private:
	GuiWindow m_win;
	DrawData m_dd;
	
	Mesh *tomesh(Plot *p);
	void checkUpdate();
	
	void addDisplayWidget();
};

#endif // GVIEW_GRAPHPLOTTERFLIB_HPP
