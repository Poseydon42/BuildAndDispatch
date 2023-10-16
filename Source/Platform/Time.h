#pragma once

namespace Time
{
	using Point = uint64_t;

	Point Now();

	float Duration(Point Start, Point End);
}
