#pragma once

#include "UI/Widget.h"

class Label : public Widget
{
public:
	static std::unique_ptr<Label> Create(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font);

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Render(RenderBuffer& Buffer) const override;

	const std::string& Text() const { return m_Text; }
	std::string& Text() { return m_Text; }

private:
	std::string m_Text;

	uint32_t m_FontSize;
	std::shared_ptr<Font> m_Font;

	Label(std::string Text, uint32_t FontSize, std::shared_ptr<Font> Font);
};
