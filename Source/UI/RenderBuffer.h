#pragma once

#include "Core/Rect2D.h"
#include "Renderer/Renderer.h"
#include "UI/Brush.h"

class RenderBuffer
{
public:
	explicit RenderBuffer(Renderer& Renderer);

	void Rect(Rect2D Rect, const Brush& Brush);

private:
	Renderer& m_Renderer;
};
