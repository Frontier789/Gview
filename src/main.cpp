#include <fstream>
#include <iostream>
#include <Frontier.hpp>
using namespace std;

#include "FDCpu.hpp"
#include "ViewPlotter.hpp"

Random random(42);

View getTestView()
{
    typedef View::Node Node;
    typedef View::Edge Edge;
    View v;
    size_t n = 5;
    Cx(n)Cy(n) {
        String label = "{" + fm::toString(x) + "," + fm::toString(y) + "}";
        v.graph.push_back({{10,{label}}, {vec2(),1}, {}});
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
    
    FDCpu *gen = new FDCpu(RKDesc::fehlberg(),true);
    gen->start(plotter->view());
    
    Clock clk;
    gen->wait();
    plotter->setLayout(gen->layout());
    cout << "generation done" << endl;
    cout << "t = " << clk.s()*1000 << "ms" << endl;
    
    win.runGuiLoop();
    
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
