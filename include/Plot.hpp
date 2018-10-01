#ifndef GVIEW_PLOT_HPP
#define GVIEW_PLOT_HPP

#include <Frontier.hpp>
#include <vector>
#include "Graph.hpp"

using std::vector;

struct Plot
{
	Graph graph;
	vector<vec2> positions;
};

#endif // GVIEW_PLOT_HPP
