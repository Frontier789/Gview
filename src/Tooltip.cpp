#include "Tooltip.hpp"
#include <iostream>
using namespace std;

Tooltip::Tooltip(GuiContext &owner,Delegate<void,LogLevel,string> logfunc) :
    GuiElement(owner, owner.getSize()),
    m_logFunc(logfunc),
    m_shown(false),
    m_shouldShow(false),
    m_str("")
{
    m_guiText = new GuiText(owner,"");
    owner.addElement(m_guiText);
    m_bckgSpr = owner.getSprite("Text_Bckg");
    
    do_hide();
}

void Tooltip::setText(String str)
{
    m_str = str;
    if (m_str == "") {
        do_hide();
    } else {
        show();
    }
}

void Tooltip::hide()
{
    if (m_shown && m_shouldShow) {
        m_shouldShow = false;
        m_clk.restart();
    }
}

void Tooltip::show()
{
    if (m_str.size()) {
        m_shown = false;
        m_shouldShow = true;
        m_clk.restart();
    }
}

void Tooltip::onMouseMove(fm::vec2 p,fm::vec2 prevp)
{
    if ((p - prevp).length() > 2 && m_str.size()) {
        show();
    }
}

void Tooltip::do_hide()
{
    m_shown = false;
    m_shouldShow = false;
    m_guiText->setText("");
    m_bckgSpr.setPosition(m_bckgSpr.getSize()*-2.f);
}

void Tooltip::onUpdate(const fm::Time &)
{
    if (m_shown && !m_shouldShow) {
        if (m_clk.s() > .2) {
            do_hide();
        }
    }
    if (!m_shown && m_shouldShow) {
        if (m_clk.s() > .2) {
            m_shown = true;
            
            m_guiText->setText(m_str);
            {
                vec2 size = m_guiText->getSize();
                vec2 pos = getLastMousePos() + vec2(0,24);
                vec2 bounds = getSize();
                if (pos.x + size.w > bounds.w) pos.x = bounds.w - size.w;
                if (pos.y + size.h > bounds.h) pos.y = bounds.h - size.h;
                m_guiText->setPosition(vec3(pos,-2));
            }
            
            vec2 frame = vec2(3,2);
            vec2 pos = m_guiText->getPosition() - frame;
            m_bckgSpr.setPosition(vec3(pos, -1));
            m_bckgSpr.setSize(m_guiText->getSize() + frame*2);
        }
    }
}

