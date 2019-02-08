#include "GridImporter.hpp"
#include "Random.hpp"

GridImporter::GridImporter(size_t n) :
    n(n)
{
    
}

Random random(42);

View GridImporter::view()
{
    typedef View::Node Node;
    typedef View::Edge Edge;
    View v;
    Cx(n)Cy(n) {
        String label = "{" + fm::toString(x) + "," + fm::toString(y) + "}";
        v.graph.push_back({{10,NodeShape::Circle,{label}}, {vec2(),1}, {}});
    }
    
    Cx(n)Cy(n) {
        size_t a = x*n+y;
        size_t b = (x+1)*n+y;
        size_t c = x*n+(y+1);
        if (x+1 < n)
            v.graph[a].edges.push_back({{1},b});
        if (y+1 < n)
            v.graph[a].edges.push_back({{1},c});
    }
    
    return v;
}