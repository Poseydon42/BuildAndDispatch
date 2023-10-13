#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "Simulation/Signal.h"
#include "Simulation/Track.h"

class World
{
public:
	void AddTrack(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	void AddSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	std::vector<TrackDirection> ListValidPathsInTile(int32_t TileX, int32_t TileY) const;

	bool IsPoint(int32_t TileX, int32_t TileY) const;

	void SwitchPoint(int32_t TileX, int32_t TileY);

	void SwitchSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	std::span<const TrackTile> TrackTiles() const;
	std::span<const Signal> Signals() const;

private:
	std::vector<TrackTile> m_TrackTiles;
	std::vector<Signal> m_Signals;

	void AddTrackInSingleDirection(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	const TrackTile* FindTile(int32_t TileX, int32_t TileY) const;
	TrackTile* FindTile(int32_t TileX, int32_t TileY);

	const Signal* FindSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY) const;
	Signal* FindSignal(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);
};
