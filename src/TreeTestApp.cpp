#include "TreeTestApp.hpp"
#include "LayeredLayout.hpp"
#include "FDGpuMat.hpp"
#include <iostream>
using namespace std;

TreeTestApp::TreeTestApp(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache) :
    App([&,logFunc](LogLevel lvl,string s){
        logFunc(lvl,s);
        if (lvl <= LOG_INFO) {
            cout << lvl << ": " << s << endl;
        }
    },enable_gpu,enable_cache)
{
    m_gentimeout = 0;
    m_cache.setFile("cache/tree_test.cache");
}

void TreeTestApp::createImporter() {
    m_importer = new TreeImporter(3,4);
}

void TreeTestApp::createLayoutGen() {
    App::m_gen = new LayeredLayout(logFunc);
}

fm::Result TreeTestApp::init() {
    Widget *widget = new Widget(win);
    win.addElement(widget);
    
    widget->onkeypress = [&](Keyboard::Key key){
        if (key == Keyboard::R) {
            m_cache.clear();
            reloadCurrentView();
        }
        if (key == Keyboard::L) {
            plotter->enableLabels(!plotter->areLabelsEnabled());
        }
        if (key == Keyboard::Plus || key == Keyboard::Minus) {
            int delta = key == Keyboard::Plus ? 1 : -1;
            importer()->setN(importer()->getN() + delta);
            
            loadCurrentView();
        }
        if (key == Keyboard::A || key == Keyboard::B) {
            int delta = key == Keyboard::A ? 1 : -1;
            importer()->setK(importer()->getK() + delta);
            
            loadCurrentView();
        }
    };
    
    plotter->enableLabelOverlap(false);
    
    loadCurrentView();
    
    return {};
}
