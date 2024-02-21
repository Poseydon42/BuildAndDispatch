#include "Renderer.h"

#include <glad/glad.h>

#include "Core/Logger.h"

VERTEX_DESCRIPTION_BEGIN(LineVertex)
	VERTEX_DESCRIPTION_ELEMENT(Position)
	VERTEX_DESCRIPTION_ELEMENT(Color)
VERTEX_DESCRIPTION_END()

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

	auto VectorIconShader = Shader::Create("Resources/Shaders/VectorIcon.vert", "Resources/Shaders/VectorIcon.frag");
	if (!VectorIconShader)
		return nullptr;

	auto LineGeometryBuffer = GeometryBuffer<LineVertex>::Create(s_MaxLineCount * 2, true);
	if (!LineGeometryBuffer)
		return nullptr;

	auto LineShader = Shader::Create("Resources/Shaders/Line.vert", "Resources/Shaders/Line.frag");
	if (!LineShader)
		return nullptr;

	return std::unique_ptr<Renderer>(new Renderer(Window, std::move(VectorIconShader), std::move(LineGeometryBuffer), std::move(LineShader)));
}

void Renderer::BeginFrame()
{
	glViewport(0, 0, m_Window.Width(), m_Window.Height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	m_ViewProjectionMatrix = glm::mat4(1.0f);

	m_LineGeometryBuffer->Reset();
}

void Renderer::Flush()
{
	m_LineGeometryBuffer->Flush();

	m_LineShader->Bind();
	m_LineShader->SetUniform("u_ViewMatrix", m_ViewProjectionMatrix);

	m_LineGeometryBuffer->Bind();
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_LineGeometryBuffer->VertexCount()));

	m_LineGeometryBuffer->Reset();
}

void Renderer::EndFrame()
{
	Flush();

	m_Window.SwapBuffers();
}

void Renderer::SetViewProjectionMatrix(const glm::mat4& Matrix)
{
	m_ViewProjectionMatrix = Matrix;
}

void Renderer::Draw(const VectorIcon& Icon, const glm::mat4& TransformationMatrix)
{
	auto MVP = m_ViewProjectionMatrix * TransformationMatrix;

	m_VectorIconShader->Bind();
	m_VectorIconShader->SetUniform("u_MVP", MVP);

	Icon.GeometryBuffer().Bind();
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(Icon.GeometryBuffer().VertexCount()));
}

void Renderer::DrawLine(glm::vec2 From, glm::vec2 To, glm::vec3 Color)
{
	m_LineGeometryBuffer->AppendVertex({ .Position = From, .Color = Color });
	m_LineGeometryBuffer->AppendVertex({ .Position = To, .Color = Color });
}

glm::vec2 Renderer::FramebufferSize() const
{
	return { m_Window.Width(), m_Window.Height() };
}

Renderer::Renderer(Window& Window, std::unique_ptr<Shader> VectorIconShader, std::unique_ptr<GeometryBuffer<LineVertex>> LineGeometryBuffer, std::unique_ptr<Shader> LineShader)
	: m_Window(Window)
	, m_VectorIconShader(std::move(VectorIconShader))
	, m_LineGeometryBuffer(std::move(LineGeometryBuffer))
	, m_LineShader(std::move(LineShader))
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
