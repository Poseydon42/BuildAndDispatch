#pragma once

#include "Core/Rect2D.h"
#include "UI/RenderBuffer.h"

class Widget
{
public:
	virtual ~Widget() = default;

	virtual glm::vec2 ComputePreferredSize() const { return {}; }

	virtual void Layout() {}

	virtual void Render(RenderBuffer& Buffer) const {}

	Rect2D& BoundingBox() { return m_BoundingBox; }
	const Rect2D& BoundingBox() const { return m_BoundingBox; }

protected:
	Widget() = default;

private:
	Rect2D m_BoundingBox;
};
