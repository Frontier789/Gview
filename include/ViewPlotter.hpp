#ifndef GVIEW_VIEWPLOTTER
#define GVIEW_VIEWPLOTTER

#include <Frontier.hpp>
using namespace std;

#include "Layout.hpp"
#include "View.hpp"
#include "LabelHandler.hpp"
#include "LogLevel.hpp"
#include "SelectorButtons.hpp"
#include "SelectionData.hpp"
#include "Tooltip.hpp"

struct ViewPlotter : public GuiElement, public TransformListener
{
    ViewPlotter(GuiContext &owner,Delegate<void,LogLevel,string> logfunc);
    
    void setView(View view);
    void setLayout(Layout layout);
    
    bool onEvent(fw::Event &ev) override;
    void onDraw(fg::ShaderManager &shader) override;
    void onTransform(bool userAction) override;
    void onUpdate() override;
    
    void enableLabelOverlap(bool enable) {m_labels.doUnintersect(!enable);};
    
    void center();
    void center(rect2f area);
    
    void setTransfCb(Delegate<void,bool> cb);
    void setClickCb(Delegate<void,SelectionData> cb);
    
    const View &view() const {return m_view;}
    
    fm::Result loadShaders();
    
    void onMouseMove(fm::vec2 p,fm::vec2 prevP) override;
    void onClick(fw::Mouse::Button button,fm::vec2 p) override;
    
    void enableLabels(bool enable);  
    bool areLabelsEnabled() const {return m_plotLabels;}  

private:
    View m_view;
    bool m_empty;
    LabelHandler m_labels;
    SelectorButtons m_selBtns;
    Tooltip *m_tooltip;
    bool m_autoCenter;
    bool m_updated;
    bool m_mouseHover;
    bool m_plotLabels;
    vec2 m_grabp;
    bool m_canClick;
    
    size_t m_hoveredId;
    
    DrawData m_ddOutline;
    DrawData m_ddFill;
    ShaderManager m_aaShader;
    
    Delegate<void,bool> m_transfCb;
    Delegate<void,SelectionData> m_clickCb;
    Delegate<void,LogLevel,string> m_logFunc;
    
    void createDD();
    void findHovered(vec2 mp);
};

#endif