#pragma once

#include <glm/glm.hpp>

struct Rect2D
{
	glm::vec2 Min = {};
	glm::vec2 Max = {};

	constexpr float Width() const { return Max.x - Min.x; }
	constexpr float Height() const { return Max.y - Min.y; }

	constexpr float& Left() { return Min.x; }
	constexpr const float& Left() const { return Min.x; }

	constexpr float& Right() { return Max.x; }
	constexpr const float& Right() const { return Max.x; }

	constexpr float& Top() { return Max.y; }
	constexpr const float& Top() const { return Max.y; }

	constexpr float& Bottom() { return Min.y; }
	constexpr const float& Bottom() const { return Min.y; }
};
