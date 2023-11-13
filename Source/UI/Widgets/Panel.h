#pragma once

#include <memory>

#include "UI/Brush.h"
#include "UI/Widget.h"

class Panel : public Widget
{
public:
	static std::unique_ptr<Panel> Create(std::optional<std::unique_ptr<Brush>> Brush, std::optional<std::unique_ptr<Widget>> Content = std::nullopt);

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Layout() override;

	virtual void Render(RenderBuffer& Buffer) const override;

	virtual void ForEachChild(const ForEachChildCallbackType& Callback) override;
	virtual void ForEachChild(const ForEachChildConstCallbackType& Callback) const override;

protected:
	Panel(std::optional<std::unique_ptr<Brush>> Brush, std::optional<std::unique_ptr<Widget>> Content);

private:
	std::unique_ptr<Brush> m_Brush;
	std::unique_ptr<Widget> m_Content;
};
