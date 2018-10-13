#include "GraphmlImporter.hpp"
#include "Importer.hpp"
#include "Gview.hpp"
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
    vector<NodeId> outEdges(NodeId id) const override {
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
//     latice drawing
//     graphml importer
//     animated view change

int main() {
    {
        auto importer = new GraphmlImporter("hello.xml");
        Gview gview(importer);
        gview.start(importer->toPriv("n0"),ViewParams{3});
        gview.wait();
        return 0;
    }
    
    auto importer = new MyImporter();
    Gview gview(importer);
    gview.start(importer->toPriv(avec2i{1,1}),ViewParams{3});
    gview.wait();
    return 0;
}
