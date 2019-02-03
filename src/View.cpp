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
