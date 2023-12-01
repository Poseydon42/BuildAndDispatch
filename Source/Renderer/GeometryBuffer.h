#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <span>

#include "Core/Assert.h"
#include "Renderer/Buffer.h"

#define VERTEX_DESCRIPTION_BEGIN(Class)                                                                  \
namespace VAOBindingsRegister {                                                                          \
	template<>                                                                                           \
	void RegisterVAOBindings<Class>(GeometryBuffer<Class>& Buffer, const Class* TemplateInstance) \
	{                                                                                                    \
		int NextIndex = 0;                                                                               \

#define VERTEX_DESCRIPTION_ELEMENT(Name)                                                                                     \
		Buffer.AddVAOBinding(                                                                                                \
			NextIndex++, sizeof(TemplateInstance->Name) / sizeof(float), sizeof(*TemplateInstance),                          \
			reinterpret_cast<const uint8_t*>(&TemplateInstance->Name) - reinterpret_cast<const uint8_t*>(TemplateInstance)); \

#define VERTEX_DESCRIPTION_END() \
	}                            \
}                                \

template<typename VertexType>
class GeometryBuffer;

namespace VAOBindingsRegister
{
	template<typename VertexType>
	void RegisterVAOBindings(GeometryBuffer<VertexType>& Buffer, const VertexType* TemplateVertex)
	{
		BD_ASSERT("Unknown vertex type" && false);
	}
}

template<typename VertexType>
class GeometryBuffer
{
public:
	~GeometryBuffer();

	static std::unique_ptr<GeometryBuffer> Create(size_t VertexCount, bool IsDynamic, const std::optional<std::span<const VertexType>>& InitialContents = std::nullopt);

	void AppendVertex(const VertexType& Vertex);

	void AppendVertices(std::span<const VertexType> Vertices);

	void Reset();

	void Flush();

	void Bind() const;

	size_t VertexCount() const;

private:
	std::unique_ptr<Buffer> m_VertexBuffer;
	GLuint m_VAO = 0;

	bool m_IsDynamic = false;

	VertexType* m_NextVertex = nullptr;
	VertexType* m_OnePastLastVertex = nullptr;

	size_t m_VertexCount = 0;

	GeometryBuffer(std::unique_ptr<Buffer> VertexBuffer, bool IsDynamic, size_t InitialVertexCount);

	void AddVAOBinding(uint32_t Index, uint32_t Size, uint32_t Stride, size_t Offset);
	
	friend void VAOBindingsRegister::RegisterVAOBindings(GeometryBuffer& Buffer, const VertexType* TemplateVertex);
};

template<typename VertexType>
GeometryBuffer<VertexType>::~GeometryBuffer()
{
	glDeleteVertexArrays(1, &m_VAO);
}

template<typename VertexType>
std::unique_ptr<GeometryBuffer<VertexType>> GeometryBuffer<VertexType>::Create(size_t VertexCount, bool IsDynamic, const std::optional<std::span<const VertexType>>& InitialContents)
{
	auto VertexBuffer = Buffer::Create(VertexCount * sizeof(VertexType), {}, IsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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

template<typename VertexType>
void GeometryBuffer<VertexType>::AppendVertex(const VertexType& Vertex)
{
	BD_ASSERT(m_NextVertex && m_NextVertex < m_OnePastLastVertex);
	*m_NextVertex++ = Vertex;
	m_VertexCount++;
}

template<typename VertexType>
void GeometryBuffer<VertexType>::AppendVertices(std::span<const VertexType> Vertices)
{
	for (const auto& Vertex : Vertices)
		AppendVertex(Vertex);
}

template<typename VertexType>
void GeometryBuffer<VertexType>::Reset()
{
	BD_ASSERT(m_IsDynamic);
	m_NextVertex = static_cast<VertexType*>(m_VertexBuffer->Map(false, true));
	m_OnePastLastVertex = m_NextVertex + (m_VertexBuffer->Size() / sizeof(VertexType));
	m_VertexCount = 0;
}

template<typename VertexType>
void GeometryBuffer<VertexType>::Flush()
{
	m_VertexBuffer->Unmap();
	m_NextVertex = nullptr;
	m_OnePastLastVertex = nullptr;
}

template<typename VertexType>
void GeometryBuffer<VertexType>::Bind() const
{
	glBindVertexArray(m_VAO);
}

template<typename VertexType>
size_t GeometryBuffer<VertexType>::VertexCount() const
{
	return m_VertexCount;
}

template<typename VertexType>
GeometryBuffer<VertexType>::GeometryBuffer(std::unique_ptr<Buffer> VertexBuffer, bool IsDynamic, size_t InitialVertexCount)
	: m_VertexBuffer(std::move(VertexBuffer))
	, m_IsDynamic(IsDynamic)
	, m_VertexCount(InitialVertexCount)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	m_VertexBuffer->Bind(GL_ARRAY_BUFFER);

	VAOBindingsRegister::RegisterVAOBindings<VertexType>(*this, nullptr);
}

template<typename VertexType>
void GeometryBuffer<VertexType>::AddVAOBinding(uint32_t Index, uint32_t Size, uint32_t Stride, size_t Offset)
{
	glVertexAttribPointer(Index, Size, GL_FLOAT, false, Stride, reinterpret_cast<const void*>(Offset));
	glEnableVertexAttribArray(Index);
}
