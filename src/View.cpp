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
    
size_t View::probe(mat4 transf,vec2 mp) const
{
    float zoom = vec2(transf * vec4(1,0,0,0)).length();
    
    C(size()) {
		
		vec2 pt = transf * vec4(graph[i].body.pos,0,1);
		float rt = graph[i].visuals.size * zoom;
		
		if ((pt - mp).LENGTH() < rt*rt) {
			return i;
		}
	}
    
    return size_t(-1);
}

vector<string> View::selectorLabels(size_t nodeId) const
{
    vector<string> ret;
    if (nodeId < graph.size()) {
        auto &selectors = graph[nodeId].selectors;
        ret.reserve(selectors.size());
        for (auto &sel : selectors) {
            ret.push_back(sel.label);
        }
    }
    return ret;
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

namespace {
    template<class T>
    void addToDesc(vector<uint8_t> &desc,T t) {
        desc.insert(desc.end(), (uint8_t*)&t, (uint8_t*)&t + sizeof(T));
    }
}

string View::getHash() const
{
    vector<uint8_t> desc;
    for (auto &node : graph) {
        addToDesc(desc,node.body.mass);
        for (auto &e : node.edges) {
            addToDesc(desc,e.to);
            addToDesc(desc,e.strength);
        }
    }
    
    return fm::sha256(desc.begin(),desc.end());
}
    
bool View::isJoint() const
{
    if (size() == 0) return true;
    
    vector<vector<int>> g(size());
    C(size())
        for (auto e : graph[i].edges) {
            g[i].push_back(e.to),
            g[e.to].push_back(i);
        }
    
    size_t visc = 0;
    vector<bool> visited(size(),false);
    vector<int> q;
    
    visited[0] = true;
    q.push_back(0);
    ++visc;
    
    while (!q.empty()) {
        int n = q.back();
        q.pop_back();
        
        for (int e : g[n]) {
            if (!visited[e]) {
                visited[e] = true;
                q.push_back(e);
                ++visc;
            }
        }
    }
    
    return visc == size();
}
