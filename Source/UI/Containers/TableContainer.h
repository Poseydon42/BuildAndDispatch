#pragma once

#include "UI/Container.h"
#include "UI/Widgets/Label.h"

class TableContainer : public Container
{
public:
	static std::unique_ptr<TableContainer> Create(std::span<std::pair<std::string, float>> Columns, uint32_t ColumnHeadingSize, std::shared_ptr<Font> ColumnHeadingFont, std::optional<size_t> MaxRowsShown);

	virtual bool OnScroll(int32_t Delta) override;

	virtual void Layout() override;

	virtual void Render(RenderBuffer& Buffer) const override;

	float CellPadding() const { return m_CellPadding; }
	float& CellPadding() { return m_CellPadding; }

	float LineThickness() const { return m_LineThickness; }
	float& LineThickness() { return m_LineThickness; }

	const glm::vec4& LineColor() const { return m_LineColor; }
	glm::vec4& LineColor() { return m_LineColor; }

private:
	struct Column
	{
		std::string Name;
		float ScalingFactor;
		std::unique_ptr<Label> Label;
	};
	mutable std::vector<Column> m_Columns;
	mutable std::vector<float> m_RowHeights;

	float m_CellPadding = 0.0f;
	float m_LineThickness = 0.0f;
	glm::vec4 m_LineColor = glm::vec4(0.0f);

	std::optional<size_t> m_MaxRowsShown;
	size_t m_IndexOfFirstShownRow = 0;

	uint32_t m_ColumnHeadingSize = 0;
	std::shared_ptr<Font> m_ColumnHeadingFont;

	TableContainer(
		std::span<std::pair<std::string, float>> Columns,
		uint32_t ColumnHeadingSize, std::shared_ptr<Font> ColumnHeadingFont,
		std::optional<size_t> MaxRowsShown);

	virtual glm::vec2 ComputeContentPreferredSize() const override;

	float ColumnWidth(size_t Index) const;
};
