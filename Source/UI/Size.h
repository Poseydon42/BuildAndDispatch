#pragma once

struct Size1D
{
	enum class Type
	{
		Absolute,
		Relative,
	};

	Type Type = Type::Absolute;
	float Value = 0.0f;

	static constexpr Size1D Absolute(float Value)
	{
		return { Type::Absolute, Value };
	}

	static constexpr Size1D Relative(float Value)
	{
		return { Type::Relative, Value };
	}

	constexpr float GetAbsoluteValue(float ParentSize) const
	{
		return Type == Type::Absolute ? Value : ParentSize * Value;
	}
};
