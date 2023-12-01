#include "Font.h"

#include <filesystem>
#include <nlohmann/json.hpp>

#include "Core/Logger.h"
#include "Platform/File.h"

std::shared_ptr<Font> Font::Load(const std::string& Path)
{
	using namespace nlohmann;

#define RETURN_WITH_ERROR(Message, ...)  { BD_LOG_ERROR(Message, __VA_ARGS__); return nullptr; }

	auto MaybeFontDesc = FileSystem::ReadFileAsString(Path);
	if (!MaybeFontDesc)
		RETURN_WITH_ERROR("Font {} could not be loaded", Path);
	auto FontDesc = json::parse(*MaybeFontDesc);


	// ================ 'ATLAS' PROPERTY ================
	if (!FontDesc.contains("atlas") || FontDesc["atlas"].type() != detail::value_t::object)
		RETURN_WITH_ERROR("Font {} does not provide valid atlas description", Path);
	const auto& AtlasDescription = FontDesc["atlas"];

	if (!AtlasDescription.contains("width") || AtlasDescription["width"].type() != detail::value_t::number_unsigned ||
		!AtlasDescription.contains("height") || AtlasDescription["height"].type() != detail::value_t::number_unsigned)
	{
		RETURN_WITH_ERROR("Font {} does provide atlas texture width and/or height", Path);
	}
	glm::vec2 AtlasDimensions = { AtlasDescription["width"].get<float>(), AtlasDescription["height"].get<float>() };

	if (!AtlasDescription.contains("size") || AtlasDescription["size"].type() != detail::value_t::number_float)
		RETURN_WITH_ERROR("Font {} does not provide font size value", Path);
	float PixelsPerEm = AtlasDescription["size"].get<float>();


	// ================ 'METRICS' PROPERTY ================
	if (!FontDesc.contains("metrics") || FontDesc["metrics"].type() != detail::value_t::object)
		RETURN_WITH_ERROR("Font {} does not provide font metrics", Path);
	const auto& Metrics = FontDesc["metrics"];

	if (!Metrics.contains("ascender")  || Metrics["ascender"].type()  != detail::value_t::number_float ||
		!Metrics.contains("descender") || Metrics["descender"].type() != detail::value_t::number_float)
	{
		BD_LOG_ERROR("Font {} does not provide ascender and/or descender value", Path);
	}
	float Ascender = Metrics["ascender"].get<float>();
	float Descender = std::abs(Metrics["descender"].get<float>());

	// ================ 'GLYPHS' PROPERTY ================
	if (!FontDesc.contains("glyphs") || FontDesc["glyphs"].type() != detail::value_t::array)
		RETURN_WITH_ERROR("Font {} does not provide any glyphs", Path);
	const auto& JSONGlyphs = FontDesc["glyphs"];

	std::unordered_map<IndexType, Glyph> Glyphs;
	for (const auto& GlyphDescription : JSONGlyphs)
	{
		if (!GlyphDescription.contains("unicode") || GlyphDescription["unicode"].type() != detail::value_t::number_unsigned ||
			GlyphDescription["unicode"].get<unsigned>() > std::numeric_limits<IndexType>::max())
		{
			RETURN_WITH_ERROR("Font {} does not provide unicode value for one of its characters or provided value is invalid", Path);
		}
		auto Unicode = static_cast<IndexType>(GlyphDescription["unicode"].get<unsigned>());

		if (!GlyphDescription.contains("advance") || GlyphDescription["advance"].type() != detail::value_t::number_float)
			RETURN_WITH_ERROR("Font {} does not provide 'advance' value for character '{}'", Path, Unicode);
		auto Advance = GlyphDescription["advance"].get<float>();

		// FIXME: some characters such as 'whitespace' (0x20) are not provided with texture coordinates,
		// but we should still load them to get their 'advance' values
		if (!GlyphDescription.contains("atlasBounds") || !GlyphDescription.contains("planeBounds"))
			continue;
			
		// Load character's texture coordinates in the atlas and its boundary box because at this point we know for sure
		// that the character is not just empty space

		const auto& AtlasBounds = GlyphDescription["atlasBounds"];
		if (!AtlasBounds.contains("left")   || AtlasBounds["left"].type()   != detail::value_t::number_float ||
			!AtlasBounds.contains("right")  || AtlasBounds["right"].type()  != detail::value_t::number_float || 
			!AtlasBounds.contains("top")    || AtlasBounds["top"].type()    != detail::value_t::number_float || 
			!AtlasBounds.contains("bottom") || AtlasBounds["bottom"].type() != detail::value_t::number_float)
		{
			RETURN_WITH_ERROR("Font {} does not provide valid atlas texture coordinates for character '{}'", Path, Unicode);
		}

		Rect2D TextureCoordinates = {
			.Min = glm::vec2{
				AtlasBounds["left"].get<float>(),
				AtlasBounds["bottom"].get<float>()
			} / AtlasDimensions,
			.Max = glm::vec2{
				AtlasBounds["right"].get<float>(),
				AtlasBounds["top"].get<float>()
			} / AtlasDimensions
		};

		const auto& PlaneBounds = GlyphDescription["planeBounds"];
		if (!PlaneBounds.contains("left")   || PlaneBounds["left"].type()   != detail::value_t::number_float ||
			!PlaneBounds.contains("right")  || PlaneBounds["right"].type()  != detail::value_t::number_float ||
			!PlaneBounds.contains("top")    || PlaneBounds["top"].type()    != detail::value_t::number_float ||
			!PlaneBounds.contains("bottom") || PlaneBounds["bottom"].type() != detail::value_t::number_float)
		{
			RETURN_WITH_ERROR("Font {} does not provide valid offset and/or dimensions for character '{}'", Path, Unicode);
		}

		glm::vec2 Offset = {
			PlaneBounds["left"].get<float>(),
			PlaneBounds["bottom"].get<float>()
		};
		glm::vec2 Dimensions = {
			PlaneBounds["right"].get<float>() - Offset.x,
			PlaneBounds["top"].get<float>() - Offset.y,
		};

		Glyphs.emplace(Unicode, Glyph{ Offset, Dimensions, Advance, TextureCoordinates });
	}

	auto AtlasFilePath = std::filesystem::path(Path).replace_extension(".png").string();
	auto Atlas = Texture::Load(AtlasFilePath);
	if (!Atlas)
		RETURN_WITH_ERROR("Could not load atlas texture from file {} for font {}", AtlasFilePath, Path);

	return std::shared_ptr<Font>(new Font(std::move(Atlas), std::move(Glyphs), PixelsPerEm, Ascender, Descender));
}

std::optional<Glyph> Font::GetGlyph(IndexType Character, uint32_t FontSize) const
{
	if (!m_Glyphs.contains(Character))
		return std::nullopt;

	auto Glyph = m_Glyphs.at(Character);

	Glyph.Offset     *= FontSize;
	Glyph.Dimensions *= FontSize;
	Glyph.Advance    *= FontSize;
	return Glyph;
}

const Texture& Font::AtlasTexture() const
{
	return *m_AtlasTexture;
}

float Font::Ascender(uint32_t FontSize) const
{
	return m_Ascender * FontSize;
}

float Font::Descender(uint32_t FontSize) const
{
	return m_Descender * FontSize;
}

float Font::LineHeight(uint32_t FontSize) const
{
	return (m_Ascender + m_Descender) * FontSize;
}

float Font::ScreenPxRange(uint32_t FontSize) const
{
	constexpr float BasePxRange = 8.0f; // FIXME: load this from font file
	return FontSize / m_PixelsPerEm * BasePxRange;
}
