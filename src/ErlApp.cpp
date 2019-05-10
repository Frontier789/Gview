#include "ErlApp.hpp"
#include "LayeredLayout.hpp"
#include "FDGpuMat.hpp"

ErlApp::ErlApp(Delegate<void,LogLevel,string> logFunc,bool enable_gpu,bool enable_cache) :
    App(logFunc,enable_gpu,enable_cache)
{
    
}

fm::Result ErlApp::init() {
    m_importer = new ErlImporter(logFunc);
    
    plotter->setClickCb([&](SelectionData sd){
        if (sd.selected == SelectionData::Node) {
            importer()->send("clicked");
            importer()->send("node");
            importer()->send(sd.node_id);
        } else if (sd.selected == SelectionData::Selector) {
            importer()->send("clicked");
            importer()->send("sel");
            importer()->send(sd.node_id);
            importer()->send(sd.selector_id);
        } else {
            logFunc(LOG_WARNING,"Click callback called with unknown selection type");
        }
    });
    
    plotter->enableLabelOverlap(true);
    
    Widget *widget = new Widget(win);
    win.addElement(widget);
    
    widget->onkeypress = [&](Keyboard::Key key){
        if (key == Keyboard::R) {
            m_cache.clear();
            reloadCurrentView();
        }
        if (key == Keyboard::Backspace || (key == Keyboard::Z && Keyboard::isKeyHeld(Keyboard::LCtrl))) {
            importer()->send("undo");
        }
        if (key == Keyboard::Y && Keyboard::isKeyHeld(Keyboard::LCtrl)) {
            importer()->send("redo");
        }
        if (key == Keyboard::R && Keyboard::isKeyHeld(Keyboard::LCtrl)) {
            importer()->send("reload");
        }
        if (key == Keyboard::L) {
            plotter->enableLabels(!plotter->areLabelsEnabled());
        }
    };
    
    return {};
}

void ErlApp::update() {
    
    App::update();
    
    if (!importer()->check()) {
        logFunc(LOG_ERROR,importer()->check().toString());
        exit(0);
    }
    
    if (importer()->hasInput()) {
        logFunc(LOG_INFO,"Input available ");
        string s = importer()->recv<string>();
        logFunc(LOG_INFO,"Received command: " + s);
        
        if (s == "center") {
            plotter->center();
        }
        else if (s == "layout") {
            string method = importer()->recv<string>();
            if (method == "layered") {
                delete m_gen;
                m_gen = new LayeredLayout(logFunc);
                m_gen->startWorkerThread();
                reloadCurrentView();
                plotter->enableLabelOverlap(false);
            }
            else if (method == "fd") {
                delete m_gen;
                m_gen = new FDGpuMat(RKDesc::fehlberg(),false,logFunc);
                m_gen->startWorkerThread();
                reloadCurrentView();
                plotter->enableLabelOverlap(true);
            }
            else {
                logFunc(LOG_WARNING,"Received unknown layout method: " + method);
            }
        }
        else if (s == "update_props") {
            sendTransf();
        }
        else if (s == "stop") {
            stop();
        }
        else if (s == "set_view") {
            // Clock clk;
            loadCurrentView();
            // logFunc(LOG_INFO, "erl input (" + fm::toString(m_curView.graph.size()).str() + "): " + fm::toString(clk.s(),5).str() + "s");clk.restart();
        }
        else if (s == "drop_cache") {
            m_cache.clear();
            reloadCurrentView();
        }
        else if (s == "set_cache") {
            m_cache.saveToFile();
            string s = importer()->recv<string>();
            logFunc(LOG_INFO,"Cache file name was set to: " + s);
            
            m_cache.setFile(s);
        }
        else if (s == "labels") {
            string method = importer()->recv<string>();
            if (method == "hide") {
                plotter->enableLabels(false);
            }
            else if (method == "normal") {
                plotter->enableLabelOverlap(true);
                plotter->enableLabels(true);
            }
            else if (method == "move") {
                plotter->enableLabelOverlap(false);
                plotter->enableLabels(true);
            }
            else {
                logFunc(LOG_WARNING,"Received unknown label method: " + method);
            }
        }
        else {
            logFunc(LOG_WARNING,"Received unknown command: " + s);
        }
    }
}

void ErlApp::sendTransf()
{
    float zoom = plotter->getZoom();
    vec2 offset = plotter->getOffset();
    float rot = plotter->getRotation().asDeg();
    
    importer()->send("transf");
    importer()->send(vector<float>{zoom,offset.x,offset.y,rot});
}
