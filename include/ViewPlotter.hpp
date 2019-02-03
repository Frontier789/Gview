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
    
    void onDraw(fg::ShaderManager &shader) override;
    void onTransform() override;
    
    const View &view() const {return m_view;}

private:
    View m_view;
    bool m_empty;
    DrawData m_dd;
    LabelHandler m_labels;
    
    void createDD();
};

#endif