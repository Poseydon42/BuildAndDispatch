#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "Simulation/Route.h"
#include "Simulation/Signal.h"
#include "Simulation/Track.h"
#include "Simulation/Train.h"
#include "Simulation/WorldTime.h"

class World
{
public:
	void AddTrack(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	uint32_t AddTrackArea(TrackArea Area);

	void AddSignal(SignalLocation Location);

	void SpawnTrain(int32_t X, int32_t Y, TrackDirection Direction, float Length);

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

	float SimulationSpeed() { return m_SimulationSpeed; }
	void SetSimulationSpeed(float NewSpeed) { m_SimulationSpeed = NewSpeed; }

	WorldTime CurrentTime() const { return m_CurrentTime; }

private:
	std::vector<TrackTile> m_TrackTiles;
	std::vector<TrackArea> m_TrackAreas;

	std::vector<Signal> m_Signals;
	std::vector<Train> m_Trains;

	float m_SimulationSpeed = 1.0f;
	WorldTime m_CurrentTime;

	// NOTE: TileBorderCallbackType = bool()(const TrackTile& From, const TrackTile& To);
	//       The callback should return true if the train can proceed to the next tile.
	// NOTE: TileCallbackType = void()(const TrackTile& Tile, TrackDirection Segment);
	//       The callback is called for each tile segment the train passes through.
	template<typename TileBorderCallbackType, typename TileCallbackType>
	float MoveAlongTrack(const TrackTile*& Tile, TrackDirection& Direction, float& OffsetInTile, float MaxDistance, TileBorderCallbackType&& TileBorderCallback, TileCallbackType&& TileCallback) const;

	void UpdateTrain(Train& Train, float DeltaTime);

	void FloodFillOccupiedTrack(TrackTile* InitialTile, TrackDirection InitialTileSegment);

	void AddTrackInSingleDirection(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	const TrackTile* FindTile(int32_t TileX, int32_t TileY) const;
	TrackTile* FindTile(int32_t TileX, int32_t TileY);

	const Signal* FindSignal(SignalLocation Location) const;
	Signal* FindSignal(SignalLocation Location);

	bool CanMoveToTile(const TrackTile& From, const TrackTile& To) const;

	void OverwriteTile(const TrackTile& Tile);
	void OverwriteSignal(const Signal& Signal);
	void AddTrainUnsafe(const Train& Train);

	friend class WorldSerialization;
};
