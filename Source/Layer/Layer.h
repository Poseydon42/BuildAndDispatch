#pragma once

#include "Core/InputState.h"
#include "Renderer/Renderer.h"
#include "Simulation/World.h"

class Layer
{
public:
	virtual ~Layer() = default;

	virtual bool OnMousePress(MouseButton::Button Button, const InputState& InputState, World& World) { return false; }

	virtual bool OnMouseRelease(MouseButton::Button Button, const InputState& InputState, World& World) { return false; }

	virtual bool OnMouseScroll(int32_t Offset, const InputState& InputState, World& World) { return false; }

	virtual void Update(float DeltaTime, const InputState& InputState, World& World) {}

	virtual void Render(Renderer& Renderer, const World& World) const {};
};
