#include <chrono>

#include "Core/Logger.h"
#include "Renderer/Renderer.h"
#include "Renderer/Window.h"

static constexpr float DefaultPixelsPerMeter = 64.0f;

int main()
{
	GLogger = std::make_unique<Logger>(LogLevel::Info, "Files/log.txt", true);

	auto AppWindow = Window::Create(1280, 720, "Build & Dispatch");
	auto Renderer = Renderer::Create(*AppWindow);

	glm::vec2 CameraLocation = {};
	float CameraScale = 1.0f;

	while (!AppWindow->ShouldClose())
	{
		AppWindow->PollEvents();

		if (AppWindow->IsMouseButtonPressed(MouseButton::Left))
			CameraLocation += AppWindow->GetMouseDelta() * -glm::vec2(1.0f, -1.0f) / (CameraScale * DefaultPixelsPerMeter);

		CameraScale = std::min(std::max(0.2f, CameraScale + AppWindow->GetMouseWheelDelta() * 0.2f), 4.0f);

		Renderer->BeginFrame(CameraLocation, CameraScale * DefaultPixelsPerMeter);

		for (int Y = -5; Y <= 5; Y++)
		{
			Renderer->Debug_PushLine({ -5, Y }, { 5, Y }, { 1.0f, 1.0f, 0.5f });
		}

		for (int X = -5; X <= 5; X++)
		{
			Renderer->Debug_PushLine({ X, -5 }, { X, 5 }, { 1.0f, 1.0f, 0.5f });
		}

		Renderer->EndFrame();
	}

	return 0;
}
