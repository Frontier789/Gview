#include "LayoutGenerator.hpp"
#include "Utility.hpp"
#include <thread>
#include <chrono>

void LayoutGenerator::run(Graph graph)
{
	stop();
	
	m_stopped = false;
	m_graph = std::move(graph);
	handleNewGraph();
	
	m_fut = std::async(std::launch::async, [this](){
		return this->generate();
    });
}

LayoutDesc LayoutGenerator::layout()
{
	auto desc = std::move(m_fut.get());
	desc.graph = std::move(m_graph);
	
	return desc;
}

bool LayoutGenerator::ready()
{
	return m_fut.valid() && ::ready(m_fut);
}

void LayoutGenerator::wait()
{
	if (m_fut.valid())
		m_fut.wait();
}

void LayoutGenerator::wait(fm::Time timeout)
{
	if (m_fut.valid())
		m_fut.wait_for(std::chrono::duration<double>(timeout.s()));
}

void LayoutGenerator::stop()
{
	m_stopped = true;
	wait();
}


