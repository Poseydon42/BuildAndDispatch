#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Renderer/GeometryBuffer.h"

class VectorIcon
{
public:
	const GeometryBuffer& GeometryBuffer() const;

	glm::vec2 MinCorner() const;
	glm::vec2 MaxCorner() const;

	bool IsPointInside(glm::vec2 Point, const glm::mat4& TransformationMatrix) const;

private:
	std::unique_ptr<class GeometryBuffer> m_GeometryBuffer;

	std::vector<glm::vec2> m_Vertices;

	VectorIcon(std::vector<glm::vec2> Vertices, std::unique_ptr<class GeometryBuffer> GeometryBuffer);

	friend class VectorIconBuilder;
};

class VectorIconBuilder
{
public:
	void AddCircle(glm::vec2 Center, float Radius, glm::vec4 Color);

	std::unique_ptr<VectorIcon> Build() const;

private:
	std::vector<Vertex> m_Vertices;
};
