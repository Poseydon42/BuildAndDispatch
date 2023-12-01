#pragma once

#include "Core/Rect2D.h"
#include "Renderer/Renderer.h"
#include "UI/Brush.h"
#include "UI/Font.h"

class RenderBuffer
{
public:
	explicit RenderBuffer(Renderer& Renderer);

	void Rect(Rect2D Rect, const Brush& Brush);

	void Text(std::string_view Text, uint32_t FontSize, const Font& Font, Rect2D Rect);

private:
	Renderer& m_Renderer;

	std::unique_ptr<Shader> m_TextShader;
};
