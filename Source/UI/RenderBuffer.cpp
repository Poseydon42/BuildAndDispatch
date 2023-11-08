#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(Renderer& Renderer)
	: m_Renderer(Renderer)
{
}

void RenderBuffer::Rect(Rect2D Rect, const Brush& Brush)
{
	std::vector<Vertex> Vertices =
	{
		{ .Position = glm::vec2(Rect.Left() , Rect.Top()   ) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Right(), Rect.Top()   ) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Left() , Rect.Bottom()) / m_Renderer.FramebufferSize(), .Color = {} },

		{ .Position = glm::vec2(Rect.Left() , Rect.Bottom()) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Right(), Rect.Top()   ) / m_Renderer.FramebufferSize(), .Color = {} },
		{ .Position = glm::vec2(Rect.Right(), Rect.Bottom()) / m_Renderer.FramebufferSize(), .Color = {} },
	};
	auto GeometryBuffer = GeometryBuffer::Create(Vertices.size(), false, Vertices);

	Brush.Prepare();
	m_Renderer.DrawWithShader(*GeometryBuffer, Brush.GetShader());
}
