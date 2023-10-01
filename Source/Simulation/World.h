#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <span>
#include <vector>

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

TrackDirection TrackDirectionFromVector(glm::ivec2 Direction);

glm::ivec2 TrackDirectionToVector(TrackDirection Direction);

enum class SignalState
{
	Danger = 0,
	Clear,
	_Count
};

constexpr bool IsDeadEnd(TrackDirection Direction)
{
	return (Direction == TrackDirection::N || Direction == TrackDirection::NE || Direction == TrackDirection::E || Direction == TrackDirection::SE ||
			Direction == TrackDirection::S || Direction == TrackDirection::SW || Direction == TrackDirection::W || Direction == TrackDirection::NW);
}

struct TrackTile
{
	glm::ivec2 Tile;

	TrackDirection ConnectedDirections = TrackDirection::None;
	uint32_t SelectedDirectionIndex = 0;
};

struct Signal
{
	glm::ivec2 From;
	glm::ivec2 To;

	SignalState State = SignalState::Danger;
};

class World
{
public:
	void AddTrack(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	void AddSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	std::vector<TrackDirection> ListValidPathsInTile(int32_t TileX, int32_t TileY) const;

	bool IsPoint(int32_t TileX, int32_t TileY) const;

	void SwitchPoint(int32_t TileX, int32_t TileY);

	std::span<const TrackTile> TrackTiles() const;
	std::span<const Signal> Signals() const;

private:
	std::vector<TrackTile> m_TrackTiles;
	std::vector<Signal> m_Signals;

	void AddTrackInSingleDirection(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	const TrackTile* FindTile(int32_t TileX, int32_t TileY) const;
	TrackTile* FindTile(int32_t TileX, int32_t TileY);
};
