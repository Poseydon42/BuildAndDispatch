#pragma once

#include <Windows.h>

#include "Platform/File.h"

class WindowsFile : public File
{
public:
	explicit WindowsFile(HANDLE File);

	virtual ~WindowsFile() override;

	virtual bool Read(uint8_t* Buffer, size_t Size) override;

	virtual bool Write(const uint8_t* Buffer, size_t Size) override;

	virtual size_t Size() const override;

	virtual void Seek(ptrdiff_t Offset) override;

	virtual void SetPosition(size_t Position) override;

	virtual size_t Tell() const override;

private:
	HANDLE m_File;
};