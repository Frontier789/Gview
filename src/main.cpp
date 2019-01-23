#include <fstream>
#include <iostream>
#include <Frontier.hpp>
using namespace std;

#include "FDCpu.hpp"
#include "ViewPlotter.hpp"

class Random
{
    std::mt19937_64 m_gen;

public:
    Random(int seed) : m_gen(seed) {}
    
    double real(double beg = 0,double end = 1);
    size_t integer(size_t beg = 0,size_t end = size_t(-1));
};

double Random::real(double beg,double end) {
    std::uniform_real_distribution<double> distribution(beg, end);
    return distribution(m_gen);
}

size_t Random::integer(size_t beg,size_t end) {
    std::uniform_int_distribution<size_t> distribution(beg, end);
    return distribution(m_gen);
}

Random random(42);

View getTestView()
{
    typedef View::Node Node;
    typedef View::Edge Edge;
    View v;
    size_t n = 5;
    Cx(n)Cy(n) {
        v.graph.push_back({{10}, {vec2(),1}, {}});
    }
    
    Cx(n)Cy(n) {
        size_t a = x*n+y;
        size_t b = (x+1)*n+y;
        size_t c = x*n+(y+1);
        if (x+1 < n)
            v.graph[a].edges.push_back({{1},b});
        if (y+1 < n)
            v.graph[a].edges.push_back({{1},c});
    }
    
    return v;
}

int main() {
    GuiWindow win(vec2(640,480),"win");
    
    auto plotter = new ViewPlotter(win);
    plotter->setView(getTestView());
    
    win.addElement(plotter);
    
    FDCpu *gen = new FDCpu(RKDesc::heun_euler(),true);
    gen->start(plotter->m_view);
    
    bool done = false;
    Clock clk;
    win.runGuiLoop([&]{
        if (!gen->ready())
            plotter->setLayout(gen->layout());
        else if (!done) {
            done = true;
            cout << "generation done" << endl;
            cout << "t = " << clk.s()*1000 << "ms" << endl;
        }
    });
    
    ofstream out_log("out.log");
    bool fst;
    
    for (auto &vec : {gen->log.h,gen->log.e,gen->log.s}) {
        fst = true;
        for (auto f : vec) {
            if (!fst) out_log << ","; fst = false;
            out_log << f;
        } out_log << endl;
    }
}
