#pragma once

#include <cstdint>

/*
 * Represents an in-game time. The time is stored as a floating point number of seconds since midnight,
 * but to a user of the class the minimum unit is a second. All comparisons are performed in seconds with
 * truncation, so e.g. 1:59:59.9999999 is considered EXACTLY equal to 1:59:59.
 */
class WorldTime
{
public:
	constexpr uint32_t Seconds() const { return static_cast<uint32_t>(m_Seconds) % 60; }
	constexpr uint32_t Minutes() const { return static_cast<uint32_t>(m_Seconds) / 60 % 60; }
	constexpr uint32_t Hours() const { return static_cast<uint32_t>(m_Seconds) / 3600 % 24; }

	constexpr bool operator==(const WorldTime& Other) const { return Seconds() == Other.Seconds() && Minutes() == Other.Minutes() && Hours() == Other.Hours(); }
	constexpr bool operator!=(const WorldTime& Other) const { return !(*this == Other); }

	constexpr bool operator<(const WorldTime& Other) const { return static_cast<uint32_t>(m_Seconds) < static_cast<uint32_t>(Other.m_Seconds); }
	constexpr bool operator<=(const WorldTime& Other) const { return static_cast<uint32_t>(m_Seconds) <= static_cast<uint32_t>(Other.m_Seconds); }
	constexpr bool operator>(const WorldTime& Other) const { return static_cast<uint32_t>(m_Seconds) > static_cast<uint32_t>(Other.m_Seconds); }
	constexpr bool operator>=(const WorldTime& Other) const { return static_cast<uint32_t>(m_Seconds) >= static_cast<uint32_t>(Other.m_Seconds); }

	constexpr WorldTime operator+(const WorldTime& Other) const { return FromSeconds(m_Seconds + Other.m_Seconds); }
	constexpr WorldTime operator-(const WorldTime& Other) const { return FromSeconds(m_Seconds - Other.m_Seconds); }
	WorldTime& operator+=(const WorldTime& Other) { m_Seconds += Other.m_Seconds; return *this; }
	WorldTime& operator-=(const WorldTime& Other) { m_Seconds -= Other.m_Seconds; return *this; }

	constexpr WorldTime operator+(float Rhs) const { return FromSeconds(m_Seconds + Rhs); }
	constexpr WorldTime operator-(float Rhs) const { return FromSeconds(m_Seconds - Rhs); }
	WorldTime& operator+=(float Rhs) { m_Seconds += Rhs; return *this; }
	WorldTime& operator-=(float Rhs) { m_Seconds -= Rhs; return *this; }

	static constexpr WorldTime FromSeconds(float Seconds)
	{
		WorldTime Result;
		Result.m_Seconds = Seconds;
		return Result;
	}

	static constexpr WorldTime FromHoursMinutesSeconds(uint32_t Hours, uint32_t Minutes, uint32_t Seconds)
	{
		return FromSeconds(static_cast<float>(Hours) * 3600.0f + static_cast<float>(Minutes) * 60.0f + static_cast<float>(Seconds));
	}

private:
	float m_Seconds = 0.0f;
};
