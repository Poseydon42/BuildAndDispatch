#pragma once

#include "Renderer/Renderer.h"
#include "Simulation/World.h"

class Layer
{
public:
	virtual ~Layer() = default;

	virtual bool OnMousePress(MouseButton Button, glm::ivec2 ScreenCursorPos, glm::vec2 WorldCursorPos, World& World) const { return false; }

	virtual bool OnMouseRelease(MouseButton Button, glm::ivec2 ScreenCursorPos, glm::vec2 WorldCursorPos, World& World) const { return false; }

	virtual void Render(Renderer& Renderer, const World& World) const {};
};
