#pragma once

#include "UI/Widget.h"

class Button : public Widget
{
public:
	using ButtonCallbackType = std::function<void(bool IsPress)>;

	static std::unique_ptr<Button> Create(std::unique_ptr<Widget> Label, ButtonCallbackType Callback = {});

	virtual void Layout() override;

	virtual void Render(RenderBuffer& Buffer) const override;

	virtual bool OnMouseDown(MouseButton::Button Button) override;
	virtual bool OnMouseUp(MouseButton::Button Button) override;

	virtual void ForEachChild(const ForEachChildCallbackType& Callback) override;
	virtual void ForEachChild(const ForEachChildConstCallbackType& Callback) const override;

private:
	Button(std::unique_ptr<Widget> Label, ButtonCallbackType Callback);

	std::unique_ptr<Widget> m_Label;
	ButtonCallbackType m_Callback;

	virtual glm::vec2 ComputeContentPreferredSize() const override;
};
