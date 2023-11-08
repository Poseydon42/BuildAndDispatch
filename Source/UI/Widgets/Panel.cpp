#include "Panel.h"

std::unique_ptr<Panel> Panel::Create(std::unique_ptr<Brush> Brush, glm::vec2 CustomPreferredSize)
{
	return std::unique_ptr<Panel>(new Panel(std::move(Brush), CustomPreferredSize));
}

glm::vec2 Panel::ComputePreferredSize() const
{
	return m_PreferredSize;
}

void Panel::Render(RenderBuffer& Buffer) const
{
	Widget::Render(Buffer);

	Buffer.Rect(BoundingBox(), *m_Brush);
}

Panel::Panel(std::unique_ptr<Brush> Brush, glm::vec2 CustomPreferredSize)
	: m_Brush(std::move(Brush))
	, m_PreferredSize(CustomPreferredSize)
{
}
