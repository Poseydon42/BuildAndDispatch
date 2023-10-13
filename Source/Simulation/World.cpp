#include "World.h"

#include <algorithm>
#include <glm/ext.hpp>

#include "Core/Assert.h"

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

void World::AddSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY)
{
	glm::ivec2 From = { FromX, FromY };
	glm::ivec2 To = { ToX, ToY };

	BD_ASSERT(std::abs(FromX - ToX) <= 1 && std::abs(FromY - ToY) <= 1 && From != To);

	const auto* ExistingSignal = FindSignal(FromX, FromY, ToX, ToY);
	if (ExistingSignal)
	{
		BD_LOG_WARNING("Trying to add signal from tile ({}, {}) to tile ({}, {}), which already exists", FromX, FromY, ToX, ToY);
		return;
	}

	Signal NewSignal =
	{
		.From = From,
		.To = To,
		.State = SignalState::Danger
	};
	m_Signals.push_back(NewSignal);
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

void World::SwitchSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY)
{
	auto Signal = FindSignal(FromX, FromY, ToX, ToY);
	if (!Signal)
		return;

	using SignalStateType = std::underlying_type_t<SignalState>;
	Signal->State = static_cast<SignalState>((static_cast<SignalStateType>(Signal->State) + 1) % static_cast<SignalStateType>(SignalState::_Count));
}

std::span<const TrackTile> World::TrackTiles() const
{
	return m_TrackTiles;
}

std::span<const Signal> World::Signals() const
{
	return m_Signals;
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

const Signal* World::FindSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY) const
{
	return const_cast<World*>(this)->FindSignal(FromX, FromY, ToX, ToY);
}

Signal* World::FindSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY)
{
	auto It = std::ranges::find_if(m_Signals, [&](const Signal& Candidate)
	{
		return Candidate.From == glm::ivec2(FromX, FromY) && Candidate.To == glm::ivec2(ToX, ToY);
	});
	return (It == m_Signals.end() ? nullptr : &*It);
}
