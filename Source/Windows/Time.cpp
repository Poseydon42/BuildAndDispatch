#include <Windows.h>

#include "Core/Assert.h"
#include "Platform/Time.h"

namespace WindowsTime
{
	bool GInitialized = false;
	LARGE_INTEGER GFrequency;

	void Init()
	{
		BD_ASSERT(QueryPerformanceFrequency(&GFrequency));
		GInitialized = true;
	}
}

namespace Time
{
	Point Now()
	{
		if (!WindowsTime::GInitialized)
			WindowsTime::Init();

		LARGE_INTEGER Result;
		BD_ASSERT(QueryPerformanceCounter(&Result));
		return Result.QuadPart;
	}

	float Duration(Point Start, Point End)
	{
		BD_ASSERT(WindowsTime::GInitialized);
		return static_cast<float>(End - Start) / static_cast<float>(WindowsTime::GFrequency.QuadPart);
	}
}
