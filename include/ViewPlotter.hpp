#ifndef GVIEW_VIEWPLOTTER
#define GVIEW_VIEWPLOTTER

#include <Frontier.hpp>
using namespace std;

#include "Layout.hpp"
#include "View.hpp"
#include "LabelHandler.hpp"

struct ViewPlotter : public GuiElement, public TransformListener
{
    ViewPlotter(GuiContext &owner);
    
    void setView(View view);
    void setLayout(Layout layout);
    
    bool onEvent(fw::Event &ev) override;
    void onDraw(fg::ShaderManager &shader) override;
    void onTransform(bool userAction) override;
    void onUpdate(const fm::Time &dt) override;
    
    void center();
    void center(rect2f area);
    
    const View &view() const {return m_view;}
    
private:
    View m_view;
    bool m_empty;
    DrawData m_dd;
    LabelHandler m_labels;
    bool m_autoCenter;
    bool m_updated;
    
    void createDD();
};

#endif