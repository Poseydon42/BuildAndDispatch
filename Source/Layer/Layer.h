#pragma once

#include "Renderer/Renderer.h"
#include "Simulation/World.h"

class Layer
{
public:
	virtual ~Layer() = default;

	virtual void OnMousePress(MouseButton Button, int32_t CursorX, int32_t CursorY, World& World) const {};

	virtual void OnMouseRelease(MouseButton Button, int32_t CursorX, int32_t CursorY, World& World) const {};

	virtual void Render(Renderer& Renderer, const World& World) const {};
};
