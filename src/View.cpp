#include "View.hpp"

#include <iostream>

Layout View::getLayout() const
{
    Layout layout;
    layout.positions.resize(size());
    
    C(size())
        layout.positions[i] = graph[i].body.pos;
    
    return layout;
}

void View::setLayout(const Layout &layout)
{
    if (layout.size() != size()) {
        cerr << "Fatal error: setting layout of size: " 
                << layout.size() << " to graph of size: " 
                << size() << endl;
        exit(1);
    }
    
    C(size())
        graph[i].body.pos = layout.positions[i];
}

rect2f View::aabb() const
{
    if (graph.size() == 0) return rect2f(-1,-1,2,2);
    
    if (graph.size() == 1) {
        vec2 s = graph[0].visuals.size;
        
        return rect2f(graph[0].body.pos - s,s*2);
    }
    
    rect2f rct(graph[0].body.pos,vec2());
    
    for (auto &node : graph) {
        vec2 p = node.body.pos;
        vec2 s = node.visuals.size;
        
        rct.expand(p + s);
        rct.expand(p - s);
    }
    
    return rct;
}
