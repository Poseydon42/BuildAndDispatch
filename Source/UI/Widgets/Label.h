#pragma once

#include "UI/Widget.h"

enum class TextAlignment
{
	Begin,
	Center,
	End
};

class Label : public Widget
{
public:
	static std::unique_ptr<Label> Create(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font, TextAlignment HorizontalAlignment = TextAlignment::Center, TextAlignment VerticalAlignment = TextAlignment::Center);

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Render(RenderBuffer& Buffer) const override;

	const std::string& Text() const { return m_Text; }
	std::string& Text() { return m_Text; }

	TextAlignment& HorizontalTextAlignment() { return m_HorizontalTextAlignment; }
	TextAlignment HorizontalTextAlignment() const { return m_HorizontalTextAlignment; }

	TextAlignment& VerticalTextAlignment() { return m_VerticalTextAlignment; }
	TextAlignment VerticalTextAlignment() const { return m_VerticalTextAlignment; }

private:
	std::string m_Text;

	TextAlignment m_HorizontalTextAlignment;
	TextAlignment m_VerticalTextAlignment;

	uint32_t m_FontSize;
	std::shared_ptr<Font> m_Font;

	Label(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font, TextAlignment HorizontalAlignment, TextAlignment VerticalAlignment);
};
