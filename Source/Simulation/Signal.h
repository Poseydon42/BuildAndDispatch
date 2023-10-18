#pragma once

#include <glm/vec2.hpp>

enum class SignalState
{
	Danger = 0,
	Clear,
	_Count
};

struct Signal
{
	glm::ivec2 From;
	glm::ivec2 To;

	SignalState State = SignalState::Danger;
};

constexpr bool CanTrainPassSignal(SignalState Signal)
{
	return Signal == SignalState::Clear;
}
