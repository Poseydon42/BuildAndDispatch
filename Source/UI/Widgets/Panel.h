#pragma once

#include <memory>

#include "UI/Widget.h"

class Panel : public Widget
{
public:
	static std::unique_ptr<Panel> Create(std::optional<std::unique_ptr<Widget>> Content = std::nullopt);

	virtual void Layout() override;

	virtual void Render(RenderBuffer& Buffer) const override;

	virtual void ForEachChild(const ForEachChildCallbackType& Callback) override;
	virtual void ForEachChild(const ForEachChildConstCallbackType& Callback) const override;

private:
	explicit Panel(std::optional<std::unique_ptr<Widget>> Content);

	std::unique_ptr<Widget> m_Content;

	virtual glm::vec2 ComputeContentPreferredSize() const override;
};
