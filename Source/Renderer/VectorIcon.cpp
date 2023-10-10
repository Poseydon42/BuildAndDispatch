#include "VectorIcon.h"

#include <algorithm>
#include <glm/gtc/constants.hpp>

#include "Core/Assert.h"

static float Sign(glm::vec2 P, glm::vec2 A, glm::vec2 B)
{
	return (P.x - B.x) * (A.y - B.y) - (A.x - B.x) * (P.y - B.y);
}

static bool IsInsideTriangle(glm::vec2 Point, glm::vec2 V1, glm::vec2 V2, glm::vec2 V3)
{
	float S1 = Sign(Point, V1, V2);
	float S2 = Sign(Point, V2, V3);
	float S3 = Sign(Point, V3, V1);

	return (S1 > 0.0f && S2 > 0.0f && S3 > 0.0f) || (S1 < 0.0f && S2 < 0.0f && S3 < 0.0f);
}

const GeometryBuffer& VectorIcon::GeometryBuffer() const
{
	return *m_GeometryBuffer;
}

bool VectorIcon::IsPointInside(glm::vec2 Point, const glm::mat4& TransformationMatrix) const
{
	for (size_t VertexIndex = 0; VertexIndex < m_Vertices.size(); VertexIndex += 3)
	{
		auto V1 = TransformationMatrix * glm::vec4(m_Vertices[VertexIndex], 0.0f, 1.0f);
		auto V2 = TransformationMatrix * glm::vec4(m_Vertices[VertexIndex + 1], 0.0f, 1.0f);
		auto V3 = TransformationMatrix * glm::vec4(m_Vertices[VertexIndex + 2], 0.0f, 1.0f);

		if (IsInsideTriangle(Point, V1, V2, V3))
			return true;
	}

	return false;
}

VectorIcon::VectorIcon(std::vector<glm::vec2> Vertices, std::unique_ptr<::GeometryBuffer> GeometryBuffer)
	: m_GeometryBuffer(std::move(GeometryBuffer))
	, m_Vertices(std::move(Vertices))
{
	BD_ASSERT(m_Vertices.size() % 3 == 0);
}

void VectorIconBuilder::AddCircle(glm::vec2 Center, float Radius, glm::vec4 Color)
{
	static constexpr int32_t SectorCount = 128;
	static constexpr auto DeltaAngle = glm::two_pi<float>() / SectorCount;

	for (int32_t SectorIndex = 0; SectorIndex < SectorCount; SectorIndex++)
	{
		float LeftRadiusAngle = DeltaAngle * SectorIndex;
		float RightRadiusAngle = DeltaAngle * (SectorIndex + 1);

		Vertex CenterVertex = { .Position = Center, .Color = Color };
		Vertex LeftVertex = { .Position = Center + Radius * glm::vec2(glm::cos(LeftRadiusAngle), glm::sin(LeftRadiusAngle)), .Color = Color };
		Vertex RightVertex = { .Position = Center + Radius * glm::vec2(glm::cos(RightRadiusAngle), glm::sin(RightRadiusAngle)), .Color = Color };

		m_Vertices.push_back(CenterVertex);
		m_Vertices.push_back(LeftVertex);
		m_Vertices.push_back(RightVertex);
	}
}

std::unique_ptr<VectorIcon> VectorIconBuilder::Build() const
{
	auto GeometryBuffer = GeometryBuffer::Create(m_Vertices.size(), false, m_Vertices);
	if (!GeometryBuffer)
		return nullptr;

	std::vector<glm::vec2> Vertices(m_Vertices.size());
	std::ranges::transform(m_Vertices, Vertices.begin(), [](const Vertex& Vertex) { return Vertex.Position; });
	return std::unique_ptr<VectorIcon>(new VectorIcon(Vertices, std::move(GeometryBuffer)));
}
