#include "World.h"

#include <algorithm>
#include <functional>
#include <stack>
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

void World::AddSignal(SignalLocation Location)
{
	BD_ASSERT(std::abs(Location.FromTile.x - Location.ToTile.x) <= 1
	       && std::abs(Location.FromTile.y - Location.ToTile.y) <= 1 
	       && Location.FromTile != Location.ToTile);

	const auto* ExistingSignal = FindSignal(Location);
	if (ExistingSignal)
	{
		BD_LOG_WARNING("Trying to add signal from tile ({}, {}) to tile ({}, {}), which already exists",
			Location.FromTile.x, Location.ToTile.x, Location.FromTile.y, Location.ToTile.y);
		return;
	}

	Signal NewSignal =
	{
		.Location = Location,
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
	// Reset the state of all occupied tracks to free (it is easier to recompute which tiles
	// are occupied from scratch than use the state from the previous frame).
	std::ranges::for_each(m_TrackTiles, [](TrackTile& Tile)
	{
		ForEachExistingDirection(Tile.ConnectedDirections, [&Tile](TrackDirection Direction)
		{
			if (Tile.State(Direction) == TrackState::Occupied)
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

void World::SwitchSignal(SignalLocation Location)
{
	auto Signal = FindSignal(Location);
	if (!Signal)
		return;

	using SignalStateType = std::underlying_type_t<SignalState>;
	Signal->State = static_cast<SignalState>((static_cast<SignalStateType>(Signal->State) + 1) % static_cast<SignalStateType>(SignalState::_Count));
}

std::optional<Route> World::TryCreateRoute(SignalLocation From, SignalLocation To)
{
	auto StartTileBeforeSignal = From.FromTile;
	auto StartTileAfterSignal = From.ToTile;
	auto EndTile = To.FromTile;

	Route Result = { .From = From, .To = To };
	Result.Tiles.push_back(StartTileBeforeSignal);

	// NOTE: using vector of bytes instead of bools here because C++23 still doesn't implement vector<bool> properly
	std::vector<uint8_t> Visited(m_TrackTiles.size(), 0);

	// Do recursive DFS to find a path from StartTile to EndTile
	// FIXME: this is a very naive implementation, we should probably use A* here
	std::function<bool(const TrackTile*, const TrackTile*)> Search = [&](const TrackTile* Current, const TrackTile* Previous)
	{
		if (Visited[Current - m_TrackTiles.data()])
			return false;
		Visited[Current - m_TrackTiles.data()] = 1;

		// NOTE: this means we're creating a route to a signal which points in the direction opposite to the direction of the start tile
		if (Current->Tile == To.ToTile)
			return false;

		if (Current->Tile == EndTile)
			return true;

		auto DirectionToPreviousTile = OppositeDirection(TrackDirectionFromVector(Current->Tile - Previous->Tile));

		for (const auto& Path : ListValidPathsInTile(Current->Tile.x, Current->Tile.y))
		{
			if (IsDeadEnd(Path))
				continue;

			if (!(Path & DirectionToPreviousTile))
				continue;
			auto Direction = Path & ~DirectionToPreviousTile;

			auto NextTileCoords = Current->Tile + TrackDirectionToVector(Direction);
			auto* Next = FindTile(NextTileCoords.x, NextTileCoords.y);
			BD_ASSERT(Next);

			Result.Tiles.push_back(NextTileCoords);
			if (Search(Next, Current))
				return true;
			Result.Tiles.pop_back();
		}

		return false;
	};

	Result.Tiles.push_back(StartTileAfterSignal);
	if (Search(FindTile(From.ToTile.x, From.ToTile.y), FindTile(StartTileBeforeSignal.x, StartTileBeforeSignal.y)))
		return Result;

	return std::nullopt;
}

bool World::TryOpenRoute(const Route& Route)
{
	// Checking the route is clear
	for (size_t Index = 0; Index < Route.Tiles.size() - 1; ++Index)
	{
		auto* From = FindTile(Route.Tiles[Index].x, Route.Tiles[Index].y);
		auto* To = FindTile(Route.Tiles[Index + 1].x, Route.Tiles[Index + 1].y);
		BD_ASSERT(From && To);

		auto Direction = TrackDirectionFromVector(To->Tile - From->Tile);

		// NOTE: we don't check the state of the first tile in the route because it might be occupied by a train stopped right in front of a signal
		if ((From->State(Direction) != TrackState::Free && Index > 0) || To->State(OppositeDirection(Direction)) != TrackState::Free)
			return false;
	}

	// Opening the route
	for (size_t Index = 0; Index < Route.Tiles.size() - 1; ++Index)
	{
		auto* From = FindTile(Route.Tiles[Index].x, Route.Tiles[Index].y);
		auto* To = FindTile(Route.Tiles[Index + 1].x, Route.Tiles[Index + 1].y);
		BD_ASSERT(From && To);

		if (IsPoint(From->Tile.x, From->Tile.y))
		{
			BD_ASSERT(Index != 0); // A point should not be the first tile in the route
			auto* Previous = FindTile(Route.Tiles[Index - 1].x, Route.Tiles[Index - 1].y);

			auto IncomingDirection = OppositeDirection(TrackDirectionFromVector(From->Tile - Previous->Tile));
			auto OutgoingDirection = TrackDirectionFromVector(To->Tile - From->Tile);

			auto Path = IncomingDirection | OutgoingDirection;

			auto PossiblePaths = ListValidPathsInTile(From->Tile.x, From->Tile.y);
			for (size_t PathIndex = 0; PathIndex < PossiblePaths.size(); ++PathIndex)
			{
				if (PossiblePaths[PathIndex] != Path)
					continue;
				From->SelectedPath = static_cast<uint32_t>(PathIndex);
			}
		}

		auto Direction = TrackDirectionFromVector(To->Tile - From->Tile);

		// NOTE: do not reserve the little piece of track before the signal
		if (Index != 0)
			From->SetState(Direction, TrackState::Reserved);
		To->SetState(OppositeDirection(Direction), TrackState::Reserved);
	}

	// NOTE: we need to set the state of the piece of track right before the destination signal to reserved
	auto DestinationSignalLocation = Route.To;
	auto* LastTile = FindTile(DestinationSignalLocation.FromTile.x, DestinationSignalLocation.FromTile.y);
	LastTile->SetState(TrackDirectionFromVector(DestinationSignalLocation.ToTile - DestinationSignalLocation.FromTile), TrackState::Reserved);

	auto* StartSignal = FindSignal(Route.From);
	BD_ASSERT(StartSignal);
	StartSignal->State = SignalState::Clear;

	return true;
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
			auto* Signal = FindSignal({ .FromTile = CurrentTile->Tile, .ToTile = NewTile->Tile });
			if (Signal && !CanTrainPassSignal(Signal->State))
				break;

			// Reset the state of the signal we just passed to danger
			if (Signal && Signal->State == SignalState::Clear)
				Signal->State = SignalState::Danger;

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
		FloodFillOccupiedTrack(Tile, CurrentDirectionInTile);
	}
}

void World::FloodFillOccupiedTrack(TrackTile* InitialTile, TrackDirection InitialTileSegment)
{
	std::stack<std::pair<TrackTile*, TrackDirection>> FillStack;
	FillStack.emplace(InitialTile, InitialTileSegment);

	int RepeatCount = 0;
	while (!FillStack.empty() && RepeatCount++ < 1000)
	{
		auto [Tile, Direction] = FillStack.top();
		FillStack.pop();

		// Do not process the tile if it already has at least one occupied segment - otherwise
		// we'll run into an endless loop here
		if (Tile->HasAny(TrackState::Occupied))
			continue;

		// The direction we start with is always occupied
		Tile->SetState(Direction, TrackState::Occupied);

		// If the current direction is not a part of active path we are done with this tile
		auto ActivePath = ListValidPathsInTile(Tile->Tile.x, Tile->Tile.y)[Tile->SelectedPath];
		if (!(ActivePath & Direction))
			continue;

		// Otherwise, go over the two directions in the active path
		ForEachExistingDirection(ActivePath, [&](TrackDirection ExistingDirection)
		{
			Tile->SetState(ExistingDirection, TrackState::Occupied);

			glm::ivec2 NeighborTileCoordinates = Tile->Tile + TrackDirectionToVector(ExistingDirection);
			auto* NeighborTile = FindTile(NeighborTileCoordinates.x, NeighborTileCoordinates.y);

			// If there is a tile in the currently processed direction and no signal between these tiles (we have
			// to check in both directions), add it to the queue
			if (NeighborTile
				&& FindSignal({ .FromTile = Tile->Tile, .ToTile = NeighborTile->Tile }) == nullptr
				&& FindSignal({ .FromTile = NeighborTile->Tile, .ToTile = Tile->Tile }) == nullptr)
			{
				FillStack.emplace(NeighborTile, OppositeDirection(ExistingDirection));
			}
		});
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

const Signal* World::FindSignal(SignalLocation Location) const
{
	return const_cast<World*>(this)->FindSignal(Location);
}

Signal* World::FindSignal(SignalLocation Location)
{
	auto It = std::ranges::find_if(m_Signals, [&](const Signal& Candidate)
	{
		return Candidate.Location == Location;
	});
	return (It == m_Signals.end() ? nullptr : &*It);
}

bool World::CanMoveToTile(const TrackTile& From, const TrackTile& To) const
{
	if (!From.IsConnectedTo(To))
		return false;

	// Signals should not be unconditionally passable, unlike regular tracks
	if (FindSignal({ .FromTile = From.Tile, .ToTile = To.Tile }) != nullptr)
		return false;

	return true;
}
