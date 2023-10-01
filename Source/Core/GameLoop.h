#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Simulation/World.h"
#include "View/View.h"

class GameLoop
{
public:
	static std::unique_ptr<GameLoop> Create();

	int Run();

private:
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Renderer> m_Renderer;

	World m_World;

	std::vector<std::unique_ptr<View>> m_Views;

	glm::vec2 m_CameraLocation = { 0.0f, 0.0f };
	float m_CameraScale = 1.0f;

	GameLoop(std::unique_ptr<Window> Window, std::unique_ptr<Renderer> Renderer);
};
