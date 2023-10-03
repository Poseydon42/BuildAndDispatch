#include "Renderer.h"

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

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

	auto DebugLineShader = Shader::Create("Resources/Shaders/DebugLine.vert", "Resources/Shaders/DebugLine.frag");

	return std::unique_ptr<Renderer>(new Renderer(Window, std::move(DebugLineShader)));
}

void Renderer::BeginFrame(glm::vec2 CameraLocation, float PixelsPerMeter)
{
	glViewport(0, 0, m_Window.Width(), m_Window.Height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto ViewMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::scale(ViewMatrix, glm::vec3(PixelsPerMeter));
	ViewMatrix = glm::translate(ViewMatrix, -glm::vec3(CameraLocation, 0.0f));

	auto ProjectionMatrix = glm::mat4();
	ProjectionMatrix[0][0] = 2.0f / m_Window.Width();
	ProjectionMatrix[1][1] = 2.0f / m_Window.Height();
	ProjectionMatrix[3][3] = 1.0f;

	m_ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	m_NextDebugLinePtr = static_cast<DebugLineVertex*>(m_DebugLineVertexBuffer->Map(false, true));
	m_DebugLineCount = 0;
}

void Renderer::EndFrame()
{
	m_DebugLineVertexBuffer->Unmap();

	m_DebugLineShader->Bind();
	m_DebugLineShader->SetUniform("u_ViewMatrix", m_ViewProjectionMatrix);
	glBindVertexArray(m_DebugLineVAO);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_DebugLineCount * 2));

	m_Window.SwapBuffers();
}

void Renderer::Debug_PushLine(glm::vec2 From, glm::vec2 To, glm::vec3 Color)
{
	if (m_DebugLineCount >= s_MaxDebugLineCount)
		return;

	*m_NextDebugLinePtr++ = { .Position = From, .Color = Color };
	*m_NextDebugLinePtr++ = { .Position = To, .Color = Color };

	m_DebugLineCount++;
}

Renderer::Renderer(Window& Window, std::unique_ptr<Shader> DebugLineShader)
	: m_Window(Window)
	, m_DebugLineVertexBuffer(Buffer::Create(s_DebugLineBufferSize, {}, GL_DYNAMIC_DRAW))
	, m_DebugLineShader(std::move(DebugLineShader))
{
	glGenVertexArrays(1, &m_DebugLineVAO);
	glBindVertexArray(m_DebugLineVAO);

	m_DebugLineVertexBuffer->Bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(DebugLineVertex), std::bit_cast<void*>(offsetof(DebugLineVertex, Position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(DebugLineVertex), std::bit_cast<void*>(offsetof(DebugLineVertex, Color)));
	glEnableVertexAttribArray(1);
}
