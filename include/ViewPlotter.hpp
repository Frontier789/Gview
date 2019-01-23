#ifndef GVIEW_VIEWPLOTTER
#define GVIEW_VIEWPLOTTER

#include <Frontier.hpp>
using namespace std;

#include "Layout.hpp"
#include "View.hpp"

struct ViewPlotter : public GuiElement, public TransformListener
{
    ViewPlotter(GuiContext &owner);
    
    void setView(View view);
    void setLayout(Layout layout);
    
    void onDraw(fg::ShaderManager &shader) override;

    View m_view;
private:
    bool m_empty;
    DrawData m_dd;
    
    void createDD();
};

#endif