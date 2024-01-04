#pragma once

#include "Core/Rect2D.h"
#include "Renderer/Renderer.h"
#include "UI/Font.h"

class RenderBuffer
{
public:
	explicit RenderBuffer(Renderer& Renderer);

	void Line(glm::vec2 From, glm::vec2 To, float Thickness, glm::vec4 Color);

	void Rect(Rect2D Rect, glm::vec4 Color, float CornerRadius, glm::vec4 BorderColor, float BorderThickness);

	void TextureRect(Rect2D Rect, const Texture& Texture);

	void Debug_RectOutline(Rect2D Rect, glm::vec3 Color);

	void Text(std::string_view Text, uint32_t FontSize, const Font& Font, Rect2D Rect);

private:
	Renderer& m_Renderer;

	std::unique_ptr<Shader> m_SolidColorShader;
	std::unique_ptr<Shader> m_RectShader;
	std::unique_ptr<Shader> m_TextureRectShader;
	std::unique_ptr<Shader> m_TextShader;
};
