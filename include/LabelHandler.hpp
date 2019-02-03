#ifndef GVIEW_LABELHANDLER
#define GVIEW_LABELHANDLER

#include <vector>
#include <Frontier.hpp>

#include "View.hpp"
#include "Layout.hpp"

struct LabelHandler : public GuiElement
{
    LabelHandler(GuiContext &cont,bool do_unintersect = true);
    
    void setView(const View &view);
    void setLayout(Layout layout);
    void setTransform(mat4 transf);

private:
    vector<GuiText*> m_texts;
    mat4 m_transf;
    Layout m_layout;
    bool m_unintersect;
    
    void applyLayout();
};

#endif