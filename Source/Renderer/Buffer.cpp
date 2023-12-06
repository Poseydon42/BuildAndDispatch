#include "Buffer.h"

Buffer::~Buffer()
{
	glDeleteBuffers(1, &m_Buffer);
}

std::unique_ptr<Buffer> Buffer::Create(size_t Size, std::optional<void*> Data, GLenum Usage)
{
	GLuint Buffer;
	glGenBuffers(1, &Buffer);

	glBindBuffer(GL_COPY_WRITE_BUFFER, Buffer);
	glBufferData(GL_COPY_WRITE_BUFFER, Size, Data.value_or(nullptr), Usage);

	return std::unique_ptr<class Buffer>(new class Buffer(Buffer, Size));
}

void* Buffer::Map(bool IsReading, bool IsWriting) const
{
	if (m_MappedAddress.has_value())
		return *m_MappedAddress;

	Bind(GL_COPY_WRITE_BUFFER);

	GLenum Access = 0;
	if (IsReading)
		Access = GL_READ_ONLY;
	if (IsWriting)
		Access = GL_WRITE_ONLY;
	if (IsReading && IsWriting)
		Access = GL_READ_WRITE;
	m_MappedAddress = glMapBuffer(GL_COPY_WRITE_BUFFER, Access);
	return *m_MappedAddress;
}

void Buffer::Unmap() const
{
	Bind(GL_COPY_WRITE_BUFFER);
	glUnmapBuffer(GL_COPY_WRITE_BUFFER);
	m_MappedAddress = std::nullopt;
}

void Buffer::Bind(GLenum Target) const
{
	glBindBuffer(Target, m_Buffer);
}

size_t Buffer::Size() const
{
	return m_Size;
}

Buffer::Buffer(GLuint Buffer, size_t Size)
	: m_Buffer(Buffer)
	, m_Size(Size)
{
}
