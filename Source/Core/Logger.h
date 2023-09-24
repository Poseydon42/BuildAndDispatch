#pragma once

#include <format>
#include <memory>
#include <optional>
#include <string>

#include "Platform/File.h"

enum class LogLevel
{
	Debug = 0,
	Info,
	Warning,
	Error,
	Fatal,
};

class Logger
{
public:
	Logger(LogLevel MinLevel, std::optional<std::string_view> FilePath, bool LogToStdout);

	void Log(LogLevel Level, std::string_view Message);

private:
	LogLevel m_MinLevel;
	std::unique_ptr<File> m_LogFile;
	bool m_LogToStdout;

	std::string ApplyFormatting(LogLevel Level, std::string_view Message) const;
};

extern std::unique_ptr<Logger> GLogger;

#define BD_LOG_DEBUG(message, ...) { if (GLogger) { auto __Message__ = std::format(message, __VA_ARGS__); GLogger->Log(LogLevel::Debug, __Message__); } }
#define BD_LOG_INFO(message, ...) { if (GLogger) { auto __Message__ = std::format(message, __VA_ARGS__); GLogger->Log(LogLevel::Info, __Message__); } }
#define BD_LOG_WARNING(message, ...) { if (GLogger) { auto __Message__ = std::format(message, __VA_ARGS__); GLogger->Log(LogLevel::Warning, __Message__); } }
#define BD_LOG_ERROR(message, ...) { if (GLogger) { auto __Message__ = std::format(message, __VA_ARGS__); GLogger->Log(LogLevel::Error, __Message__); } }
#define BD_LOG_FATAL(message, ...) { if (GLogger) { auto __Message__ = std::format(message, __VA_ARGS__); GLogger->Log(LogLevel::Fatal, __Message__); } }

