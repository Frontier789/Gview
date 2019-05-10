#include "LayeredLayout.hpp"
#include <iostream>
#include <queue>
using namespace std;

LayeredLayout::LayeredLayout(Delegate<void,LogLevel,string> logfun) :
    LayoutGen(logfun)
{
    
}

fm::Result LayeredLayout::init(const View &view,bool cached)
{
    graph.clear();
    graph.resize(view.graph.size());
    
    C(graph.size()) {
        for (auto e : view.graph[i].edges) {
            graph[i].push_back(e.to);
            graph[e.to].push_back(i);
        }
    }
    
    for (auto &es : graph) {
        std::random_shuffle(es.begin(), es.end());
    }
    
    return {};
}

void LayeredLayout::run()
{
    if (graph.empty()) return;
    
    vector<vector<size_t>> layers;
    size_t node_count = graph.size();
    
    vector<vector<size_t>> parents(node_count);
    vector<size_t> inDegs(node_count,0);
    vector<size_t> parentOrdinalSum(node_count,0);
    vector<float>  descendantCount(node_count,0);
    vector<float>  horizontalSpace(node_count,50);
    
    vector<bool> putInLayer(node_count,false);
    vector<bool> assigned(node_count,false);
    
    { // assign ids to layers
        vector<size_t> curLayer;
        curLayer.push_back(0);
        assigned[0] = true;
        
        while (!curLayer.empty()) {
            for (auto node : curLayer) putInLayer[node] = true;
            layers.push_back(curLayer); 
            vector<size_t> nxtLayer;
            
            for (size_t i=0;i < curLayer.size();++i) {
                size_t cur = curLayer[i];
                
                for (size_t nxt : graph[cur]) {
                    if (!assigned[nxt]) {
                        horizontalSpace[cur] = 0;
                        assigned[nxt] = true;
                        nxtLayer.push_back(nxt);
                    }
                    if (!putInLayer[nxt] && nxt != cur) {
                        parents[nxt].push_back(cur);
                        inDegs[nxt]++;
                        parentOrdinalSum[nxt] += i;
                    }
                }
            }
            
            curLayer = std::move(nxtLayer);
            std::sort(curLayer.begin(),curLayer.end(),[&](size_t a,size_t b){
                size_t prio_A = parentOrdinalSum[a] * inDegs[b];
                size_t prio_B = parentOrdinalSum[b] * inDegs[a];
                
                if (prio_A == prio_B) return a < b;
                
                return prio_A < prio_B;
            });
        }
    }
    
    { // add unassigned ones as a last layer
        bool foundUnassigned = false;
        C(node_count) {
            if (!assigned[i]) {
                if (!foundUnassigned) {
                    foundUnassigned = true;
                    layers.push_back({});
                }
                layers.back().push_back(i);
            }
        }
    }
    
    C(layers.size()) {
        auto &curLayer = layers[layers.size() - 1 - i];
        
        for (size_t cur : curLayer) {
            if (parents[cur].size()) {
                float slice = horizontalSpace[cur] / parents[cur].size();
                for (size_t parent : parents[cur]) {
                    horizontalSpace[parent] += slice;
                }
            }
        }
    }
    
    // cout << "parents:" << endl;
    // C(node_count) {
    //     cout << i << " --> ";
    //     for (auto pnt : parents[i]) {
    //         cout << pnt << " ";
    //     }
    //     cout << endl;
    // }
    
    // cout << "layering:" << endl;
    // for (auto &layer : layers) {
    //     for (auto cur : layer) {
    //         cout << cur << " ";
    //     }
    //     cout << endl;
    // }
    
    // cout << "hor sapces:" << endl;
    // for (auto &layer : layers) {
    //     for (auto cur : layer) {
    //         cout << horizontalSpace[cur] << " ";
    //     }
    //     cout << endl;
    // }
        
    vector<float> horLayerSizes(layers.size(),0);
    float maxHorLayerSize = 50;
    C(layers.size()) {
        auto &layer = layers[i];
        for (size_t cur : layer) {
            horLayerSizes[i] += horizontalSpace[cur];
        }
        maxHorLayerSize = max(maxHorLayerSize, horLayerSizes[i]);
    }
    
    l.positions.resize(node_count);
    
    float y_cursor = 0;
    
    vector<float> spaceUsed(node_count,0);
    
    Cf(layers.size(),layerId) {
        auto &layer = layers[layerId];
        
        y_cursor += layer.size()*10 + 50;
        
        for (auto cur : layer) {
            float xpos = 0;
            if (parents[cur].size()) {
                auto parent = parents[cur][0];
                xpos = l.positions[parent].x + spaceUsed[parent] - horizontalSpace[parent]/2;
                xpos += horizontalSpace[cur]/2;
                spaceUsed[parent] += horizontalSpace[cur];
            }
            l.positions[cur] = vec2(xpos,y_cursor);
        }
    }
    
    lock_guard<mutex> guard(m_layoutMut);
    m_layout = l;
}

