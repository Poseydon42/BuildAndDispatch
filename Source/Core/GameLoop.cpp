#include "GameLoop.h"

#include <algorithm>

#include "Core/Logger.h"
#include "Layer/GameUILayer.h"
#include "Layer/TrackLayer.h"
#include "Platform/Time.h"
#include "Simulation/WorldSerialization.h"

static constexpr uint32_t WindowWidth = 1280;
static constexpr uint32_t WindowHeight = 720;
static constexpr const char* WindowName = "Build & Dispatch";

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
	auto LastFrameEnd = Time::Now();
	while (!m_Window->ShouldClose())
	{
		auto FrameStart = Time::Now();
		auto DeltaTime = Time::Duration(LastFrameEnd, FrameStart);
		LastFrameEnd = FrameStart;

		m_Window->PollEvents();
		UpdateInputState();

		Rect2D RenderArea = { .Min = { 0.0f, 0.0f }, .Max = m_Renderer->FramebufferSize() };
		for (size_t Index = m_Layers.size(); Index > 0; Index--)
		{
			const auto& Layer = m_Layers[Index - 1];
			Layer->Update(DeltaTime, m_InputState, m_World, RenderArea);
		}

		m_World.Update(DeltaTime);

		m_Renderer->BeginFrame();

		for (const auto& Layer : m_Layers)
		{
			Layer->Render(*m_Renderer, m_World);
			m_Renderer->Flush();
		}

		m_Renderer->EndFrame();

		Time::Sleep(1); // NOTE: this is here just so that we don't get 10k+ FPS and break the world simulation
	}

	return 0;
}

GameLoop::GameLoop(std::unique_ptr<Window> Window, std::unique_ptr<Renderer> Renderer)
	: m_Window(std::move(Window))
	, m_Renderer(std::move(Renderer))
{
	m_Layers.push_back(TrackLayer::Create());
	m_Layers.push_back(std::make_unique<GameUILayer>());

	static constexpr auto DefaultLevelName = "Resources/Levels/Level0.json";
	auto SerializedWorld = FileSystem::ReadFileAsString(DefaultLevelName).value_or("");
	m_World = WorldSerialization::Deserialize(SerializedWorld);

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
