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
        
        int treshold = 70;
        vector<NodeId> ret;
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x-1,id.y});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x+1,id.y});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x,id.y-1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x,id.y+1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x+1,id.y+1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x-1,id.y-1});
        
        return ret;
    }
};


// TODO:
//     class that runs a graphplotter on a separate thread
//      - how to communicate?
//     latice drawing


int main() {
    ViewLoader<MyImporter> vl;
    
    auto v1 = vl.load(avec2i{2,2},ViewParams{2});
   
    FDLayoutCPU gen;
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
    
    cout << "points: " << endl;
    for (auto p : desc.positions) {
        cout << p << endl;
    }
    cout << "bounding rect: " << desc.aabb << endl;
    
    Plot p;
    p.graph = std::move(desc.graph);
    p.positions = std::move(desc.positions);
    
    GraphPlotterFlib plt;
    plt.start();
    plt.plot(std::move(p));
    plt.wait();
}
