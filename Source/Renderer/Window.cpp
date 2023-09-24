#include "Window.h"

#include "Core/Assert.h"

static int MouseButtonToGLFWCode(MouseButton Button)
{
	switch (Button)
	{
	case MouseButton::Left:
		return GLFW_MOUSE_BUTTON_LEFT;
	case MouseButton::Right:
		return GLFW_MOUSE_BUTTON_RIGHT;
	case MouseButton::Middle:
		return GLFW_MOUSE_BUTTON_MIDDLE;
	default:
		BD_UNREACHABLE();
	}
}

static void GLFWErrorCallback(int Error, const char* Description)
{
	BD_LOG_ERROR("GLFW error (code {}): {}", Error, Description);
}

bool Window::s_IsGLFWInitialized = false;
int Window::s_InstanceCount = 0;

Window::~Window()
{
	glfwDestroyWindow(m_Window);

	BD_ASSERT(s_InstanceCount > 0);
	s_InstanceCount--;

	if (s_InstanceCount == 0)
	{
		glfwTerminate();
		s_IsGLFWInitialized = false;
	}
}

std::unique_ptr<Window> Window::Create(unsigned Width, unsigned Height, std::string_view Name)
{
	if (!s_IsGLFWInitialized)
	{
		glfwSetErrorCallback(GLFWErrorCallback);

		BD_ASSERT(glfwInit());
	}

	s_IsGLFWInitialized = true;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, s_GLVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, s_GLVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto* GLFWWindow = glfwCreateWindow(Width, Height, Name.data(), nullptr, nullptr);
	if (!GLFWWindow)
		return nullptr;

	s_InstanceCount++;

	return std::unique_ptr<Window>(new Window(GLFWWindow));
}

uint32_t Window::Width() const
{
	int Width, Dummy;
	glfwGetFramebufferSize(m_Window, &Width, &Dummy);
	return Width;
}

uint32_t Window::Height() const
{
	int Dummy, Height;
	glfwGetFramebufferSize(m_Window, &Dummy, &Height);
	return Height;
}

void Window::PollEvents()
{
	m_MouseWheelDelta = 0;

	glfwPollEvents();

	double MouseX, MouseY;
	glfwGetCursorPos(m_Window, &MouseX, &MouseY);

	auto NewMousePosition = glm::vec2(static_cast<float>(MouseX), static_cast<float>(MouseY));
	m_DeltaMousePosition = NewMousePosition - m_CurrentMousePosition;
	m_CurrentMousePosition = NewMousePosition;
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(m_Window);
}

void Window::MakeGLContextCurrent() const
{
	glfwMakeContextCurrent(m_Window);
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_Window);
}

bool Window::IsMouseButtonPressed(MouseButton Button) const
{
	return glfwGetMouseButton(m_Window, MouseButtonToGLFWCode(Button)) == GLFW_PRESS;
}

glm::vec2 Window::GetMouseDelta() const
{
	return m_DeltaMousePosition;
}

int32_t Window::GetMouseWheelDelta() const
{
	return m_MouseWheelDelta;
}

Window::Window(GLFWwindow* Window)
	: m_Window(Window)
{
	glfwSetWindowUserPointer(m_Window, this);

	glfwSetScrollCallback(m_Window, [](GLFWwindow* GLFWWindow, double, double YOffset)
	{
		static_cast<class Window*>(glfwGetWindowUserPointer(GLFWWindow))->ScrollCallback(YOffset);
	});
}

void Window::ScrollCallback(double YOffset)
{
	m_MouseWheelDelta += static_cast<int32_t>(YOffset);
}
