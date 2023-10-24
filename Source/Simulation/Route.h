#pragma once

#include <vector>

#include "Simulation/Signal.h"

struct Route
{
	SignalLocation From;
	SignalLocation To;

	std::vector<glm::ivec2> Tiles;
};
