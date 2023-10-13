#include "GameLoop.h"

#include <algorithm>

#include "Core/Logger.h"
#include "Layer/TrackLayer.h"

static constexpr uint32_t WindowWidth = 1280;
static constexpr uint32_t WindowHeight = 720;
static constexpr const char* WindowName = "Build & Dispatch";

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

template<typename FuncType>
void DispatchEventForEachLayer(const std::vector<std::unique_ptr<Layer>>& Layers, FuncType&& Func)
{
	for (int32_t Index = static_cast<int32_t>(Layers.size()) - 1; Index >= 0; --Index)
	{
		const auto& Layer = Layers[Index];

		bool EventWasHandled = Func(*Layer);

		if (EventWasHandled)
			break;
	}
}

std::unique_ptr<GameLoop> GameLoop::Create()
{
	auto Window = Window::Create(WindowWidth, WindowHeight, WindowName, 16);
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
		UpdateInputState();

		for (size_t Index = m_Layers.size(); Index > 0; Index--)
		{
			const auto& Layer = m_Layers[Index - 1];
			Layer->Update(0.0f, m_InputState, m_World); // FIXME: add delta time
		}

		m_Renderer->BeginFrame();

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
	m_Layers.push_back(TrackLayer::Create());

	GenerateDebugWorld(m_World);

	m_Window->AddMouseButtonCallback([this](MouseButton::Button Button, ButtonEventType::Type Type, int32_t CursorX, int32_t CursorY)
	{
		if (Type == ButtonEventType::Press)
			DispatchEventForEachLayer(m_Layers, [&](Layer& Layer) { return Layer.OnMousePress(Button, m_InputState, m_World); });
		if (Type == ButtonEventType::Release)
			DispatchEventForEachLayer(m_Layers, [&](Layer& Layer) { return Layer.OnMouseRelease(Button, m_InputState, m_World); });
	});

	m_Window->AddMouseScrollCallback([this](int32_t Offset)
	{
		DispatchEventForEachLayer(m_Layers, [&](Layer& Layer) { return Layer.OnMouseScroll(Offset, m_InputState, m_World); });
	});
}

void GameLoop::UpdateInputState()
{
	m_InputState.MousePositionBoundaries = { m_Window->Width(), m_Window->Height() };

	m_InputState.MousePosition = m_Window->GetCursorPosition();
	m_InputState.MousePositionDelta = m_Window->GetCursorDelta();

	for (auto Button = 0; Button < MouseButton::Count_; ++Button)
	{
		auto ButtonState = m_Window->IsMouseButtonPressed(static_cast<MouseButton::Button>(Button));
		m_InputState.MouseButtonStates[Button] = ButtonState;
	}
}
