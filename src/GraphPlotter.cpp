#include "GraphPlotter.hpp"
#include "Utility.hpp"
#include "Logger.hpp"

void GraphPlotter::plot(Plot p) {
	if (!running())
		start();
	
	newPlot(new Plot(std::move(p)));
}

void GraphPlotter::newPlot(Plot *p)
{
	delete m_newplot.exchange(p);
}

void GraphPlotter::start()
{
	stop();
	
	m_running = true;
	m_stopped = false;
	
	m_fut = std::async(std::launch::async, [this](){
		return displayLoop();
	});
}

void GraphPlotter::stop()
{
	if (running()) {
		m_stopped = true;
		m_fut.wait();
	}
}

void GraphPlotter::wait()
{
	if (running())
		m_fut.wait();
}

bool GraphPlotter::running()
{
	if (!m_running) return false;
	if (ready(m_fut)) {
		m_running = false;
		return false;
	}
	
	return true;
}

GraphPlotter::GraphPlotter() :
	m_newplot(nullptr),
	m_stopped(false),
	m_running(false)
{
	
}

GraphPlotter::~GraphPlotter()
{
	stop();
}