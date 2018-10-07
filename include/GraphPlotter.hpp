#ifndef GVIEW_GRAPHPLOTTER_HPP
#define GVIEW_GRAPHPLOTTER_HPP

#include "Utility.hpp"
#include "Plot.hpp"
#include <atomic>
#include <future>

struct GraphPlotter
{
	typedef Graph::NodeId NodeId;
	typedef int Result;
	
	GraphPlotter(Delegate<void,NodeId> click_cb = nullptr);
	~GraphPlotter();
	
	void plot(Plot p);
	void start();
	void stop();
	void wait();
	bool running();
private:
	
	Delegate<void,NodeId> m_onclick;
	std::atomic<Plot*>  m_newplot;
	std::atomic<bool>   m_stopped;
	std::future<Result> m_fut;
	bool m_running;
	
	void newPlot(Plot *p);

protected:
	bool stopped() {return m_stopped;}
	Plot *getplot() {return m_newplot.exchange(nullptr);}
	void callcb(int id) {m_onclick(id);}
	
	virtual Result displayLoop() = 0;
};

#endif // GVIEW_GRAPHPLOTTER_HPP
