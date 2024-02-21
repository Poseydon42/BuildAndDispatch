#pragma once

#include <glm/vec2.hpp>

#include "Simulation/Timetable.h"
#include "Simulation/Track.h"

struct Train
{
	/*
	 * ID or name of the train
	 */
	std::string ID;

	/*
	 * The tile that the train is currently moving through.
	 */
	glm::ivec2 Tile;

	/*
	 * Offset from the tile center in the train's direction.
	 * 0 means that the train is at the center of the tile.
	 * +1 means the train is just about to leave the tile and it is.
	 * -1 means the train has just entered the tile.
	 */
	float OffsetInTile = 0.0f;

	/*
	 * Current direction the train is moving in. It might not be one of the directions
	 * in the current tile when the train has just entered that tile and is moving towards
	 * its center. In such case the train's direction would be the exact opposite of one of
	 * the directions in the tile.
	 */
	TrackDirection Direction;

	/*
	 * Length of the train in meters.
	 */
	const float Length = 1.0f;

	/*
	 * Train's timetable
	 */
	Timetable Timetable;

	/*
	 * Player's score for correctly and timely routing the current train.
	 */
	float Score = 0.0f;

	/*
	 * True if and only if the train is moving
	 */
	bool IsMoving = false;

	/*
	 * NOTE: the following fields should not be exposed publicly. They are just cached data to make the simulation easier to code.
	 */
	const TrackArea* CurrentArea = nullptr;
};
