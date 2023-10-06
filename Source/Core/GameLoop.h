#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Layer/Layer.h"
#include "Simulation/World.h"

class GameLoop
{
public:
	static std::unique_ptr<GameLoop> Create();

	int Run();

private:
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Renderer> m_Renderer;

	World m_World;

	std::vector<std::unique_ptr<Layer>> m_Layers;

	GameLoop(std::unique_ptr<Window> Window, std::unique_ptr<Renderer> Renderer);
};
