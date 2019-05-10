#ifndef GVIEW_LAYEREDLAYOUTGEN
#define GVIEW_LAYEREDLAYOUTGEN

#include <Frontier.hpp>
using namespace std;

#include "LayoutGen.hpp"

struct LayeredLayout : public LayoutGen
{
    LayeredLayout(Delegate<void,LogLevel,string> logfun = nullptr);
    
    void run() override;
    
    string tag() const override {return "layered";}
    
private:
    vector<vector<size_t>> graph;
    Layout l;
    
    fm::Result init(const View &view,bool cached) override;
};

#endif