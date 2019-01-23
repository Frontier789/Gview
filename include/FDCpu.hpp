#ifndef GVIEW_FDCPU
#define GVIEW_FDCPU

#include <Frontier.hpp>
using namespace std;

#include "LayoutGen.hpp"
#include "RKDesc.hpp"

struct FDCpu : public LayoutGen
{
    FDCpu(RKDesc rk = RKDesc::fehlberg(),bool do_logging = false) : m_rk(rk) { log.do_log = do_logging; }
    
    void init(View &view) override;
    void run() override;
    
    void init_bodies();
    void init_graph(View &view);
    void print_graph();
    void update_bodies();
    
    struct Log {
        vector<float> h; ///< Stepsize of RK
        vector<float> e; ///< Error estimate of RK
        vector<float> s; ///< Maximum of delta of position of bodies
        vector<int> its; ///< Iterations of RK
        bool do_log;
    } log;

private:
    struct Body {
        vec2 p,v;
        float m;
    };
    
    struct Edge {
        float strength;
    };
    
    RKDesc m_rk;
    vector<vector<Edge>> m_graph;
    vector<Body> m_bodies;
    double m_h;
    double m_s;
    
    double distance(const vector<Body> &bodiesFrom,const vector<Body> &bodiesTo);
    
    void update_with(vector<Body> &bodies,
                     vector<Body> &bodiesH,
                     vector<Body> &bodiesE);
};

#endif