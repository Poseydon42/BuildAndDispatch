#include "Label.h"

#include "UI/TextLayout.h"

std::unique_ptr<Label> Label::Create(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font)
{
	return std::unique_ptr<Label>(new Label(std::move(Text), FontSize, std::move(Font)));
}

glm::vec2 Label::ComputePreferredSize() const
{
	auto Result = TextLayout::Measure(m_Text, m_FontSize, *m_Font);
	return Result;
}

void Label::Render(RenderBuffer& Buffer) const
{
	Buffer.Text(m_Text, m_FontSize, *m_Font, BoundingBox());
}

Label::Label(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font)
	: m_Text(std::move(Text))
	, m_FontSize(FontSize)
	, m_Font(std::move(Font))
{
}
