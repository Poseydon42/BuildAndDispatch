#include "World.h"

#include <algorithm>
#include <glm/ext.hpp>

#include "Core/Assert.h"

TrackDirection TrackDirectionFromVector(glm::ivec2 Direction)
{
	if (Direction == glm::ivec2(0, 1))
		return TrackDirection::N;
	if (Direction == glm::ivec2(1, 1))
		return TrackDirection::NE;
	if (Direction == glm::ivec2(1, 0))
		return TrackDirection::E;
	if (Direction == glm::ivec2(1, -1))
		return TrackDirection::SE;
	if (Direction == glm::ivec2(0, -1))
		return TrackDirection::S;
	if (Direction == glm::ivec2(-1, -1))
		return TrackDirection::SW;
	if (Direction == glm::ivec2(-1, 0))
		return TrackDirection::W;
	if (Direction == glm::ivec2(-1, 1))
		return TrackDirection::NW;
	BD_UNREACHABLE();
}

glm::ivec2 TrackDirectionToVector(TrackDirection Direction)
{
	switch (Direction)
	{
	case TrackDirection::N:
		return { 0, 1 };
	case TrackDirection::NE:
		return { 1, 1 };
	case TrackDirection::E:
		return { 1, 0};
	case TrackDirection::SE:
		return { 1, -1 };
	case TrackDirection::S:
		return { 0, -1 };
	case TrackDirection::SW:
		return { -1, -1 };
	case TrackDirection::W:
		return { -1, 0 };
	case TrackDirection::NW:
		return { -1, 1 };
	default:
		BD_UNREACHABLE();
	}
}

static bool IsValidTurn(TrackDirection From, TrackDirection To)
{
	auto V1 = TrackDirectionToVector(From);
	auto V2 = TrackDirectionToVector(To);
	return V1.x * V2.x + V1.y * V2.y < 0;
}

// FIXME: cache the results of this or use a lookup table
std::vector<TrackDirection> World::ListValidPathsInTile(int32_t TileX, int32_t TileY) const
{
	const auto* Tile = FindTile(TileX, TileY);
	if (!Tile)
		return {};

	if (IsDeadEnd(Tile->ConnectedDirections))
		return { Tile->ConnectedDirections };

	std::vector<TrackDirection> Result;
	auto Directions = Tile->ConnectedDirections;
	ForEachExistingDirection(Directions, [&](const TrackDirection& From)
	{
		ForEachExistingDirection(Directions, [&](const TrackDirection& To)
		{
			if (!IsValidTurn(From, To))
				return;

			if (std::ranges::find(Result, From | To) != Result.end())
				return;

			Result.push_back(From | To);
		});
	});
	return Result;
}

void World::AddTrack(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY)
{
	AddTrackInSingleDirection(FromX, FromY, ToX, ToY);
	AddTrackInSingleDirection(ToX, ToY, FromX, FromY);
}

bool World::IsPoint(int32_t TileX, int32_t TileY) const
{
	return ListValidPathsInTile(TileX, TileY).size() > 1;
}

void World::SwitchPoint(int32_t TileX, int32_t TileY)
{
	if (!IsPoint(TileX, TileY))
		return;

	auto* Tile = FindTile(TileX, TileY);
	if (!Tile)
		return;

	auto NumberOfValidPositions = static_cast<uint32_t>(ListValidPathsInTile(TileX, TileY).size());
	Tile->SelectedDirectionIndex = (Tile->SelectedDirectionIndex + 1) % NumberOfValidPositions;
}

std::span<const TrackTile> World::TrackTiles() const
{
	return m_TrackTiles;
}

void World::AddTrackInSingleDirection(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY)
{
	auto DeltaX = ToX - FromX;
	auto DeltaY = ToY - FromY;

	auto Direction = TrackDirectionFromVector(glm::ivec2(DeltaX, DeltaY));

	auto* ExistingTile = FindTile(FromX, FromY);
	if (!ExistingTile)
	{
		m_TrackTiles.emplace_back(glm::ivec2(FromX, FromY), TrackDirection::None);
		ExistingTile = &m_TrackTiles.back();
	}

	if (!!(ExistingTile->ConnectedDirections & Direction))
	{
		BD_LOG_WARNING("Trying to add track from tile ({}, {}) to tile ({}, {}), which already exists", FromX, FromY, ToX, ToY);
		return;
	}

	ExistingTile->ConnectedDirections = ExistingTile->ConnectedDirections | Direction;
}

const TrackTile* World::FindTile(int32_t TileX, int32_t TileY) const
{
	return const_cast<World*>(this)->FindTile(TileX, TileY);
}

TrackTile* World::FindTile(int32_t TileX, int32_t TileY)
{
	auto It = std::ranges::find_if(m_TrackTiles, [&](const TrackTile& Candidate)
	{
		return Candidate.Tile == glm::ivec2(TileX, TileY);
	});
	return (It == m_TrackTiles.end() ? nullptr : &*It);
}
