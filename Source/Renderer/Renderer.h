#pragma once

#include <glm/glm.hpp>

#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Window.h"

class Renderer
{
public:
	static std::unique_ptr<Renderer> Create(Window& Window);
	
	void BeginFrame(glm::vec2 CameraLocation, float PixelsPerMeter);

	void EndFrame();

	void Debug_PushLine(glm::vec2 From, glm::vec2 To, glm::vec3 Color);

private:
	Window& m_Window;

	glm::mat4 m_ViewProjectionMatrix = {};

	/**************************************
	 ********* DEBUG LINE DRAWING *********
	 **************************************/
	struct DebugLineVertex
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};

	static constexpr size_t s_MaxDebugLineCount = 4096;
	static constexpr size_t s_DebugLineBufferSize = s_MaxDebugLineCount * 2 * sizeof(DebugLineVertex);

	std::unique_ptr<Buffer> m_DebugLineVertexBuffer;
	GLuint m_DebugLineVAO = -1;
	DebugLineVertex* m_NextDebugLinePtr = nullptr;
	size_t m_DebugLineCount = 0;
	std::unique_ptr<Shader> m_DebugLineShader;

	Renderer(Window& Window, std::unique_ptr<Shader> DebugLineShader);
};
