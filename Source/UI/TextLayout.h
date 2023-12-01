#pragma once

#include <string_view>

#include "UI/Font.h"

namespace TextLayout
{
	template<typename CallbackFuncType>
	bool Layout(std::string_view Text, uint32_t FontSize, const Font& Font, CallbackFuncType&& Callback)
	{
		float Cursor = 0.0f;
		for (auto Char : Text)
		{
			auto MaybeGlyph = Font.GetGlyph(Char, FontSize);
			if (!MaybeGlyph.has_value())
				return false;

			auto Glyph = MaybeGlyph.value();

			Rect2D Rect;
			Rect.Min = glm::vec2(Cursor, Font.Descender(FontSize)) + Glyph.Offset;
			Rect.Max = Rect.Min + Glyph.Dimensions;

			Callback(Rect, Glyph.TextureCoordinates);

			Cursor += Glyph.Advance;
		}
		return true;
	}

	inline glm::vec2 Measure(std::string_view Text, uint32_t FontSize, const Font& Font)
	{
		glm::vec2 Result = { 0.0f, Font.LineHeight(FontSize) };
		BD_ASSERT(Layout(Text, FontSize, Font, [&Result](Rect2D Rect, Rect2D)
		{
			Result.x = Rect.Right();
		}));
		return Result;
	}
}
