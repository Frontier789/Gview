#include "GridImporter.hpp"

GridImporter::GridImporter(size_t n) :
    n(n)
{
    
}

View GridImporter::view()
{
    typedef View::Node Node;
    typedef View::Edge Edge;
    View v;
    Cx(n)Cy(n) {
        String label = "{" + fm::toString(x) + "," + fm::toString(y) + "}";
        auto shape = (x+y)%2 ? NodeShape::Circle : NodeShape::Triangle;
        Node node;
        node.visuals = {10,shape};
        node.label = {label,"This is " + label};
        node.body = {vec2(),1};
        node.edges = {},
        node.selectors = {{"focus", 0},{"dummy",1}};
        v.graph.push_back(std::move(node));
    }
    
    Cx(n)Cy(n) {
        size_t a = x*n+y;
        size_t b = (x+1)*n+y;
        size_t c = x*n+(y+1);
        if (x+1 < n)
            v.graph[a].edges.push_back({{4.1,NodeShape::None,NodeShape::None,vec4::Gray},b,1});
        if (y+1 < n)
            v.graph[a].edges.push_back({{4.1,NodeShape::None,NodeShape::Triangle,vec4::Gray},c,1});
    }
    
    return v;
}