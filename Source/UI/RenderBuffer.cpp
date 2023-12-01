#include "RenderBuffer.h"

struct UIVertex
{
	glm::vec2 Position;
	glm::vec4 Color;
};

VERTEX_DESCRIPTION_BEGIN(UIVertex)
	VERTEX_DESCRIPTION_ELEMENT(Position)
	VERTEX_DESCRIPTION_ELEMENT(Color)
VERTEX_DESCRIPTION_END()

RenderBuffer::RenderBuffer(Renderer& Renderer)
	: m_Renderer(Renderer)
{
}

void RenderBuffer::Rect(Rect2D Rect, const Brush& Brush)
{
	std::vector<UIVertex> Vertices =
	{
		{ .Position = glm::vec2(Rect.Left() , Rect.Top()   ) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Right(), Rect.Top()   ) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Left() , Rect.Bottom()) / m_Renderer.FramebufferSize(), .Color = {} },

		{ .Position = glm::vec2(Rect.Left() , Rect.Bottom()) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Right(), Rect.Top()   ) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Right(), Rect.Bottom()) / m_Renderer.FramebufferSize(), .Color = {} },
	};
	auto Geometry = GeometryBuffer<UIVertex>::Create(Vertices.size(), false, Vertices);

	Brush.Prepare();
	m_Renderer.DrawWithShader(*Geometry, Brush.GetShader());
}
