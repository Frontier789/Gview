#include "TreeImporter.hpp"
#include <iostream>
using std::cout;
using std::endl;

TreeImporter::TreeImporter(size_t n,size_t k) :
    n(n),
    k(k)
{
    
}

View TreeImporter::view()
{
    typedef View::Node Node;
    typedef View::Edge Edge;
    
    View v;
    
    vector<size_t> cur_layer{0};
    vector<size_t> nxt_layer;
    
    Random r(5);
    
    size_t next_id = 1;
    
    C(n) {
        for (size_t node_id : cur_layer) {
            
            String label = fm::toString(node_id);
            auto shape = NodeShape::Circle;
            Node node;
            node.visuals = {10,shape};
            node.label = {label,""};
            node.body = {vec2(),1};
            node.edges = {};
            
            if (i+1 != n) {
                size_t children = pow(r.real(),.5)*k;
                Cx(children) {
                    node.edges.push_back(Edge{{4.1,NodeShape::None,NodeShape::Triangle,vec4::Royal},next_id,1});
                    nxt_layer.push_back(next_id);
                    ++next_id;
                }
            }
            
            node.selectors = {};
            v.graph.push_back(std::move(node));
        }
        cur_layer = std::move(nxt_layer);
        nxt_layer.clear();
    }
    
    return v;
}