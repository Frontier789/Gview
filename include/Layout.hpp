#ifndef GVIEW_LAYOUT
#define GVIEW_LAYOUT

#include <Frontier.hpp>
using namespace std;

struct Layout
{
    vector<vec2> positions;
    
    size_t size() const {return positions.size();}
};

#endif