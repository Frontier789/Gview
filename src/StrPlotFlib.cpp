#include "GraphPlotterFlib.hpp"
#include "Logger.hpp"

namespace priv
{
	StrPlotFlib::StrPlotFlib() : m_layout(nullptr), m_size(0) {}
	
	void StrPlotFlib::layout(GuiLayout *layout)
	{
		m_layout = layout;
	}
	
	void StrPlotFlib::resize(size_t n)
	{
		m_layout->setChildCount(n);
		auto &cont = m_layout->getOwnerContext();
		
		for (size_t i=m_size;i<n;++i) {
			auto txt = new GuiText(cont);
			txt->setCharacterSize(12);
			m_layout->setChildElement(i, txt);
		}
		
		m_size = n;
	}
	
	void StrPlotFlib::set(size_t i,String str)
	{
		GuiText *txt = m_layout->getChildElement<GuiText>(i);
		txt->setText(str);
	}
	
	void StrPlotFlib::set(size_t i,vec2 pos)
	{
		GuiText *txt = m_layout->getChildElement<GuiText>(i);
		vec2 s = txt->getSize();
		txt->setPosition(pos - s * vec2(.5,0));
	}

	vec2 StrPlotFlib::size(size_t i)
	{
		GuiText *txt = m_layout->getChildElement<GuiText>(i);
		return txt->getSize();
	}
}
