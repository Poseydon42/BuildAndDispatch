#include "GeometryBuffer.h"

#include "Core/Assert.h"

GeometryBuffer::~GeometryBuffer()
{
	glDeleteVertexArrays(1, &m_VAO);
}

std::unique_ptr<GeometryBuffer> GeometryBuffer::Create(size_t VertexCount, bool IsDynamic, const std::optional<std::span<const Vertex>>& InitialContents)
{
	auto VertexBuffer = Buffer::Create(VertexCount * sizeof(Vertex), {}, IsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	if (!VertexBuffer)
		return nullptr;

	BD_ASSERT(IsDynamic || InitialContents.has_value());

	size_t InitialVertexCount = 0;
	if (InitialContents.has_value())
	{
		BD_ASSERT(InitialContents->size() == VertexCount);

		auto* Memory = VertexBuffer->Map(false, true);
		memcpy(Memory, InitialContents->data(), InitialContents->size_bytes());
		VertexBuffer->Unmap();

		InitialVertexCount = InitialContents->size();
	}

	return std::unique_ptr<GeometryBuffer>(new GeometryBuffer(std::move(VertexBuffer), IsDynamic, InitialVertexCount));
}

void GeometryBuffer::AppendVertex(const Vertex& Vertex)
{
	BD_ASSERT(m_NextVertex && m_NextVertex < m_OnePastLastVertex);
	*m_NextVertex++ = Vertex;
	m_VertexCount++;
}

void GeometryBuffer::AppendVertices(std::span<const Vertex> Vertices)
{
	for (const auto& Vertex : Vertices)
		AppendVertex(Vertex);
}

void GeometryBuffer::Reset()
{
	BD_ASSERT(m_IsDynamic);
	m_NextVertex = static_cast<Vertex*>(m_VertexBuffer->Map(false, true));
	m_OnePastLastVertex = m_NextVertex + (m_VertexBuffer->Size() / sizeof(Vertex));
	m_VertexCount = 0;
}

void GeometryBuffer::Flush()
{
	m_VertexBuffer->Unmap();
	m_NextVertex = nullptr;
	m_OnePastLastVertex = nullptr;
}

void GeometryBuffer::Bind() const
{
	glBindVertexArray(m_VAO);
}

size_t GeometryBuffer::VertexCount() const
{
	return m_VertexCount;
}

GeometryBuffer::GeometryBuffer(std::unique_ptr<Buffer> VertexBuffer, bool IsDynamic, size_t InitialVertexCount)
	: m_VertexBuffer(std::move(VertexBuffer))
	, m_IsDynamic(IsDynamic)
	, m_VertexCount(InitialVertexCount)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	m_VertexBuffer->Bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), &static_cast<Vertex*>(nullptr)->Position);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), &static_cast<Vertex*>(nullptr)->Color);
	glEnableVertexAttribArray(1);
}
