#ifndef GVIEW_TOOLTIP
#define GVIEW_TOOLTIP

#include <Frontier.hpp>
using namespace std;

#include "LogLevel.hpp"

struct Tooltip : public GuiElement, public MouseMoveListener
{
    Tooltip(GuiContext &owner,Delegate<void,LogLevel,string> logfunc);
    
    void setText(String str);
    
    void onMouseMove(fm::vec2 p,fm::vec2 prevP) override;
    void onUpdate() override;

private:
    void hide();
    void show();
    void do_hide();
    
    bool m_shouldShow;
    bool m_shown;
    Clock m_clk;
    String m_str;
    GuiText *m_guiText;
    Sprite m_bckgSpr;
    Delegate<void,LogLevel,string> m_logFunc;
};

#endif