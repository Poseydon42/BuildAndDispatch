#pragma once

#include <glad/glad.h>
#include <memory>
#include <optional>

class Buffer
{
public:
	~Buffer();

	static std::unique_ptr<Buffer> Create(size_t Size, std::optional<void*> Data, GLenum Usage);

	void* Map(bool IsReading, bool IsWriting) const;

	void Unmap() const;

	void Bind(GLenum Target) const;

	size_t Size() const;

private:
	GLuint m_Buffer;
	size_t m_Size;

	Buffer(GLuint Buffer, size_t Size);
};
