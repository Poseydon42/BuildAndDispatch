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

	void AddTrackArea(TrackArea Area);

	void AddExit(Exit Exit);

	void AddSignal(SignalLocation Location, SignalKind Kind);

	void SpawnTrain(std::string ID, float Length, Timetable Timetable);

	void Update(float DeltaTime);

	std::vector<TrackDirection> ListValidPathsInTile(int32_t TileX, int32_t TileY) const;

	bool IsPoint(int32_t TileX, int32_t TileY) const;

	void SwitchPoint(int32_t TileX, int32_t TileY);

	void SwitchSignal(SignalLocation Location);

	std::optional<Route> TryCreateRoute(SignalLocation From, SignalLocation To);

	bool TryOpenRoute(const Route& Route);

	std::span<const TrackTile> TrackTiles() const;
	std::span<const TrackArea> TrackAreas() const;
	std::span<const Exit> Exits() const;
	std::span<const Signal> Signals() const;
	std::span<const Train> Trains() const;

	float SimulationSpeed() { return m_SimulationSpeed; }
	void SetSimulationSpeed(float NewSpeed) { m_SimulationSpeed = NewSpeed; }

	WorldTime CurrentTime() const { return m_CurrentTime; }

private:
	std::vector<TrackTile> m_TrackTiles;
	std::vector<TrackArea> m_TrackAreas;
	std::vector<Exit> m_Exits;

	std::vector<Signal> m_Signals;
	std::vector<Train> m_Trains;

	float m_SimulationSpeed = 1.0f;
	WorldTime m_CurrentTime;

	// NOTE: TileBorderCallbackType = bool()(const TrackTile& From, const TrackTile& To);
	//       The callback should return true if the train can proceed to the next tile.
	// NOTE: TileCallbackType = void()(const TrackTile& Tile, TrackDirection Segment);
	//       The callback is called for each tile segment the train passes through.
	template<typename TileBorderCallbackType, typename TileCallbackType>
	float MoveAlongTrack(
		const TrackTile*& Tile, TrackDirection& Direction, float& OffsetInTile,
		float MaxDistance,
		TileBorderCallbackType&& TileBorderCallback,
		TileCallbackType&& TileCallback
	) const;

	void UpdateTrain(Train& Train, float DeltaTime);

	void UpdateMovingTrain(Train& Train, float DeltaTime);

	void UpdateTrackStateForTrain(const Train& Train);

	void FloodFillOccupiedTrack(TrackTile* InitialTile, TrackDirection InitialTileSegment);

	void AddTrackInSingleDirection(int32_t FromX, int32_t FromY, int32_t ToX, int32_t ToY);

	const TrackTile* FindTile(int32_t TileX, int32_t TileY) const;
	TrackTile* FindTile(int32_t TileX, int32_t TileY);
	const TrackTile* FindTile(glm::ivec2 Tile) const;
	TrackTile* FindTile(glm::ivec2 Tile);

	const Signal* FindSignal(SignalLocation Location) const;
	Signal* FindSignal(SignalLocation Location);

	const Exit* FindExit(std::string_view Name) const;

	bool CanMoveToTile(const TrackTile& From, const TrackTile& To) const;

	bool IsBlockInFrontFullyClear(const Signal& Signal) const;

	void OverwriteTile(const TrackTile& Tile);
	void OverwriteSignal(const Signal& Signal);
	void AddTrainUnsafe(const Train& Train);
	void OverrideTime(WorldTime Time);

	friend class WorldSerialization;
};
