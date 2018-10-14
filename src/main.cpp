#include "GraphmlImporter.hpp"
#include "PajekImporter.hpp"
#include "FDLayoutCPU.hpp"
#include "Groupper.hpp"
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
        {
            vector<NodeId> ret;
            if (id.x > 0) ret.push_back(avec2i{id.x-1,id.y});
            ret.push_back(avec2i{id.x+1,id.y});
            if (id.y > 0) ret.push_back(avec2i{id.x,id.y-1});
            ret.push_back(avec2i{id.x,id.y+1});
            
            return ret;
        }
        
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

class MyGroupper : public Groupper<avec2i>
{
public:
    bool sameGroup(avec2i a,avec2i b) const override {
        // if (a.x == 0 && b.x == 0) return true;
        return false;
    }
    
    String label(avec2i a) const override {
        if (a.x == 0) return "SPECIAL";
        auto imp = getImporter();
        return imp->label(a);
    }
};

// TODO:
//     latice drawing
//     animated view change
//     animated zoom
//     groupping

int main(int argc,char *argv[]) {
    int depth = 1;
    if (argc > 1) depth = atoi(argv[1]);
    
    auto layoutgen = new FDLayoutCPU(0.000001);
    
    if (argc > 2) {
        auto importer = new PajekImporter("modules.net");
        Gview gview(importer,layoutgen);
        gview.start(importer->toPriv(21),ViewParams{depth});
        gview.wait();
    } else {
        auto importer = new MyImporter();
        auto groupper = new MyGroupper();
        Gview gview(importer,layoutgen,groupper);
        gview.start(importer->toPriv(avec2i{1,1}),ViewParams{depth});
        gview.wait();
    }
}
