#include "GameLoop.h"

#include <algorithm>

#include "Core/Logger.h"
#include "Layer/TrackLayer.h"

static constexpr uint32_t WindowWidth = 1280;
static constexpr uint32_t WindowHeight = 720;
static constexpr const char* WindowName = "Build & Dispatch";

static constexpr float DefaultPixelsPerMeter = 64.0f;

static void GenerateDebugWorld(World& World)
{
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
	World.AddTrack(-1, -1, 0, -1);
	World.AddTrack(0, -1, 1, -1);
	World.AddTrack(1, -1, 2, -1);
	World.AddTrack(2, -1, 3, -1);
	World.AddTrack(3, -1, 4, -1);

	World.AddTrack(-4, 0, -3, 1);
	World.AddTrack(-3, 0, -2, -1);
	World.AddTrack(3, 1, 4, 0);
	World.AddTrack(4, -1, 5, 0);

	World.AddSignal(-3, 1, -4, 0);
	World.AddSignal(-2, -1, -3, 0);
	World.AddSignal(2, 1, 3, 1);
	World.AddSignal(4, -1, 5, 0);
}

std::unique_ptr<GameLoop> GameLoop::Create()
{
	auto Window = Window::Create(WindowWidth, WindowHeight, WindowName);
	if (!Window)
	{
		BD_LOG_ERROR("Could not create game window");
		return nullptr;
	}

	auto Renderer = Renderer::Create(*Window);
	if (!Renderer)
	{
		BD_LOG_ERROR("Could not initialize renderer");
		return nullptr;
	}

	return std::unique_ptr<GameLoop>(new GameLoop(std::move(Window), std::move(Renderer)));
}

int GameLoop::Run()
{
	while (!m_Window->ShouldClose())
	{
		m_Window->PollEvents();

		if (m_Window->IsMouseButtonPressed(MouseButton::Left))
			m_CameraLocation += glm::vec2(m_Window->GetCursorDelta()) * -glm::vec2(1.0f, -1.0f) / (m_CameraScale * DefaultPixelsPerMeter);
		m_CameraScale = std::min(std::max(0.2f, m_CameraScale + m_Window->GetMouseWheelDelta() * 0.2f), 4.0f);

		auto PixelsPerMeter = m_CameraScale * DefaultPixelsPerMeter;
		m_Renderer->BeginFrame(m_CameraLocation, PixelsPerMeter);

		for (const auto& Layer : m_Layers)
		{
			Layer->Render(*m_Renderer, m_World);
		}

		m_Renderer->EndFrame();
	}

	return 0;
}

GameLoop::GameLoop(std::unique_ptr<Window> Window, std::unique_ptr<Renderer> Renderer)
	: m_Window(std::move(Window))
	, m_Renderer(std::move(Renderer))
{
	m_Layers.push_back(std::make_unique<TrackLayer>());

	GenerateDebugWorld(m_World);

	m_Window->AddMouseButtonCallback([this](MouseButton Button, ButtonEventType Type, int32_t CursorX, int32_t CursorY)
	{
		std::for_each(m_Layers.rbegin(), m_Layers.rend(), [&](const auto& Layer)
		{
			if (Type == ButtonEventType::Press)
				Layer->OnMousePress(Button, CursorX, CursorY, m_World);
			else if (Type == ButtonEventType::Release)
				Layer->OnMouseRelease(Button, CursorX, CursorY, m_World);
		});
	});
}
