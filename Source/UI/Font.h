#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "Core/Rect2D.h"
#include "Renderer/Texture.h"

struct Glyph
{
	glm::vec2 Offset;
	glm::vec2 Dimensions;
	float Advance;

	Rect2D TextureCoordinates;
};

class Font
{
public:
	static std::shared_ptr<Font> Load(const std::string& Path);

	using IndexType = unsigned char;

	std::optional<Glyph> GetGlyph(IndexType Character, uint32_t FontSize) const;

	const Texture& AtlasTexture() const;

	float Ascender(uint32_t FontSize) const;
	float Descender(uint32_t FontSize) const;
	float LineHeight(uint32_t FontSize) const;

	float WhitespaceAdvance(uint32_t FontSize) const;

	float ScreenPxRange(uint32_t FontSize) const;

private:
	std::unique_ptr<Texture> m_AtlasTexture;
	std::unordered_map<IndexType, Glyph> m_Glyphs;

	float m_PixelsPerEm = 0.0f;
	float m_Ascender = 0.0f;
	float m_Descender = 0.0f;

	float m_WhitespaceAdvance = 0.0f;

	Font(std::unique_ptr<Texture> AtlasTexture, std::unordered_map<IndexType, Glyph> Glyphs, float PixelsPerEm, float Ascender, float Descender, float WhitespaceAdvance)
		: m_AtlasTexture(std::move(AtlasTexture))
		, m_Glyphs(std::move(Glyphs))
		, m_PixelsPerEm(PixelsPerEm)
		, m_Ascender(Ascender)
		, m_Descender(Descender)
		, m_WhitespaceAdvance(WhitespaceAdvance)
	{}
};
