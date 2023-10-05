#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <span>

#include "Renderer/Buffer.h"

struct Vertex
{
	glm::vec2 Position;
	glm::vec3 Color;
};

class GeometryBuffer
{
public:
	~GeometryBuffer();

	static std::unique_ptr<GeometryBuffer> Create(size_t VertexCount, bool IsDynamic, const std::optional<std::span<const Vertex>>& InitialContents = std::nullopt);

	void AppendVertex(const Vertex& Vertex);

	void AppendVertices(std::span<const Vertex> Vertices);

	void Reset();

	void Flush();

	void Bind() const;

	size_t VertexCount() const;

private:
	std::unique_ptr<Buffer> m_VertexBuffer;
	GLuint m_VAO = 0;

	bool m_IsDynamic = false;

	Vertex* m_NextVertex = nullptr;
	Vertex* m_OnePastLastVertex = nullptr;

	size_t m_VertexCount = 0;

	GeometryBuffer(std::unique_ptr<Buffer> VertexBuffer, bool IsDynamic, size_t InitialVertexCount);
};