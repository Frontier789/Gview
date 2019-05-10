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
    
    void doUnintersect(bool enable) {m_unintersect = enable;}
    
    size_t probe(vec2 p) const;
    
    void setActiveId(size_t id);
    
    void hide();
    void clear();

private:
    vector<GuiText*> m_texts;
    vector<float> m_radii;
    mat4 m_transf;
    Layout m_layout;
    bool m_unintersect;
    size_t m_activeId;
    
    void applyLayout();
    void applyActiveId();
};

#endif