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
            NodeShape end1;
            NodeShape end2;
            vec4 color;
            
        } visuals;
        size_t to;
        float strength;
    };
    
    struct Node {
        struct Visuals {
            float size;
            NodeShape shape;
            
        } visuals;
        
        struct Label {
            String text;
            String tooltip;
            
        } label;
        
        struct Body {
            vec2 pos;
            float mass;
        } body;
        
        struct Selector {
            String label;
            size_t id; 
        };
        
        vector<Selector> selectors;
        vector<Edge> edges;
    };
    vector<Node> graph;
    
    Layout getLayout() const;
    void setLayout(const Layout &layout);
    size_t size() const {return graph.size();}
    rect2f aabb(mat4 transf = mat4()) const;
    
    vector<string> selectorLabels(size_t nodeId) const;
    size_t probe(mat4 transf,vec2 mp) const;
    
    bool isJoint() const;
    
    string getHash() const;
    
};

#endif