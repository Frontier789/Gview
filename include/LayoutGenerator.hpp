#ifndef GVIEW_LAYOUTGENERATOR_HPP
#define GVIEW_LAYOUTGENERATOR_HPP
#include "Graph.hpp"
#include <Frontier.hpp>
#include <future>
#include <atomic>

struct LayoutDesc
{
	LayoutDesc(const LayoutDesc&) = delete;
	LayoutDesc(LayoutDesc&&) = default;
	LayoutDesc() = default;
	
	std::vector<vec2> positions;
	Graph graph;
	rect2f aabb;
};

struct LayoutGenerator
{
	void start(Graph graph);
	LayoutDesc layout();
	bool ready();
	void wait(fm::Time timeout);
	void wait();
	void stop();
	
	typedef Graph::NodeId NodeId;
	typedef Graph::OutEdge OutEdge; 
	
private:
	std::atomic<bool> m_stopped;
	std::future<LayoutDesc> m_fut;
	Graph m_graph;

protected:
	const Graph &getGraph() const {return m_graph;}
	bool stopped() const {return m_stopped;}
	
	virtual LayoutDesc generate() = 0;
	virtual void handleNewGraph() = 0;
};

#endif
