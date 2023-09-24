#pragma once

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <memory>
#include <string_view>
#include <glm/vec2.hpp>

enum class MouseButton
{
	Left,
	Right,
	Middle,
};

class Window
{
public:
	~Window();

	static std::unique_ptr<Window> Create(unsigned Width, unsigned Height, std::string_view Name);

	uint32_t Width() const;
	uint32_t Height() const;

	void PollEvents();

	void SwapBuffers();

	void MakeGLContextCurrent() const;

	bool ShouldClose() const;

	bool IsMouseButtonPressed(MouseButton Button) const;

	glm::vec2 GetMouseDelta() const;

	int32_t GetMouseWheelDelta() const;

private:
	GLFWwindow* m_Window = nullptr;

	glm::vec2 m_CurrentMousePosition = {};
	glm::vec2 m_DeltaMousePosition = {};
	int32_t m_MouseWheelDelta = 0;

	static constexpr int s_GLVersionMajor = 4;
	static constexpr int s_GLVersionMinor = 2;

	static bool s_IsGLFWInitialized;
	static int s_InstanceCount;

	explicit Window(GLFWwindow* Window);

	void ScrollCallback(double YOffset);
};
