#include "File.h"

#include "Core/Assert.h"

WindowsFile::~WindowsFile()
{
	CloseHandle(m_File);
}

bool WindowsFile::Read(uint8_t* Buffer, size_t Size)
{
	DWORD BytesRead;
	if (!ReadFile(m_File, Buffer, static_cast<DWORD>(Size), &BytesRead, nullptr))
		return false;
	return BytesRead == Size;
}

bool WindowsFile::Write(const uint8_t* Buffer, size_t Size)
{
	// FIXME: this is unlikely to work when Size > 2^31 or 2^32
	DWORD BytesWritten;
	if (!WriteFile(m_File, Buffer, static_cast<DWORD>(Size), &BytesWritten, nullptr))
		return false;
	return Size == BytesWritten;
}

size_t WindowsFile::Size() const
{
	LARGE_INTEGER Result;
	if (!GetFileSizeEx(m_File, &Result))
		return 0;
	return Result.QuadPart;
}

void WindowsFile::Seek(ptrdiff_t Offset)
{
	SetFilePointerEx(m_File, { .QuadPart = Offset }, nullptr, FILE_CURRENT);
}

void WindowsFile::SetPosition(size_t Position)
{
	SetFilePointerEx(m_File, { .QuadPart = static_cast<LONGLONG>(Position) }, nullptr, FILE_BEGIN);
}

size_t WindowsFile::Tell() const
{
	LARGE_INTEGER Result;
	SetFilePointerEx(m_File, { .QuadPart = 0 }, &Result, FILE_CURRENT);
	return Result.QuadPart;
}

WindowsFile::WindowsFile(HANDLE File)
	: m_File(File)
{
}

std::unique_ptr<File> FileSystem::Open(std::string_view Path, OpenMode OpenMode, AccessMode AccessMode)
{
	DWORD Access = 0;
	switch (AccessMode)
	{
	case AccessMode::Read:
		Access = GENERIC_READ;
		break;
	case AccessMode::ReadWrite:
		Access = GENERIC_READ | GENERIC_WRITE;
		break;
	default:
		BD_UNREACHABLE();
	}

	DWORD CreationDisposition = 0;
	switch (OpenMode)
	{
	case OpenMode::CreateNew:
		CreationDisposition = CREATE_ALWAYS;
		break;
	case OpenMode::OpenExisting:
		CreationDisposition = OPEN_EXISTING;
		break;
	case OpenMode::OpenExistingOverwrite:
		CreationDisposition = TRUNCATE_EXISTING;
		break;
	default:
		BD_UNREACHABLE();
	}

	auto Handle = CreateFileA(Path.data(), Access, 0, nullptr, CreationDisposition, 0, INVALID_HANDLE_VALUE);

	if (Handle != INVALID_HANDLE_VALUE)
	{
		return std::make_unique<WindowsFile>(Handle);
	}
	return nullptr;
}

std::optional<std::vector<uint8_t>> FileSystem::ReadFileAsBytes(std::string_view Path)
{
	auto File = Open(Path, OpenMode::OpenExisting, AccessMode::Read);
	if (!File)
		return std::nullopt;

	auto Size = File->Size();
	std::vector<uint8_t> Result(Size);

	if (!File->Read(Result.data(), Result.size()))
		return std::nullopt;

	return Result;
}

std::optional<std::string> FileSystem::ReadFileAsString(std::string_view Path)
{
	auto Bytes = ReadFileAsBytes(Path);
	if (!Bytes.has_value())
		return std::nullopt;

	return std::string(reinterpret_cast<const char*>(Bytes->data()), Bytes->size());
}
