#include "VectorIcon.h"

#include <glm/gtc/constants.hpp>

const GeometryBuffer& VectorIcon::GeometryBuffer() const
{
	return *m_GeometryBuffer;
}

VectorIcon::VectorIcon(std::unique_ptr<::GeometryBuffer> GeometryBuffer)
	: m_GeometryBuffer(std::move(GeometryBuffer))
{
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

	return std::unique_ptr<VectorIcon>(new VectorIcon(std::move(GeometryBuffer)));
}
