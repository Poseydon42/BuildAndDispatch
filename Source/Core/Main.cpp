#include <chrono>

#include "Core/Logger.h"
#include "Renderer/Renderer.h"
#include "Renderer/Window.h"
#include "Simulation/World.h"
#include "View/TrackView.h"

static constexpr uint32_t WindowWidth = 1280;
static constexpr uint32_t WindowHeight = 720;
static constexpr float DefaultPixelsPerMeter = 64.0f;

static glm::ivec2 CursorPositionToTile(int32_t CursorX, int32_t CursorY, glm::vec2 CameraLocation, float PixelsPerMeter)
{
	float NormalizedX = (static_cast<float>(CursorX) / WindowWidth) * 2.0f - 1.0f;
	float NormalizedY = -((static_cast<float>(CursorY) / WindowHeight) * 2.0f - 1.0f);

	auto ScaledX = NormalizedX * (0.5f * WindowWidth) / PixelsPerMeter + CameraLocation.x;
	auto ScaledY = NormalizedY * (0.5f * WindowHeight) / PixelsPerMeter + CameraLocation.y;

	auto TileX = static_cast<int>(std::lround(ScaledX));
	auto TileY = static_cast<int>(std::lround(ScaledY));

	return { TileX, TileY };
}

int main()
{
	GLogger = std::make_unique<Logger>(LogLevel::Info, "Files/log.txt", true);

	auto AppWindow = Window::Create(WindowWidth, WindowHeight, "Build & Dispatch");
	auto Renderer = Renderer::Create(*AppWindow);

	glm::vec2 CameraLocation = {};
	float CameraScale = 1.0f;

	World World;
	World.AddTrack(-5, 0, -4, 0);
	World.AddTrack(-4, 0, -3, 0);
	World.AddTrack(-3, 0, -2, 0);
	World.AddTrack(-2, 0, -1, 0);
	World.AddTrack(-1, 0, 0, 0);
	World.AddTrack(0, 0, 1, 0);
	World.AddTrack(1, 0, 2, 0);
	World.AddTrack(2, 0, 3, 0);
	World.AddTrack(3, 0, 4, 0);
	World.AddTrack(4, 0, 5, 0);
	World.AddTrack(5, 0, 6, 0);

	World.AddTrack(-3, 1, -2, 1);
	World.AddTrack(-2, 1, -1, 1);
	World.AddTrack(-1, 1, 0, 1);
	World.AddTrack(0, 1, 1, 1);
	World.AddTrack(1, 1, 2, 1);
	World.AddTrack(2, 1, 3, 1);
	World.AddTrack(3, 1, 4, 1);
	World.AddTrack(4, 1, 5, 1);

	World.AddTrack(-2, -1, -1, -1);
	World.AddTrack(-1, -1, 0,-1);
	World.AddTrack(0, -1, 1, -1);
	World.AddTrack(1, -1, 2, -1);
	World.AddTrack(2, -1, 3, -1);
	World.AddTrack(3, -1, 4, -1);

	World.AddTrack(-4, 0, -3, 1);
	World.AddTrack(-3, 0, -2, -1);
	World.AddTrack(3, 1, 4, 0);
	World.AddTrack(4, -1, 5, 0);

	AppWindow->AddMouseButtonCallback([&](MouseButton Button, ButtonEventType Type, int32_t CursorX, int32_t CursorY)
	{
		if (Button != MouseButton::Left || Type != ButtonEventType::Release)
			return;

		auto Tile = CursorPositionToTile(CursorX, CursorY, CameraLocation, CameraScale * DefaultPixelsPerMeter);
		
		World.SwitchPoint(Tile.x, Tile.y);
	});

	TrackView TrackView;

	while (!AppWindow->ShouldClose())
	{
		AppWindow->PollEvents();

		if (AppWindow->IsMouseButtonPressed(MouseButton::Left))
			CameraLocation += glm::vec2(AppWindow->GetCursorDelta()) * -glm::vec2(1.0f, -1.0f) / (CameraScale * DefaultPixelsPerMeter);

		CameraScale = std::min(std::max(0.2f, CameraScale + AppWindow->GetMouseWheelDelta() * 0.2f), 4.0f);

		Renderer->BeginFrame(CameraLocation, CameraScale * DefaultPixelsPerMeter);

		for (int Y = -5; Y <= 6; Y++)
		{
			Renderer->Debug_PushLine({ -5.5f, Y - 0.5f }, { 5.5f, Y - 0.5f }, { 1.0f, 1.0f, 0.5f });
		}

		for (int X = -5; X <= 6; X++)
		{
			Renderer->Debug_PushLine({ X - 0.5f, -5.5f }, { X - 0.5f, 5.5f }, { 1.0f, 1.0f, 0.5f });
		}

		TrackView.Render(*Renderer, World);

		auto TileUnderCursor = CursorPositionToTile(AppWindow->GetCursorPosition().x, AppWindow->GetCursorPosition().y, CameraLocation, CameraScale * DefaultPixelsPerMeter);
		Renderer->Debug_PushLine(glm::vec2(TileUnderCursor) + glm::vec2(-0.08f, -0.08f), glm::vec2(TileUnderCursor) + glm::vec2(-0.08f, 0.08f), glm::vec3(0.0f, 0.0f, 1.0f));
		Renderer->Debug_PushLine(glm::vec2(TileUnderCursor) + glm::vec2(-0.08f, 0.08f), glm::vec2(TileUnderCursor) + glm::vec2(0.08f, 0.08f), glm::vec3(0.0f, 0.0f, 1.0f));
		Renderer->Debug_PushLine(glm::vec2(TileUnderCursor) + glm::vec2(0.08f, 0.08f), glm::vec2(TileUnderCursor) + glm::vec2(0.08f, -0.08f), glm::vec3(0.0f, 0.0f, 1.0f));
		Renderer->Debug_PushLine(glm::vec2(TileUnderCursor) + glm::vec2(0.08f, -0.08f), glm::vec2(TileUnderCursor) + glm::vec2(-0.08f, -0.08f), glm::vec3(0.0f, 0.0f, 1.0f));

		Renderer->EndFrame();
	}

	return 0;
}
