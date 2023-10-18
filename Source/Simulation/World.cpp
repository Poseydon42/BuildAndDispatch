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

void World::SpawnTrain(int32_t X, int32_t Y, TrackDirection Direction)
{
	const auto* Tile = FindTile(X, Y);
	if (!Tile)
	{
		BD_LOG_WARNING("Trying to spawn train at ({},{}), direction {}, which is not a valid track tile", X, Y, TrackDirectionToString(Direction));
		return;
	}
	if (!(Tile->ConnectedDirections & Direction))
	{
		BD_LOG_WARNING("Trying to spawn train at ({},{}), direction {} in invalid direction", X, Y, TrackDirectionToString(Direction));
		return;
	}
	
	auto Paths = ListValidPathsInTile(X, Y);
	if (!(Paths[Tile->SelectedPath] & Direction))
	{
		BD_LOG_WARNING("Trying to spawn train at ({},{}), direction {}, which is not a direction of the currently selected path", X, Y, TrackDirectionToString(Direction));
		return;
	}

	Train NewTrain =
	{
		.Tile = glm::ivec2(X, Y),
		.OffsetInTile = 0.0f,
		.Direction = Direction
	};
	m_Trains.push_back(NewTrain);
}

void World::Update(float DeltaTime)
{
	// Reset the state of all tracks to free
	std::ranges::for_each(m_TrackTiles, [](auto& Tile)
	{
		ForEachExistingDirection(Tile.ConnectedDirections, [&Tile](TrackDirection Direction)
		{
			Tile.SetState(Direction, TrackState::Free);
		});
	});

	std::ranges::for_each(m_Trains, [&](auto& Train) { UpdateTrain(Train, DeltaTime); });
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
	Tile->SelectedPath = (Tile->SelectedPath + 1) % NumberOfValidPositions;
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

std::span<const Train> World::Trains() const
{
	return m_Trains;
}

void World::UpdateTrain(Train& Train, float DeltaTime)
{
	// In m/s
	constexpr float TrainSpeed = 2.0f;
	float DistanceToTravel = TrainSpeed * DeltaTime;

	const auto* CurrentTile = FindTile(Train.Tile.x, Train.Tile.y);
	BD_ASSERT(CurrentTile);
	while (CurrentTile && DistanceToTravel > 0.001f)
	{
		// Handle motion towards the center of the tile first
		if (Train.OffsetInTile < 0.0f)
		{
			// -Train.OffsetInTile represents the percentage of the distance that the
			// train still has to travel towards the center of the current tile.
			float LengthOfCurrentHalfTile = HalfTileLengthInDirection(Train.Direction);
			float DistanceToTravelToCenter = -Train.OffsetInTile * LengthOfCurrentHalfTile;
			if (DistanceToTravelToCenter >= DistanceToTravel)
			{
				float DistanceTraveled = DistanceToTravel / LengthOfCurrentHalfTile;
				DistanceToTravel = 0.0f;
				Train.OffsetInTile += DistanceTraveled; // Because Train.OffsetInTile < 0 and DistanceTraveled > 0
				BD_ASSERT(Train.OffsetInTile <= 0.0f);
			}
			else
			{
				DistanceToTravel -= DistanceToTravelToCenter;
				Train.OffsetInTile = 0.0f;
			}
		}

		// Set the new direction when going over the center of the tile
		if (Train.OffsetInTile == 0.0f)
		{
			// We reached end of the track
			if (IsDeadEnd(CurrentTile->ConnectedDirections))
				break;

			auto SelectedPath = ListValidPathsInTile(Train.Tile.x, Train.Tile.y)[CurrentTile->SelectedPath];
			BD_ASSERT(!!(SelectedPath & OppositeDirection(Train.Direction))); // NOTE: just to ensure that the train's path so far was valid

			// NOTE: the direction the train is currently moving in is OPPOSITE to the direction
			//       of the track segment it is currently occupying since it's moving TOWARDS
			//       the center of the current tile.
			auto DirectionInFront = SelectedPath & ~OppositeDirection(Train.Direction);
			Train.Direction = DirectionInFront;
		}

		// Handle motion away from the center of the tile
		if (Train.OffsetInTile >= 0.0f)
		{
			// (1.0 - Train.OffsetInTile) represents the percentage of the distance that the
			// train still has to travel towards the border with the next tile.
			float LengthOfCurrentHalfTile = HalfTileLengthInDirection(Train.Direction);
			float DistanceToTravelToEdge = (1.0f - Train.OffsetInTile) * LengthOfCurrentHalfTile;
			if (DistanceToTravelToEdge > DistanceToTravel)
			{
				float DistanceTraveled = DistanceToTravel / LengthOfCurrentHalfTile;
				DistanceToTravel = 0.0f;
				Train.OffsetInTile += DistanceTraveled;
				BD_ASSERT(Train.OffsetInTile <= 1.0f);
			}
			else
			{
				DistanceToTravel -= DistanceToTravelToEdge;
				Train.OffsetInTile = 1.0f;
			}
		}

		// Move to the next tile if train is right at the boundary
		constexpr float Epsilon = 1.0e-5f;
		if (std::abs(Train.OffsetInTile - 1.0f) < Epsilon)
		{
			auto NewTileCoords = Train.Tile + TrackDirectionToVector(Train.Direction);
			const auto* NewTile = FindTile(NewTileCoords.x, NewTileCoords.y);

			// Reached a dead end
			if (!NewTile)
				break;

			// Check if there is a red signal ahead
			const auto* Signal = FindSignal(CurrentTile->Tile.x, CurrentTile->Tile.y, NewTile->Tile.x, NewTile->Tile.y);
			if (Signal && !CanTrainPassSignal(Signal->State))
				break;

			CurrentTile = NewTile;
			Train.Tile = NewTileCoords;
			Train.OffsetInTile = -1.0f;
		}
	}

	// Set the state of the track the train is currently at to occupied
	auto* Tile = FindTile(Train.Tile.x, Train.Tile.y);
	if (Tile)
	{
		auto CurrentDirectionInTile = (Train.OffsetInTile < 0.0f ? OppositeDirection(Train.Direction) : Train.Direction);
		Tile->SetState(CurrentDirectionInTile, TrackState::Occupied);
	}
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
