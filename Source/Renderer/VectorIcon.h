#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string_view>
#include <vector>

#include "Renderer/GeometryBuffer.h"

class VectorIcon
{
public:
	static std::unique_ptr<VectorIcon> CreateFromString(std::string_view String);

	static std::unique_ptr<VectorIcon> LoadFromFile(std::string_view Path);

	const GeometryBuffer& GeometryBuffer() const;

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

	void AddLine(glm::vec2 From, glm::vec2 To, float Thickness, glm::vec4 Color);

	/**
	 * Adds a closed polygon constructed with the given vertices, in order.
	 * NOTE: the polygon must be convex.
	 */
	void AddPolygon(const std::vector<glm::vec2>& Vertices, glm::vec4 Color);

	std::unique_ptr<VectorIcon> Build() const;

private:
	std::vector<Vertex> m_Vertices;

	void AddTriangle(glm::vec2 V1, glm::vec2 V2, glm::vec2 V3, glm::vec4 Color);
};
