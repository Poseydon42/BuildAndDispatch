#include "Logger.h"

#include <chrono>
#include <format>
#include <iostream>

std::unique_ptr<Logger> GLogger = nullptr;

static constexpr const char* LogLevelToString(LogLevel Level)
{
	switch (Level)
	{
	case LogLevel::Debug:
		return "Debug";
	case LogLevel::Info:
		return "Info";
	case LogLevel::Warning:
		return "Warning";
	case LogLevel::Error:
		return "Error";
	case LogLevel::Fatal:
		return "Fatal";
	default:
		return "";
	}
}

Logger::Logger(LogLevel MinLevel, std::optional<std::string_view> FilePath, bool LogToStdout)
	: m_MinLevel(MinLevel)
	, m_LogFile(FilePath.has_value() ? FileSystem::Open(FilePath.value(), FileSystem::OpenMode::CreateNew, FileSystem::AccessMode::ReadWrite) : nullptr)
	, m_LogToStdout(LogToStdout)
{
}

std::string Logger::ApplyFormatting(LogLevel Level, std::string_view Message) const
{
	auto Time = std::chrono::system_clock::now();
	auto LocalTime = std::chrono::floor<std::chrono::seconds>(std::chrono::current_zone()->to_local(Time));
	return std::format("[{:%d-%m-%Y %H:%M:%S}][{}] {}\n", LocalTime, LogLevelToString(Level), Message);
}

void Logger::Log(LogLevel Level, std::string_view Message)
{
	auto FormattedMessage = ApplyFormatting(Level, Message);
	if (m_LogFile)
		m_LogFile->Write(reinterpret_cast<const uint8_t*>(FormattedMessage.data()), FormattedMessage.length());
	if (m_LogToStdout)
	{
		if (Level >= LogLevel::Error)
			std::cerr << FormattedMessage;
		else
			std::cout << FormattedMessage;
	}
}
