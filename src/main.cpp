#include "GraphPlotterFlib.hpp"
#include "LayoutGenerator.hpp"
#include "GraphPlotter.hpp"
#include "FDLayoutCPU.hpp"
#include "ViewLoader.hpp"
#include "ViewParams.hpp"
#include "Importer.hpp"
#include "Logger.hpp"
#include "View.hpp"
#include <iostream>
using namespace std;

class avec2i
{
public:
    int x,y;
    bool operator==(avec2i p) const {return x==p.x && y==p.y;}
    bool operator<(avec2i v) const {if (x != v.x) return x < v.x; return y < v.y;}
};

class MyImporter : public Importer<avec2i>
{
public:
    vector<NodeId> outEdges(NodeId id) const {
        int h = id.x ^ id.y + id.x*id.y;
        srand(h);
        
        int treshold = 50;
        vector<NodeId> ret;
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x-1,id.y});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x+1,id.y});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x,id.y-1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x,id.y+1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x+1,id.y+1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x-1,id.y-1});
        
        return ret;
    }
    
    String label(NodeId id) const override {
        return "{" + toString(id.x) + "," + toString(id.y) + "}";
    }
};


// TODO:
//     class that runs a graphplotter on a separate thread
//      - how to communicate?
//     latice drawing


int main() {
    
    // rect2f a(310.909,184.472,84,14);
    // rect2f b(322.793,179.735,84,14);
    // cout << a.intersects(b) << endl;
    // cout << (a.pos.x < b.pos.x + b.size.w && a.pos.x + a.size.w > b.pos.x &&
    //         a.pos.y < b.pos.y + b.size.h && a.pos.y + a.size.h > b.pos.y) << endl;
    // return 0;
    
    ViewLoader<MyImporter> vl;
    
    auto v1 = vl.load(avec2i{2,2},ViewParams{3});
   
    FDLayoutCPU gen(0.000001);
    gen.run(std::move(v1.graph));
    
    cout << "gen started\n";
    gen.wait(1);
    if (!gen.ready()) {
        cout << "ERROR: timout :|" << endl;
        gen.stop();
        gen.wait();
    }
    else
        cout << "gen done" << endl;
    
    LayoutDesc desc = gen.layout();
    
    Plot p;
    p.graph = std::move(desc.graph);
    p.positions = std::move(desc.positions);
    p.aabb = desc.aabb;
    
    GraphPlotterFlib plt;
    plt.start();
    plt.plot(std::move(p));
    plt.wait();
}
