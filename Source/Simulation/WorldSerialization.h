#pragma once

#include <string>

#include "Simulation/World.h"

class WorldSerialization
{
public:
	static std::string Serialize(const World& World);

	static World Deserialize(std::string_view Source);
};
