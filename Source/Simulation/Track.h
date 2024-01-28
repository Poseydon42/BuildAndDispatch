#pragma once

#include <bit>
#include <cstdint>
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <type_traits>

#include "Core/Assert.h"

enum class TrackDirection : uint8_t
{
	None = 0,
	N = 1,
	NE = 2,
	E = 4,
	SE = 8,
	S = 16,
	SW = 32,
	W = 64,
	NW = 128,
};

constexpr bool operator!(TrackDirection Value)
{
	return static_cast<std::underlying_type_t<TrackDirection>>(Value) == 0;
}

constexpr TrackDirection operator|(TrackDirection Lhs, TrackDirection Rhs)
{
	return static_cast<TrackDirection>(static_cast<std::underlying_type_t<TrackDirection>>(Lhs) | static_cast<std::underlying_type_t<TrackDirection>>(Rhs));
}

constexpr TrackDirection operator&(TrackDirection Lhs, TrackDirection Rhs)
{
	return static_cast<TrackDirection>(static_cast<std::underlying_type_t<TrackDirection>>(Lhs) & static_cast<std::underlying_type_t<TrackDirection>>(Rhs));
}

constexpr TrackDirection operator~(TrackDirection Lhs)
{
	return static_cast<TrackDirection>(~static_cast<std::underlying_type_t<TrackDirection>>(Lhs));
}

template<typename FunctorType>
constexpr void ForEachExistingDirection(TrackDirection Directions, FunctorType&& Functor)
{
	if (!!(Directions & TrackDirection::N))
		Functor(TrackDirection::N);
	if (!!(Directions & TrackDirection::NE))
		Functor(TrackDirection::NE);
	if (!!(Directions & TrackDirection::E))
		Functor(TrackDirection::E);
	if (!!(Directions & TrackDirection::SE))
		Functor(TrackDirection::SE);
	if (!!(Directions & TrackDirection::S))
		Functor(TrackDirection::S);
	if (!!(Directions & TrackDirection::SW))
		Functor(TrackDirection::SW);
	if (!!(Directions & TrackDirection::W))
		Functor(TrackDirection::W);
	if (!!(Directions & TrackDirection::NW))
		Functor(TrackDirection::NW);
}

constexpr TrackDirection TrackDirectionFromVector(glm::ivec2 Direction)
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

constexpr glm::ivec2 TrackDirectionToVector(TrackDirection Direction)
{
	switch (Direction)
	{
	case TrackDirection::N:
		return { 0, 1 };
	case TrackDirection::NE:
		return { 1, 1 };
	case TrackDirection::E:
		return { 1, 0 };
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

static constexpr const char* TrackDirectionToString(TrackDirection Direction)
{
	switch (Direction)
	{
	case TrackDirection::N:
		return "N";
	case TrackDirection::NE:
		return "NE";
	case TrackDirection::E:
		return "E";
	case TrackDirection::SE:
		return "SE";
	case TrackDirection::S:
		return "S";
	case TrackDirection::SW:
		return "SW";
	case TrackDirection::W:
		return "W";
	case TrackDirection::NW:
		return "NW";
	default:
		BD_UNREACHABLE();
	}
}

constexpr TrackDirection OppositeDirection(TrackDirection Direction)
{
	switch (Direction)
	{
	case TrackDirection::N:
		return TrackDirection::S;
	case TrackDirection::NE:
		return TrackDirection::SW;
	case TrackDirection::E:
		return TrackDirection::W;
	case TrackDirection::SE:
		return TrackDirection::NW;
	case TrackDirection::S:
		return TrackDirection::N;
	case TrackDirection::SW:
		return TrackDirection::NE;
	case TrackDirection::W:
		return TrackDirection::E;
	case TrackDirection::NW:
		return TrackDirection::SE;
	default:
		BD_ASSERT(false);
	}
}

constexpr float HalfTileLengthInDirection(TrackDirection Direction)
{
	switch (Direction)
	{
	case TrackDirection::N:
	case TrackDirection::E:
	case TrackDirection::S:
	case TrackDirection::W:
		return 0.5f;
	case TrackDirection::NE:
	case TrackDirection::SE:
	case TrackDirection::SW:
	case TrackDirection::NW:
		return 0.5f * glm::root_two<float>();
	default:
		BD_UNREACHABLE();
	};
}

constexpr bool IsDeadEnd(TrackDirection Direction)
{
	return (
		Direction == TrackDirection::N || Direction == TrackDirection::NE ||
		Direction == TrackDirection::E || Direction == TrackDirection::SE ||
		Direction == TrackDirection::S || Direction == TrackDirection::SW ||
		Direction == TrackDirection::W || Direction == TrackDirection::NW);
}

constexpr bool AreTilesNeighbors(glm::ivec2 Lhs, glm::ivec2 Rhs)
{
	auto Delta = Lhs - Rhs;
	if (Delta.x == 0 && Delta.y == 0)
		return false;
	if (std::abs(Delta.x) >= 2 || std::abs(Delta.y) >= 2)
		return false;
	return true;
}

enum class TrackState
{
	Free,
	Reserved,
	Occupied,
};

struct TrackTile
{
	glm::ivec2 Tile;

	TrackDirection ConnectedDirections = TrackDirection::None;
	uint32_t SelectedPath = 0;

	constexpr TrackTile(glm::ivec2 InTile, TrackDirection InConnectedDirections)
		: Tile(InTile)
		, ConnectedDirections(InConnectedDirections)
	{
	}

	constexpr TrackState State(TrackDirection Direction) const
	{
		return m_State[StateArrayIndex(Direction)];
	}

	constexpr bool HasAny(TrackState State) const
	{
		return this->State(TrackDirection::N) == State || this->State(TrackDirection::NE) == State ||
			this->State(TrackDirection::E) == State || this->State(TrackDirection::SE) == State ||
			this->State(TrackDirection::S) == State || this->State(TrackDirection::SW) == State ||
			this->State(TrackDirection::W) == State || this->State(TrackDirection::NW) == State;
	}

	constexpr void SetState(TrackDirection Direction, TrackState State)
	{
		m_State[StateArrayIndex(Direction)] = State;
	}

	constexpr bool IsConnectedTo(const TrackTile& Other) const
	{
		return AreTilesNeighbors(Tile, Other.Tile) && !!(ConnectedDirections & TrackDirectionFromVector(Other.Tile - Tile));
	}

private:
	TrackState m_State[8] = { TrackState::Free };

	static constexpr size_t StateArrayIndex(TrackDirection Direction)
	{
		auto DirectionAsByte = std::to_underlying(Direction);
		BD_ASSERT(std::has_single_bit(DirectionAsByte));
		auto Index = std::countr_zero(DirectionAsByte);
		return Index;
	}
};

struct TrackAreaEntryPoint
{
	glm::ivec2 TileFrom;
	glm::ivec2 TileTo;
};

struct TrackArea
{
	std::string Name;

	/*
	 * Locations of the ends of the track area. If a train passes through one of these points in the forward (TileFrom -> TileTo) direction,
	 * it will enter the track area, and if it passes through one of these points in the reverse (TileTo -> TileFrom) direction, it will exit.
	 */
	std::vector<TrackAreaEntryPoint> EntryPoints;
};

struct Exit
{
	std::string Name;
	glm::ivec2 Location;
	TrackDirection SpawnDirection; // NOTE: trains spawn in this direction and exit in the opposite
};
