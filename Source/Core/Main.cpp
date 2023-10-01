#include <chrono>

#include "Core/GameLoop.h"
#include "Core/Logger.h"

int main()
{
	GLogger = std::make_unique<Logger>(LogLevel::Info, "Files/log.txt", true);

	auto GameLoop = GameLoop::Create();
	if (!GameLoop)
	{
		BD_LOG_ERROR("Failed to initialize game loop");
		return 1;
	}

	auto ExitCode = GameLoop->Run();
	BD_LOG_INFO("Exiting the main() function with exit code {}", ExitCode);

	return ExitCode;
}
