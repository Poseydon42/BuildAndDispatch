#pragma once

#define GLFW_INCLUDE_NONE

#include <functional>
#include <GLFW/glfw3.h>
#include <memory>
#include <string_view>
#include <glm/vec2.hpp>

#include "Core/InputState.h"

class Window
{
public:
	~Window();

	static std::unique_ptr<Window> Create(unsigned Width, unsigned Height, std::string_view Name, unsigned Samples);

	uint32_t Width() const;
	uint32_t Height() const;

	void PollEvents();

	void SwapBuffers();

	void MakeGLContextCurrent() const;

	bool ShouldClose() const;

	using MouseButtonCallbackType = void(MouseButton::Button, ButtonEventType::Type, int32_t, int32_t);
	void AddMouseButtonCallback(std::function<MouseButtonCallbackType>&& Callback);

	bool IsMouseButtonPressed(MouseButton::Button Button) const;

	using MouseScrollCallbackType = void(int32_t);
	void AddMouseScrollCallback(std::function<MouseScrollCallbackType>&& Callback);

	glm::ivec2 GetCursorPosition() const;
	glm::ivec2 GetCursorDelta() const;

	int32_t GetMouseWheelDelta() const;

private:
	GLFWwindow* m_Window = nullptr;

	glm::ivec2 m_CurrentCursorPosition = {};
	glm::ivec2 m_DeltaCursorPosition = {};
	int32_t m_MouseWheelDelta = 0;

	std::vector<std::function<MouseButtonCallbackType>> m_MouseButtonCallbacks;
	std::vector<std::function<MouseScrollCallbackType>> m_MouseScrollCallbacks;

	static constexpr int s_GLVersionMajor = 4;
	static constexpr int s_GLVersionMinor = 2;

	static bool s_IsGLFWInitialized;
	static int s_InstanceCount;

	explicit Window(GLFWwindow* Window);

	void MouseButtonCallback(int GLFWButton, int GLFWAction);

	void ScrollCallback(double YOffset);
};
