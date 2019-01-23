#include "FDCpu.hpp"

#include <iomanip>
#include <iostream>

void FDCpu::init(View &view)
{
    init_graph(view);
    m_bodies.resize(view.size());
    init_bodies();
    
    m_h = m_rk.h0;
    
    if (view.graph.size() < 30)
        print_graph();
}

void FDCpu::print_graph()
{
    cout << m_graph.size() << endl;
    for (auto &a : m_graph) {
        for (auto &b : a) {
            cout << b.strength << " ";
        }
        cout << endl;
    }
}

void FDCpu::init_bodies()
{
    size_t n = m_bodies.size();
    
    Anglef base = fm::deg(360.0 / n);
    
    for (size_t id = 0;id < n;++id) {
        m_bodies[id].p = pol2(100, base * id);
        m_bodies[id].v = vec2(0);
        m_bodies[id].m = 1;
    }
}

void FDCpu::init_graph(View &view)
{
    m_graph.clear();
    size_t noden = view.graph.size();
    m_graph.resize(noden,vector<Edge>(noden,Edge{0}));
    
    size_t id = 0;
    for (auto &node : view.graph) {
        for (auto &edge : node.edges) {
            m_graph[id][edge.to] = Edge{1};
            m_graph[edge.to][id] = Edge{1};
        }
        
        ++id;
    }
}

void FDCpu::run()
{
    bool finished = false;
    
    for (size_t step=0;!stopped() && !finished;++step)
    {
        // Sleep(milliseconds(1)); ///< Spare cpu ...?
        
        update_bodies();
        if (m_s < 0.01 && step > 30) finished = true;
        
        std::lock_guard<std::mutex> guard(m_layoutMut);
        C(m_layout.size())
             m_layout.positions[i] = m_bodies[i].p;
    }
}
    
double FDCpu::distance(const vector<Body> &bodiesFrom,const vector<Body> &bodiesTo)
{
    double dist = -1;
    
    C(bodiesFrom.size()) {
        double d = (bodiesFrom[i].p - bodiesTo[i].p).LENGTH();
        if (d > dist) dist = d;
    }
    
    return sqrt(dist);
}

// http://www.aip.de/groups/soe/local/numres/bookcpdf/c16-2.pdf
void FDCpu::update_bodies()
{
    // for (auto b : bodies) {
    //     cout << fixed << setprecision(4);
    //     cout << "p = " << b.p << " v = " << b.v << endl;
    // } cout << endl;
    
    auto bodies_cpy_H = m_bodies;
    auto bodies_cpy_E = m_bodies;
    double e;
    int its = 0;
    
    do {
        ++its;
        bodies_cpy_H = m_bodies;
        bodies_cpy_E = m_bodies;
        update_with(m_bodies,bodies_cpy_H,bodies_cpy_E);
        
        e = distance(bodies_cpy_H,bodies_cpy_E);
        
        // cout << "error = " << fixed << setprecision(15) << e << endl;
        // cout << "eps   = " << fixed << setprecision(15) << m_rk.eps << endl;
        
        double N = m_rk.eps >= e ? m_rk.N_h : m_rk.N_e;
        
        m_h = 0.96*pow(m_rk.eps / e,1.0/N)*m_h;
        // cout << "new h = " << m_h << endl;
    } while (e >= m_rk.eps);
    
    // cout << "h = " << m_h << endl;
    // cout << "e = " << fixed << setprecision(15) << e << endl;
    // cout << endl;
    
    m_s = distance(m_bodies,bodies_cpy_E);
    
    if (log.do_log) {
        log.h.push_back(m_h);
        log.e.push_back(e);
        log.s.push_back(m_s);
        log.its.push_back(its);
    }
    
    m_bodies = bodies_cpy_E;
}

// l^2 * log(l) * 2 = G
float G = 30*30*std::log(30)*2;
float H = 1;

vec2 speed(vec2 A,vec2 B,double m,float strength)
{
	auto d = B - A;
    double L = d.LENGTH();
    d = d.sgn();
	
	vec2 culomb = -d * G * m / L;
    vec2 hooke  =  d * H * std::log(L) * strength;
    
    return culomb + hooke;
}

void FDCpu::update_with(vector<Body> &bodies,
						vector<Body> &bodiesH,
						vector<Body> &bodiesE)
{
	size_t n = bodies.size();
	size_t rk_S = m_rk.A.size();
	vector<vector<vec2>> k(n,vector<vec2>(rk_S,0));
	
	C(n) Cf(n,s) if (i != s)
        k[i][0] += speed(bodies[i].p,bodies[s].p,bodies[s].m,m_graph[i][s].strength);
    
	for (size_t j=0;j+1<rk_S;++j) {
		C(n) Cf(n,s) if (i != s)
			k[i][j+1] += speed(bodies[i].p + m_rk.A[j+1]*m_h*k[i][j],
			                   bodies[s].p + m_rk.A[j+1]*m_h*k[s][j],
							   bodies[s].m,m_graph[i][s].strength);
	}
	
	C(n) Cf(rk_S,s) {
        bodiesH[i].p += m_h*m_rk.C_H[s]*k[i][s];
        bodiesE[i].p += m_h*m_rk.C_E[s]*k[i][s];
    }
}
