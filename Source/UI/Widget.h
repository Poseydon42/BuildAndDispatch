#pragma once

#include "Core/Rect2D.h"
#include "UI/RenderBuffer.h"
#include "UI/Size.h"

struct WidgetStyle
{
	Size1D LeftMargin;
	Size1D RightMargin;
	Size1D TopMargin;
	Size1D BottomMargin;

	float VerticalStretchRatio = 0.0f;
	float HorizontalStretchRatio = 0.0f;

	float BorderThickness = 0.0f;
	float CornerRadius = 0.0f;

	float PaddingLeft = 0.0f;
	float PaddingRight = 0.0f;
	float PaddingTop = 0.0f;
	float PaddingBottom = 0.0f;

	glm::vec4 BackgroundColor = glm::vec4(0.0f);
	glm::vec4 BorderColor = glm::vec4(1.0);
};

class Widget
{
public:
	virtual ~Widget() = default;

	static std::unique_ptr<Widget> Create();

	glm::vec2 ComputePreferredSize() const;

	virtual void Layout() {}

	virtual void Render(RenderBuffer& Buffer) const;

	virtual bool OnMouseDown(MouseButton::Button Button) { return false; }
	virtual bool OnMouseUp(MouseButton::Button Button) { return false; }

	using ForEachChildConstCallbackType = std::function<void(const Widget&)>;
	using ForEachChildCallbackType = std::function<void(Widget&)>;

	virtual void ForEachChild(const ForEachChildConstCallbackType& Callback) const {}
	virtual void ForEachChild(const ForEachChildCallbackType& Callback) {}

	Widget* Parent() { return m_Parent; }
	const Widget* Parent() const { return m_Parent; }
	void SetParent(Widget* Parent) { m_Parent = Parent; }

	Rect2D& BoundingBox() { return m_BoundingBox; }
	const Rect2D& BoundingBox() const { return m_BoundingBox; }

	Rect2D ContentBoundingBox() const;

	WidgetStyle& Style() { return m_Style; }
	const WidgetStyle& Style() const { return m_Style; }

protected:
	Widget() = default;

private:
	Widget* m_Parent = nullptr;

	Rect2D m_BoundingBox;

	WidgetStyle m_Style;

	virtual glm::vec2 ComputeContentPreferredSize() const { return {}; }
};
