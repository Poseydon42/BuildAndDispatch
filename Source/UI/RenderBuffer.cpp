#include "RenderBuffer.h"

#include "UI/TextLayout.h"

struct UIVertex
{
	glm::vec2 Position;
	glm::vec2 TextureCoordinates;
};

VERTEX_DESCRIPTION_BEGIN(UIVertex)
	VERTEX_DESCRIPTION_ELEMENT(Position)
	VERTEX_DESCRIPTION_ELEMENT(TextureCoordinates)
VERTEX_DESCRIPTION_END()

RenderBuffer::RenderBuffer(Renderer& Renderer)
	: m_Renderer(Renderer)
{
	m_RectShader = Shader::Create("Resources/Shaders/UIRect.vert", "Resources/Shaders/UIRect.frag");
	m_TextShader = Shader::Create("Resources/Shaders/TextMSDF.vert", "Resources/Shaders/TextMSDF.frag");
	BD_ASSERT(m_TextShader);
}

void RenderBuffer::Rect(Rect2D Rect, glm::vec4 Color, float CornerRadius, glm::vec4 BorderColor, float BorderThickness)
{
	std::vector<UIVertex> Vertices =
	{
		{ .Position = glm::vec2(Rect.Left() , Rect.Top()   ) / m_Renderer.FramebufferSize(), .TextureCoordinates = { 0.0f, 1.0f } },
		{ .Position = glm::vec2(Rect.Right(), Rect.Top()   ) / m_Renderer.FramebufferSize(), .TextureCoordinates = { 1.0f, 1.0f } },
		{ .Position = glm::vec2(Rect.Left() , Rect.Bottom()) / m_Renderer.FramebufferSize(), .TextureCoordinates = { 0.0f, 0.0f } },

		{ .Position = glm::vec2(Rect.Left() , Rect.Bottom()) / m_Renderer.FramebufferSize(), .TextureCoordinates = { 0.0f, 0.0f } },
		{ .Position = glm::vec2(Rect.Right(), Rect.Top()   ) / m_Renderer.FramebufferSize(), .TextureCoordinates = { 1.0f, 1.0f } },
		{ .Position = glm::vec2(Rect.Right(), Rect.Bottom()) / m_Renderer.FramebufferSize(), .TextureCoordinates = { 1.0f, 0.0f } },
	};
	auto Geometry = GeometryBuffer<UIVertex>::Create(Vertices.size(), false, Vertices);

	m_RectShader->SetUniform("u_Color", Color);
	m_RectShader->SetUniform("u_BorderColor", BorderColor);
	m_RectShader->SetUniform("u_FramebufferDimensions", m_Renderer.FramebufferSize());
	m_RectShader->SetUniform("u_RectDimensions", Rect.Dimensions());
	m_RectShader->SetUniform("u_CornerRadius", CornerRadius);
	m_RectShader->SetUniform("u_BorderThickness", BorderThickness);

	m_Renderer.DrawWithShader(*Geometry, *m_RectShader);
}

void RenderBuffer::Debug_RectOutline(Rect2D Rect, glm::vec3 Color)
{
	auto TopLeft = glm::vec2(Rect.Left(), Rect.Top()) / m_Renderer.FramebufferSize() * 2.0f - 1.0f;
	auto TopRight = glm::vec2(Rect.Right(), Rect.Top()) / m_Renderer.FramebufferSize() * 2.0f - 1.0f;
	auto BottomRight = glm::vec2(Rect.Right(), Rect.Bottom()) / m_Renderer.FramebufferSize() * 2.0f - 1.0f;
	auto BottomLeft = glm::vec2(Rect.Left(), Rect.Bottom()) / m_Renderer.FramebufferSize() * 2.0f - 1.0f;

	m_Renderer.Debug_PushLine(TopLeft, TopRight, Color);
	m_Renderer.Debug_PushLine(TopRight, BottomRight, Color);
	m_Renderer.Debug_PushLine(BottomRight, BottomLeft, Color);
	m_Renderer.Debug_PushLine(BottomLeft, TopLeft, Color);
}

struct TextVertex
{
	glm::vec2 Position;
	glm::vec2 TextureCoordinates;
};

VERTEX_DESCRIPTION_BEGIN(TextVertex)
	VERTEX_DESCRIPTION_ELEMENT(Position)
	VERTEX_DESCRIPTION_ELEMENT(TextureCoordinates)
VERTEX_DESCRIPTION_END()

void RenderBuffer::Text(std::string_view Text, uint32_t FontSize, const Font& Font, Rect2D Rect)
{
	std::vector<TextVertex> Vertices;

	BD_ASSERT(TextLayout::Layout(Text, FontSize, Font, [&](Rect2D ScreenRect, Rect2D TextureRect)
	{
		TextVertex TopLeft = {
			2.0f * (Rect.Min + ScreenRect.Min + glm::vec2(0.0f, ScreenRect.Height())) / m_Renderer.FramebufferSize() - 1.0f,
			TextureRect.Min + glm::vec2(0.0f, TextureRect.Height())
		};
		TextVertex TopRight = {
			2.0f * (Rect.Min + ScreenRect.Max) / m_Renderer.FramebufferSize() - 1.0f,
			TextureRect.Max
		};
		TextVertex BottomLeft = {
			2.0f * (Rect.Min + ScreenRect.Min) / m_Renderer.FramebufferSize() - 1.0f,
			TextureRect.Min
		};
		TextVertex BottomRight = {
			2.0f * (Rect.Min + ScreenRect.Min + glm::vec2(ScreenRect.Width(), 0.0f)) / m_Renderer.FramebufferSize() - 1.0f,
			TextureRect.Min + glm::vec2(TextureRect.Width(), 0.0f)
		};

		Vertices.push_back(TopLeft);
		Vertices.push_back(TopRight);
		Vertices.push_back(BottomRight);

		Vertices.push_back(TopLeft);
		Vertices.push_back(BottomRight);
		Vertices.push_back(BottomLeft);
	}));

	auto Geometry = GeometryBuffer<TextVertex>::Create(Vertices.size(), false, Vertices);

	m_TextShader->Bind();
	m_TextShader->SetUniform("u_Atlas", Font.AtlasTexture());
	m_TextShader->SetUniform("u_Color", glm::vec3(1.0f, 1.0f, 1.0f));
	m_TextShader->SetUniform("u_ScreenPxRange", Font.ScreenPxRange(FontSize));

	m_Renderer.DrawWithShader(*Geometry, *m_TextShader);
}
