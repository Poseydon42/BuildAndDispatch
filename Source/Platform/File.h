#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

class File
{
public:
	virtual ~File() = default;

	virtual bool Read(uint8_t* Buffer, size_t Size) = 0;

	virtual bool Write(const uint8_t* Buffer, size_t Size) = 0;

	virtual size_t Size() const = 0;

	virtual void Seek(ptrdiff_t Offset) = 0;

	virtual void SetPosition(size_t Position) = 0;

	virtual size_t Tell() const = 0;
};

class FileSystem
{
public:
	enum class OpenMode
	{
		CreateNew,
		OpenExisting,
		OpenExistingOverwrite,
	};

	enum class AccessMode
	{
		Read,
		ReadWrite,
	};

	static std::unique_ptr<File> Open(std::string_view Path, OpenMode OpenMode, AccessMode AccessMode);

	static std::optional<std::vector<uint8_t>> ReadFileAsBytes(std::string_view Path);

	static std::optional<std::string> ReadFileAsString(std::string_view Path);
};
