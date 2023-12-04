#include "Label.h"

#include "UI/TextLayout.h"

std::unique_ptr<Label> Label::Create(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font, TextAlignment HorizontalAlignment, TextAlignment VerticalAlignment)
{
	return std::unique_ptr<Label>(new Label(std::move(Text), FontSize, std::move(Font), HorizontalAlignment, VerticalAlignment));
}

glm::vec2 Label::ComputePreferredSize() const
{
	auto Result = TextLayout::Measure(m_Text, m_FontSize, *m_Font);
	return Result;
}

void Label::Render(RenderBuffer& Buffer) const
{
	auto TextSize = TextLayout::Measure(m_Text, m_FontSize, *m_Font);

	Rect2D TextRect;
	switch (m_HorizontalTextAlignment)
	{
	case TextAlignment::Begin:
		TextRect.Left() = BoundingBox().Left();
		TextRect.Right() = TextRect.Left() + TextSize.x;
		break;
	case TextAlignment::Center:
		TextRect.Left() = BoundingBox().Left() + (BoundingBox().Width() - TextSize.x) / 2.0f;
		TextRect.Right() = BoundingBox().Right() - (BoundingBox().Width() - TextSize.x) / 2.0f;
		break;
	case TextAlignment::End:
		TextRect.Left() = BoundingBox().Left() + (BoundingBox().Width() - TextSize.x);
		TextRect.Right() = BoundingBox().Right();
		break;
	default:
		BD_UNREACHABLE();
	}

	switch (m_VerticalTextAlignment)
	{
	case TextAlignment::Begin:
		TextRect.Top() = BoundingBox().Top();
		TextRect.Bottom() = BoundingBox().Top() - TextSize.y;
		break;
	case TextAlignment::Center:
		TextRect.Top() = BoundingBox().Top() - (BoundingBox().Height() - TextSize.y) / 2.0f;
		TextRect.Bottom() = BoundingBox().Bottom() + (BoundingBox().Height() - TextSize.y) / 2.0f;
		break;
	case TextAlignment::End:
		TextRect.Top() = BoundingBox().Bottom() + TextSize.y;
		TextRect.Bottom() = BoundingBox().Bottom();
		break;
	default:
		BD_UNREACHABLE();
	}

	Buffer.Text(m_Text, m_FontSize, *m_Font, TextRect);
}

Label::Label(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font, TextAlignment HorizontalAlignment, TextAlignment VerticalAlignment)
	: m_Text(std::move(Text))
	, m_HorizontalTextAlignment(HorizontalAlignment)
	, m_VerticalTextAlignment(VerticalAlignment)
	, m_FontSize(FontSize)
	, m_Font(std::move(Font))
{
}
