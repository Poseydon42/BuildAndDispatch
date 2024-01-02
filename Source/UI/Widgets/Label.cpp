#include "Label.h"

#include "UI/TextLayout.h"

std::unique_ptr<Label> Label::Create(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font, TextAlignment HorizontalAlignment, TextAlignment VerticalAlignment)
{
	return std::unique_ptr<Label>(new Label(std::move(Text), FontSize, std::move(Font), HorizontalAlignment, VerticalAlignment));
}

glm::vec2 Label::ComputeContentPreferredSize() const
{
	auto Result = TextLayout::Measure(m_Text, m_FontSize, *m_Font);
	return Result;
}

void Label::Render(RenderBuffer& Buffer) const
{
	Widget::Render(Buffer);

	auto TextSize = TextLayout::Measure(m_Text, m_FontSize, *m_Font);

	auto ContentRect = ContentBoundingBox();

	Rect2D TextRect;
	switch (m_HorizontalTextAlignment)
	{
	case TextAlignment::Begin:
		TextRect.Left() = ContentRect.Left();
		TextRect.Right() = TextRect.Left() + TextSize.x;
		break;
	case TextAlignment::Center:
		TextRect.Left() = ContentRect.Left() + (ContentRect.Width() - TextSize.x) / 2.0f;
		TextRect.Right() = ContentRect.Right() - (ContentRect.Width() - TextSize.x) / 2.0f;
		break;
	case TextAlignment::End:
		TextRect.Left() = ContentRect.Left() + (ContentRect.Width() - TextSize.x);
		TextRect.Right() = ContentRect.Right();
		break;
	default:
		BD_UNREACHABLE();
	}

	switch (m_VerticalTextAlignment)
	{
	case TextAlignment::Begin:
		TextRect.Top() = ContentRect.Top();
		TextRect.Bottom() = ContentRect.Top() - TextSize.y;
		break;
	case TextAlignment::Center:
		TextRect.Top() = ContentRect.Top() - (ContentRect.Height() - TextSize.y) / 2.0f;
		TextRect.Bottom() = ContentRect.Bottom() + (ContentRect.Height() - TextSize.y) / 2.0f;
		break;
	case TextAlignment::End:
		TextRect.Top() = ContentRect.Bottom() + TextSize.y;
		TextRect.Bottom() = ContentRect.Bottom();
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
