#include "Renderer.h"

#include <glad/glad.h>

#include "Core/Logger.h"

std::unique_ptr<Renderer> Renderer::Create(Window& Window)
{
	Window.MakeGLContextCurrent();

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		BD_LOG_ERROR("Could not load GLAD loader");
		return nullptr;
	}

	if (!gladLoadGL())
	{
		BD_LOG_ERROR("Could not load GL functions");
		return nullptr;
	}

	auto DebugLineGeometryBuffer = GeometryBuffer::Create(s_MaxDebugLineCount * 2, true);
	if (!DebugLineGeometryBuffer)
		return nullptr;

	auto DebugLineShader = Shader::Create("Resources/Shaders/DebugLine.vert", "Resources/Shaders/DebugLine.frag");
	if (!DebugLineShader)
		return nullptr;

	return std::unique_ptr<Renderer>(new Renderer(Window, std::move(DebugLineGeometryBuffer), std::move(DebugLineShader)));
}

void Renderer::BeginFrame()
{
	glViewport(0, 0, m_Window.Width(), m_Window.Height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	m_ViewProjectionMatrix = glm::mat4(1.0f);

	m_DebugLineGeometryBuffer->Reset();
}

void Renderer::EndFrame()
{
	m_DebugLineGeometryBuffer->Flush();

	m_DebugLineShader->Bind();
	m_DebugLineShader->SetUniform("u_ViewMatrix", m_ViewProjectionMatrix);

	m_DebugLineGeometryBuffer->Bind();
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_DebugLineGeometryBuffer->VertexCount()));

	m_Window.SwapBuffers();
}

void Renderer::SetViewProjectionMatrix(const glm::mat4& Matrix)
{
	m_ViewProjectionMatrix = Matrix;
}

void Renderer::Debug_PushLine(glm::vec2 From, glm::vec2 To, glm::vec3 Color)
{
	m_DebugLineGeometryBuffer->AppendVertex({ .Position = From, .Color = Color });
	m_DebugLineGeometryBuffer->AppendVertex({ .Position = To, .Color = Color });
}

glm::vec2 Renderer::FramebufferSize() const
{
	return glm::vec2(m_Window.Width(), m_Window.Height());
}

Renderer::Renderer(Window& Window, std::unique_ptr<GeometryBuffer> DebugLineGeometryBuffer, std::unique_ptr<Shader> DebugLineShader)
	: m_Window(Window)
	, m_DebugLineGeometryBuffer(std::move(DebugLineGeometryBuffer))
	, m_DebugLineShader(std::move(DebugLineShader))
{
}
