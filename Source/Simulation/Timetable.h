#pragma once

#include <string>

#include "Core/Assert.h"
#include "Simulation/WorldTime.h"

enum class TimetableState
{
	NotSpawned = 0,
	MovingToDestination,
	StoppedAtDestination,
	MovingToExit,
	Left,
};

struct Timetable
{
	WorldTime SpawnTime;
	WorldTime ArrivalTime;
	WorldTime DepartureTime;

	float MinStopDuration = 0.0f;

	std::string SpawnLocation;
	std::string PreferredTrack;
	std::string LeaveLocation;

	TimetableState State = TimetableState::NotSpawned;

	void AdvanceState()
	{
		BD_ASSERT(State != TimetableState::Left);
		State = static_cast<TimetableState>(std::to_underlying(State) + 1);
	}

	bool IsPresentInTheWorld() const
	{
		return State == TimetableState::MovingToDestination || State == TimetableState::StoppedAtDestination || State == TimetableState::MovingToExit;
	}
};
