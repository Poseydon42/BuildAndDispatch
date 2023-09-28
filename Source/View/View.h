#pragma once

#include "Renderer/Renderer.h"
#include "Simulation/World.h"

class View
{
public:
	virtual ~View() = default;

	virtual void Render(Renderer& Renderer, const World& World) const = 0;
};
