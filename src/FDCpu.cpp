#include "FDCpu.hpp"

#include <iostream>
using namespace std;


void FDCpu::print_graph(std::ostream &out)
{
    out << m_graph.size() << endl;
    for (auto &a : m_graph) {
        for (auto &b : a) {
            out << b.strength << " ";
        }
        out << endl;
    }
}

fm::Result FDCpu::init_memory(const View &view)
{
    auto res = init_graph(view);
    res += init_bodies(view);
    
    return res;
}

fm::Result FDCpu::init_bodies(const View &view)
{
    size_t n = view.size();
    m_bodies.resize(n);
    
    for (size_t id = 0;id < n;++id) {
        m_bodies[id].p = m_layout.positions[id];
        m_bodies[id].v = vec2(0);
        m_bodies[id].m = view.graph[id].body.mass;
    }
    
    return {};
}

fm::Result FDCpu::init_graph(const View &view)
{
    m_graph.clear();
    size_t noden = view.graph.size();
    m_graph.resize(noden,vector<Edge>(noden,Edge{0}));
    
    size_t id = 0;
    for (auto &node : view.graph) {
        for (auto &edge : node.edges) {
            m_graph[id][edge.to].strength += edge.strength;
            m_graph[edge.to][id].strength += edge.strength;
        }
        
        ++id;
    }
    
    return {};
}

void FDCpu::sync_layout()
{
    C(m_layout.size())
        m_layout.positions[i] = m_bodies[i].p;
}
    
double FDCpu::distance(const vector<Body> &bodiesFrom,const vector<Body> &bodiesTo,const vector<float> &coefs)
{
    double dist = -1;
    
    C(bodiesFrom.size()) {
        double d = (bodiesFrom[i].p - bodiesTo[i].p).LENGTH() * coefs[i];
        if (d > dist) dist = d;
    }
    
    return sqrt(dist);
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

double FDCpu::error_estiamte(const vector<Body> &bodiesH,const vector<Body> &bodiesE,const vector<Body> &bodies)
{
    double err = -1;
    
    C(bodies.size()) {
        double e = (bodiesH[i].p - bodiesE[i].p).length();
        double d = (bodiesE[i].p - bodies[i].p ).length();
        
        e /= d;
        
        if (e > err) err = e;
    }
    
    return err;
}

// http://www.aip.de/groups/soe/local/numres/bookcpdf/c16-2.pdf
void FDCpu::update_bodies()
{
    auto bodies_cpy_H = m_bodies;
    auto bodies_cpy_E = m_bodies;
    
    do {
        ++m_its;
        bodies_cpy_H = m_bodies;
        bodies_cpy_E = m_bodies;
        update_with(m_bodies,bodies_cpy_H,bodies_cpy_E);
        
        m_e = distance(bodies_cpy_H,bodies_cpy_E);
        
        double N = rk().eps >= m_e ? rk().N_h : rk().N_e;
        
        m_h = std::min<float>(1,0.96*pow(rk().eps / m_e,1.0/N)*m_h);
        
    } while (m_e >= rk().eps);
    
    m_s = distance(m_bodies,bodies_cpy_E);
    
    m_bodies = bodies_cpy_E;
    
    // m_logfunc(LOG_DATA,"FDCpu update completed, new positions:");
    // for (const auto &a : m_bodies) {
    //     m_logfunc(LOG_DATA, fm::toString(a.p.x).str() + "," + fm::toString(a.p.y).str());
    // }
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

vec2 speed_sq(vec2 p,float strength)
{
    double L = p.LENGTH();
    vec2 d = -p.sgn();
	
	vec2 hooke  =  d * H * std::log(L) * strength;
    
    return hooke;
}


void FDCpu::update_with(vector<Body> &bodies,
						vector<Body> &bodiesH,
						vector<Body> &bodiesE)
{
	size_t n = bodies.size();
	size_t rk_S = rk().A.size();
	vector<vector<vec2>> k(n,vector<vec2>(rk_S,0));
	
    C(n) k[i][0] += speed_sq(bodies[i].p,m_squeeze);
	C(n) Cf(n,s) if (i != s)
        k[i][0] += speed(bodies[i].p,
                         bodies[s].p,
                         bodies[i].m*bodies[s].m,
                         m_graph[i][s].strength);
    
	for (size_t j=0;j+1<rk_S;++j) {
        C(n) k[i][j+1] += speed_sq(bodies[i].p + rk().A[j+1]*m_h*k[i][j],m_squeeze);
		C(n) Cf(n,s) if (i != s)
			k[i][j+1] += speed(bodies[i].p + rk().A[j+1]*m_h*k[i][j],
			                   bodies[s].p + rk().A[j+1]*m_h*k[s][j],
							   bodies[i].m*bodies[s].m,
                               m_graph[i][s].strength);
	}
	
	C(n) Cf(rk_S,s) {
        bodiesH[i].p += m_h*rk().C_H[s]*k[i][s];
        bodiesE[i].p += m_h*rk().C_E[s]*k[i][s];
    }
}
