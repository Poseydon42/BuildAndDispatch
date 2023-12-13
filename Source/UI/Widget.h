#pragma once

#include "Core/Rect2D.h"
#include "UI/RenderBuffer.h"
#include "UI/Size.h"

class Widget
{
public:
	virtual ~Widget() = default;

	static std::unique_ptr<Widget> Create();

	virtual glm::vec2 ComputePreferredSize() const { return {}; }

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

	Size1D& LeftMargin() { return m_LeftMargin; }
	const Size1D& LeftMargin() const { return m_LeftMargin; }
	Size1D& RightMargin() { return m_RightMargin; }
	const Size1D& RightMargin() const { return m_RightMargin; }
	Size1D& TopMargin() { return m_TopMargin; }
	const Size1D& TopMargin() const { return m_TopMargin; }
	Size1D& BottomMargin() { return m_BottomMargin; }
	const Size1D& BottomMargin() const { return m_BottomMargin; }

	float& VerticalStretchRatio() { return m_VerticalStretchRatio; }
	const float& VerticalStretchRatio() const { return m_VerticalStretchRatio; }
	float& HorizontalStretchRatio() { return m_HorizontalStretchRatio; }
	const float& HorizontalStretchRatio() const { return m_HorizontalStretchRatio; }

	float& BorderThickness() { return m_BorderThickness; }
	const float& BorderThickness() const { return m_BorderThickness; }
	float& CornerRadius() { return m_CornerRadius; }
	const float& CornerRadius() const { return m_CornerRadius; }

	glm::vec4& BackgroundColor() { return m_BackgroundColor; }
	const glm::vec4& BackgroundColor() const { return m_BackgroundColor; }
	glm::vec4& BorderColor() { return m_BorderColor; }
	const glm::vec4& BorderColor() const { return m_BorderColor; }

protected:
	Widget() = default;

private:
	Widget* m_Parent = nullptr;

	Rect2D m_BoundingBox;

	Size1D m_LeftMargin;
	Size1D m_RightMargin;
	Size1D m_TopMargin;
	Size1D m_BottomMargin;

	float m_VerticalStretchRatio = 0.0f;
	float m_HorizontalStretchRatio = 0.0f;

	float m_BorderThickness = 0.0f;
	float m_CornerRadius = 0.0f;

	glm::vec4 m_BackgroundColor = glm::vec4(0.0f);
	glm::vec4 m_BorderColor = glm::vec4(1.0);
};
