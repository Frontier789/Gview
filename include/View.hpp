#ifndef GVIEW_VIEW
#define GVIEW_VIEW

#include <Frontier.hpp>
using namespace std;

#include "Layout.hpp"

enum class NodeShape
{
    Circle   = 1,
    Square   = 2,
    Triangle = 3,
    None     = 0
};

struct View
{
    struct Edge {
        struct Visuals {
            float width;
            
        } visuals;
        size_t to;
        
    };
    
    struct Node {
        struct Visuals {
            float size;
            NodeShape shape;
            
            struct Label {
                String text;
                
            } label;
            
        } visuals;
        
        struct Body {
            vec2 pos;
            float mass;
        } body;
        
        vector<Edge> edges;
    };
    vector<Node> graph;
    
    Layout getLayout() const;
    void setLayout(const Layout &layout);
    size_t size() const {return graph.size();}
    rect2f aabb() const;
};

#endif