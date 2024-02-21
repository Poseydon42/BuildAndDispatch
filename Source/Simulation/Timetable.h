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

class Timetable
{
public:
	Timetable(
		WorldTime InSpawnTime, WorldTime InArrivalTime, WorldTime InDepartureTime, WorldTime InLeaveTime,
		std::string InSpawnLocation, std::string InPreferredTrack, std::string InLeaveLocation, float InMinStopDuration)
		: SpawnTime(InSpawnTime)
		, ArrivalTime(InArrivalTime)
		, DepartureTime(InDepartureTime)
		, LeaveTime(InLeaveTime)
		, SpawnLocation(std::move(InSpawnLocation))
		, PreferredTrack(std::move(InPreferredTrack))
		, LeaveLocation(std::move(InLeaveLocation))
		, MinStopDuration(InMinStopDuration)
	{
	}

	bool IsPresentInTheWorld() const
	{
		return m_State == TimetableState::MovingToDestination || m_State == TimetableState::StoppedAtDestination || m_State == TimetableState::MovingToExit;
	}

	void JustSpawned()
	{
		m_State = TimetableState::MovingToDestination;
	}

	bool ShouldStop(std::string_view TrackAreaName)
	{
		return PreferredTrack == TrackAreaName;
	}

	void JustArrived(std::string_view TrackAreaName, WorldTime CurrentTime)
	{
		BD_ASSERT(m_State == TimetableState::MovingToDestination);

		bool CorrectTrack = (TrackAreaName == PreferredTrack);
		AddScoreForAction(ArrivalTime, CurrentTime, CorrectTrack);
		m_State = TimetableState::StoppedAtDestination;
	}

	bool ShouldDepart(WorldTime CurrentTime)
	{
		BD_ASSERT(m_State == TimetableState::StoppedAtDestination);
		return CurrentTime > DepartureTime && m_StoppingTime > MinStopDuration;
	}

	void JustDeparted(std::string_view TrackAreaName, WorldTime CurrentTime)
	{
		BD_ASSERT(m_State == TimetableState::StoppedAtDestination);

		bool CorrectTrack = (TrackAreaName == PreferredTrack);
		AddScoreForAction(DepartureTime, CurrentTime, CorrectTrack);
		m_State = TimetableState::MovingToExit;
	}

	void JustLeft(WorldTime CurrentTime)
	{
		BD_ASSERT(m_State == TimetableState::MovingToExit);

		AddScoreForAction(LeaveTime, CurrentTime, true);
		m_State = TimetableState::Left;
	}

	void Update(float DeltaTime)
	{
		if (m_State == TimetableState::StoppedAtDestination)
			m_StoppingTime += DeltaTime;
	}

	TimetableState State() const
	{
		return m_State;
	}

	uint32_t Score() const
	{
		return m_AccumulatedScore;
	}

	WorldTime SpawnTime;
	WorldTime ArrivalTime;
	WorldTime DepartureTime;
	WorldTime LeaveTime;

	std::string SpawnLocation;
	std::string PreferredTrack;
	std::string LeaveLocation;

	float MinStopDuration = 0.0f;

private:
	float m_StoppingTime = 0.0f;

	uint32_t m_AccumulatedScore = 0;

	TimetableState m_State = TimetableState::NotSpawned;

	void AddScoreForAction(WorldTime TimetableTime, WorldTime ActualTime, bool CorrectTrack)
	{
		static constexpr uint32_t BaseScoreValue = 40;
		static constexpr uint32_t WrongTrackScoreValue = BaseScoreValue / 2;
		static constexpr float DelayScoreModifierPerMinute = 0.08f;
		static constexpr float MinScoreModifier = 0.4f;

		auto BaseScore = 0;
		if (CorrectTrack)
		{
			BaseScore = BaseScoreValue;
		}

		auto DelayInMinutes = (ActualTime > TimetableTime ? (ActualTime - TimetableTime).Minutes() : 0u);
		auto DelayScoreModifier = std::max(MinScoreModifier, 1.0f - DelayScoreModifierPerMinute * DelayInMinutes);

		auto AddedScore = static_cast<uint32_t>(BaseScore * DelayScoreModifier);
		BD_LOG_DEBUG("Adding {} points", AddedScore);
		m_AccumulatedScore += AddedScore;
	}
};
