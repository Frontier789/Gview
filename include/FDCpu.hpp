#ifndef GVIEW_FDCPU
#define GVIEW_FDCPU

#include <Frontier.hpp>
using namespace std;

#include "FDLayoutGen.hpp"
#include "RKDesc.hpp"

struct FDCpu : public FDLayoutGen
{
    FDCpu(RKDesc rk = RKDesc::fehlberg(),bool do_logging = false,Delegate<void,LogLevel,string> logfun = nullptr) : FDLayoutGen(rk,do_logging,logfun) {}
    
    fm::Result init_memory(const View &view) override;
    void update_bodies() override;
    
    void print_graph(std::ostream &out);

private:
    struct Body {
        vec2 p,v;
        float m;
    };
    
    struct Edge {
        float strength;
    };
    
    vector<vector<Edge>> m_graph;
    vector<Body> m_bodies;
    
    fm::Result init_bodies(const View &view);
    fm::Result init_graph(const View &view);
    
    double distance(const vector<Body> &bodiesFrom,const vector<Body> &bodiesTo,const vector<float> &coefs);
    double distance(const vector<Body> &bodiesFrom,const vector<Body> &bodiesTo);
    double error_estiamte(const vector<Body> &bodiesH,const vector<Body> &bodiesE,const vector<Body> &bodies);

    void sync_layout() override;
    void update_with(vector<Body> &bodies,
                     vector<Body> &bodiesH,
                     vector<Body> &bodiesE);
};

#endif