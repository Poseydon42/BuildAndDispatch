#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "Simulation/Route.h"
#include "Simulation/Signal.h"
#include "Simulation/Track.h"
#include "Simulation/Train.h"

class World
{
public:
	void AddTrack(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	void AddSignal(SignalLocation Location);

	void SpawnTrain(int32_t X, int32_t Y, TrackDirection Direction);

	void Update(float DeltaTime);

	std::vector<TrackDirection> ListValidPathsInTile(int32_t TileX, int32_t TileY) const;

	bool IsPoint(int32_t TileX, int32_t TileY) const;

	void SwitchPoint(int32_t TileX, int32_t TileY);

	void SwitchSignal(SignalLocation Location);

	std::optional<Route> TryCreateRoute(SignalLocation From, SignalLocation To);

	bool TryOpenRoute(const Route& Route);

	std::span<const TrackTile> TrackTiles() const;
	std::span<const Signal> Signals() const;
	std::span<const Train> Trains() const;

private:
	std::vector<TrackTile> m_TrackTiles;
	std::vector<Signal> m_Signals;
	std::vector<Train> m_Trains;

	void UpdateTrain(Train& Train, float DeltaTime);

	void FloodFillOccupiedTrack(TrackTile* InitialTile, TrackDirection InitialTileSegment);

	void AddTrackInSingleDirection(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	const TrackTile* FindTile(int32_t TileX, int32_t TileY) const;
	TrackTile* FindTile(int32_t TileX, int32_t TileY);

	const Signal* FindSignal(SignalLocation Location) const;
	Signal* FindSignal(SignalLocation Location);

	bool CanMoveToTile(const TrackTile& From, const TrackTile& To) const;
};
