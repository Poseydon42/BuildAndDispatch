#pragma once

#include <cassert>

#include "Core/Logger.h"

namespace AssertInternal
{
	[[noreturn]] constexpr void NoReturnAssertDummy()
	{
		*static_cast<volatile uint8_t*>(nullptr) = 0;
		while (true);
	}
}

#define BD_ASSERT(condition) { if (!(condition)) { BD_LOG_FATAL("Assertion failed at {}:{}; condition {} is false", __FILE__, __LINE__, #condition); assert(false); ::AssertInternal::NoReturnAssertDummy(); } }
#define BD_UNREACHABLE() { BD_LOG_FATAL("Unreachable code was executed at {}:{}", __FILE__, __LINE__); assert(false); ::AssertInternal::NoReturnAssertDummy(); }
