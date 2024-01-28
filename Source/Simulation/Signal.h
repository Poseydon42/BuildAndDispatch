#pragma once

#include <glm/vec2.hpp>

enum class SignalState
{
	Danger = 0,
	Clear,
	_Count
};

enum class SignalKind
{
	Manual,
	Automatic,
};

struct SignalLocation
{
	glm::ivec2 FromTile = {};
	glm::ivec2 ToTile = {};

	constexpr bool operator==(const SignalLocation& Rhs)
	{
		return FromTile == Rhs.FromTile && ToTile == Rhs.ToTile;
	}
};

struct Signal
{
	SignalLocation Location = {};

	SignalState State = SignalState::Danger;

	SignalKind Kind;
};

constexpr bool CanTrainPassSignal(SignalState Signal)
{
	return Signal == SignalState::Clear;
}
