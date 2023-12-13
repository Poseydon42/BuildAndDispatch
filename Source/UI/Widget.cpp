#include "Widget.h"

std::unique_ptr<Widget> Widget::Create()
{
	return std::unique_ptr<Widget>(new Widget);
}

void Widget::Render(RenderBuffer& Buffer) const
{
	auto IsColorNonZero = [](const glm::vec4& Color)
	{
		return glm::dot(Color, Color) >= 0.001f;
	};

	if (IsColorNonZero(m_BackgroundColor) || (IsColorNonZero(m_BorderColor) && m_BorderThickness > 0.0f))
	{
		Buffer.Rect(BoundingBox(), m_BackgroundColor, m_CornerRadius, m_BorderColor, m_BorderThickness);
	}

#ifdef UI_DEBUG
	Buffer.Debug_RectOutline(BoundingBox(), glm::vec3(0.0f, 1.0f, 0.0f));
#endif
}
