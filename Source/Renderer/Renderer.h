#pragma once

#include <glm/glm.hpp>

#include "Renderer/GeometryBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VectorIcon.h"
#include "Renderer/Window.h"

struct DebugLineVertex
{
	glm::vec2 Position;
	glm::vec3 Color;
};

class Renderer
{
public:
	static std::unique_ptr<Renderer> Create(Window& Window);
	
	void BeginFrame();

	void EndFrame();

	void SetViewProjectionMatrix(const glm::mat4& Matrix);

	void Draw(const VectorIcon& Icon, const glm::mat4& TransformationMatrix);

	template<typename VertexType>
	void DrawWithShader(const GeometryBuffer<VertexType>& Buffer, const Shader& Shader);

	void Debug_PushLine(glm::vec2 From, glm::vec2 To, glm::vec3 Color);

	glm::vec2 FramebufferSize() const;

private:
	Window& m_Window;

	glm::mat4 m_ViewProjectionMatrix = {};

	/***************************************
	 ********* VECTOR ICON DRAWING *********
	 ***************************************/
	std::unique_ptr<Shader> m_VectorIconShader;

	/**************************************
	 ********* DEBUG LINE DRAWING *********
	 **************************************/
	static constexpr size_t s_MaxDebugLineCount = 4096;

	std::unique_ptr<GeometryBuffer<DebugLineVertex>> m_DebugLineGeometryBuffer;
	std::unique_ptr<Shader> m_DebugLineShader;

	Renderer(Window& Window, std::unique_ptr<Shader> VectorIconShader, std::unique_ptr<GeometryBuffer<DebugLineVertex>> DebugLineGeometryBuffer, std::unique_ptr<Shader> DebugLineShader);
};

template<typename VertexType>
void Renderer::DrawWithShader(const GeometryBuffer<VertexType>& Buffer, const Shader& Shader)
{
	Shader.Bind();

	Buffer.Bind();
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(Buffer.VertexCount()));
}
	
