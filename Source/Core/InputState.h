#pragma once

#include <glm/glm.hpp>

namespace MouseButton
{
	enum Button
	{
		Left = 0,
		Right,
		Middle,
		Count_
	};
}

namespace ButtonEventType
{
	enum Type
	{
		Press,
		Release,
	};
}

struct InputState
{
	glm::ivec2 MousePositionBoundaries = {};

	glm::ivec2 MousePosition = {};
	glm::ivec2 MousePositionDelta = {};
	
	bool MouseButtonStates[MouseButton::Count_] = { false };
};
