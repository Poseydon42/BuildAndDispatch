#include "VectorIcon.h"

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <nlohmann/json.hpp>

#include "Core/Assert.h"
#include "Platform/File.h"

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

static std::optional<float> ParseNumber(std::string_view Name, const nlohmann::json& Parent)
{
	if (!Parent.contains(Name) || !Parent[Name].is_number())
		return std::nullopt;

	return Parent[Name];
}

template<unsigned Size>
requires (Size >= 2 && Size <= 4)
static std::optional<glm::vec<Size, float>> ParseVector(std::string_view Name, const nlohmann::json& Parent)
{
	if (!Parent.contains(Name))
		return std::nullopt;

	const auto& Array = Parent[Name];
	if (!Array.is_array() || Array.size() != Size)
		return std::nullopt;

	std::array<float, Size> Result = {};
	for (size_t Index = 0; Index < Size; Index++)
	{
		const auto& Value = Array[Index];
		if (!Value.is_number())
			return std::nullopt;

		Result[Index] = Value;
	}

	if constexpr (Size == 2)
		return glm::vec2(Result[0], Result[1]);
	else if constexpr (Size == 3)
		return glm::vec3(Result[0], Result[1], Result[2]);
	else if constexpr (Size == 4)
		return glm::vec4(Result[0], Result[1], Result[2], Result[3]);
	else
		return std::nullopt;
}

static bool ParseCircle(const nlohmann::json& Node, VectorIconBuilder& Builder)
{
	BD_ASSERT(Node.contains("type") && Node["type"].is_string() && Node["type"] == "circle");

	auto MaybeCenter = ParseVector<2>("center", Node);
	if (!MaybeCenter.has_value())
	{
		BD_LOG_ERROR("Failed to parse circle node: no valid center provided");
		BD_LOG_INFO("JSON string: \n{}", Node.dump());
		return false;
	}
	auto Center = MaybeCenter.value();

	auto MaybeRadius = ParseNumber("radius", Node);
	if (!MaybeRadius.has_value())
	{
		BD_LOG_ERROR("Failed to parse circle node: no valid radius provided");
		BD_LOG_INFO("JSON string: \n{}", Node.dump());
		return false;
	}
	auto Radius = MaybeRadius.value();

	glm::vec4 Color = {};
	auto MaybeColorWithAlpha = ParseVector<4>("color", Node);
	if (MaybeColorWithAlpha.has_value())
	{
		Color = MaybeColorWithAlpha.value();
	}
	else
	{
		auto MaybeColorWithoutAlpha = ParseVector<3>("color", Node);
		if (MaybeColorWithoutAlpha.has_value())
		{
			Color = { MaybeColorWithoutAlpha.value(), 1.0f };
		}
		else
		{
			BD_LOG_ERROR("Failed to parse circle node: no valid color provided");
			BD_LOG_INFO("JSON string: \n{}", Node.dump());
			return false;
		}
	}

	Builder.AddCircle(Center, Radius, Color);
	return true;
}

static bool ParseLine(const nlohmann::json& Node, VectorIconBuilder& Builder)
{
	BD_ASSERT(Node.contains("type") && Node["type"].is_string() && Node["type"] == "line");

	auto MaybeFrom = ParseVector<2>("from", Node);
	if (!MaybeFrom.has_value())
	{
		BD_LOG_ERROR("Failed to parse line node: no valid starting point provided");
		BD_LOG_INFO("JSON string: \n{}", Node.dump());
		return false;
	}
	auto From = MaybeFrom.value();

	auto MaybeTo = ParseVector<2>("to", Node);
	if (!MaybeTo.has_value())
	{
		BD_LOG_ERROR("Failed to parse line node: no valid ending point provided");
		BD_LOG_INFO("JSON string: \n{}", Node.dump());
		return false;
	}
	auto To = MaybeTo.value();

	auto MaybeThickness = ParseNumber("thickness", Node);
	if (!MaybeThickness.has_value())
	{
		BD_LOG_ERROR("Failed to parse line node: no valid thickness provided");
		BD_LOG_INFO("JSON string: \n{}", Node.dump());
		return false;
	}
	auto Thickness = MaybeThickness.value();

	glm::vec4 Color = {};
	auto MaybeColorWithAlpha = ParseVector<4>("color", Node);
	if (MaybeColorWithAlpha.has_value())
	{
		Color = MaybeColorWithAlpha.value();
	}
	else
	{
		auto MaybeColorWithoutAlpha = ParseVector<3>("color", Node);
		if (MaybeColorWithoutAlpha.has_value())
		{
			Color = { MaybeColorWithoutAlpha.value(), 1.0f };
		}
		else
		{
			BD_LOG_ERROR("Failed to parse line node: no valid color provided");
			BD_LOG_INFO("JSON string: \n{}", Node.dump());
			return false;
		}
	}

	Builder.AddLine(From, To, Thickness, Color);
	return true;
}

static bool ParseNode(const nlohmann::json& Node, VectorIconBuilder& Builder)
{
	using namespace nlohmann;
	if (Node.type() != detail::value_t::object)
	{
		BD_LOG_ERROR("Failed to parse node description: not an object");
		BD_LOG_INFO("JSON string: \n{}", Node.dump());
		return false;
	}

	if (!Node.contains("type") || Node["type"].type() != detail::value_t::string)
	{
		BD_LOG_ERROR("Failed to parse node description: no node type provided");
		BD_LOG_INFO("JSON string: \n{}", Node.dump());
		return false;
	}

	auto Type = Node["type"].get<std::string>();
	if (Type == "circle")
	{
		return ParseCircle(Node, Builder);
	}
	else if (Type == "line")
	{
		return ParseLine(Node, Builder);
	}
	else
	{
		BD_LOG_ERROR("Failed to parse node description: unknown node type '{}'", Type);
		return false;
	}
}

std::unique_ptr<VectorIcon> VectorIcon::CreateFromString(std::string_view String)
{
	using namespace nlohmann;

	auto Root = json::parse(String);
	if (!Root.contains("nodes") || Root["nodes"].type() != detail::value_t::array)
	{
		BD_LOG_ERROR("Failed to create VectorIcon from string: no nodes array");
		BD_LOG_DEBUG("JSON string: \n{}", String);
		return nullptr;
	}

	VectorIconBuilder Builder;

	const auto& Nodes = Root["nodes"];
	for (const auto& Node : Nodes)
	{
		if (!ParseNode(Node, Builder))
			return nullptr;
	}

	return Builder.Build();
}

std::unique_ptr<VectorIcon> VectorIcon::LoadFromFile(std::string_view Path)
{
	auto MaybeContents = FileSystem::ReadFileAsString(Path);
	if (!MaybeContents.has_value())
		return nullptr;

	auto String = MaybeContents.value();
	return CreateFromString(String);
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

		auto V1 = Center;
		auto V2 = Center + Radius * glm::vec2(glm::cos(LeftRadiusAngle), glm::sin(LeftRadiusAngle));
		auto V3 = Center + Radius * glm::vec2(glm::cos(RightRadiusAngle), glm::sin(RightRadiusAngle));

		AddTriangle(V1, V2, V3, Color);
	}
}

void VectorIconBuilder::AddLine(glm::vec2 From, glm::vec2 To, float Thickness, glm::vec4 Color)
{
	auto Direction = To - From;
	auto Perpendicular = glm::normalize(glm::vec2(-Direction.y, Direction.x));
	auto HalfThickness = 0.5f * Thickness;

	auto V1 = From + Perpendicular * HalfThickness;
	auto V2 = From - Perpendicular * HalfThickness;
	auto V3 = To + Perpendicular * HalfThickness;
	auto V4 = To - Perpendicular * HalfThickness;

	AddTriangle(V1, V2, V3, Color);
	AddTriangle(V2, V4, V3, Color);
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

void VectorIconBuilder::AddTriangle(glm::vec2 V1, glm::vec2 V2, glm::vec2 V3, glm::vec4 Color)
{
	m_Vertices.emplace_back(V1, Color);
	m_Vertices.emplace_back(V2, Color);
	m_Vertices.emplace_back(V3, Color);
}
