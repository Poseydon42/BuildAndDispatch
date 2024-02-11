#include "TableContainer.h"

#include <numeric>

std::unique_ptr<TableContainer> TableContainer::Create(
	std::span<std::pair<std::string, float>> Columns,
	uint32_t ColumnHeadingSize, std::shared_ptr<Font> ColumnHeadingFont,
	std::optional<size_t> MaxRowsShown)
{
	return std::unique_ptr<TableContainer>(new TableContainer(std::move(Columns), ColumnHeadingSize, std::move(ColumnHeadingFont), MaxRowsShown));
}

bool TableContainer::OnScroll(int32_t Delta)
{
	auto NumRows = (ChildCount() + m_Columns.size() - 1) / m_Columns.size();
	auto RowsShown = m_MaxRowsShown.value_or(1);
	m_IndexOfFirstShownRow = std::max(static_cast<int32_t>(m_IndexOfFirstShownRow) - Delta, 0);
	m_IndexOfFirstShownRow = std::min(m_IndexOfFirstShownRow, NumRows - RowsShown);
	return true;
}

glm::vec2 TableContainer::ComputeContentPreferredSize() const
{
	BD_ASSERT(!m_Columns.empty());
	BD_ASSERT(ChildCount() % m_Columns.size() == 0);

	auto NumRows = ChildCount() / m_Columns.size();
	if (m_MaxRowsShown.has_value())
		NumRows = std::min(NumRows, m_MaxRowsShown.value());
	m_RowHeights.resize(NumRows);

	float ScalingFactorSum = std::accumulate<std::vector<Column>::const_iterator, float>(m_Columns.begin(), m_Columns.end(), 0.0, [](const float& Value, const Column& Column) { return Value + Column.ScalingFactor; });

	size_t ChildIndex = 0;
	float MinWidth = 0.0f;
	ForEachChild([&](const Widget& Child)
	{
		auto Column = ChildIndex % m_Columns.size();
		auto Row = ChildIndex / m_Columns.size();
		ChildIndex++;

		auto ChildSize = Child.ComputePreferredSize();
		ChildSize += 2.0f * m_CellPadding;
		MinWidth = std::max(MinWidth, ChildSize.x * ScalingFactorSum / m_Columns[Column].ScalingFactor);

		// NOTE: we need these exit conditions to stay here so that the rows that are currently not shown can still
		// affect the width of the columns, and, hence, of the table itself. Otherwise it is possible that the width
		// of the table will change as we scroll up and down.
		if (Row < m_IndexOfFirstShownRow)
			return;
		if (m_MaxRowsShown.has_value() && m_MaxRowsShown.value() <= Row - m_IndexOfFirstShownRow)
			return;

		if (Column == 0)
		{
			m_RowHeights[Row - m_IndexOfFirstShownRow] = 0.0f;
		}
		m_RowHeights[Row - m_IndexOfFirstShownRow] = std::max(m_RowHeights[Row - m_IndexOfFirstShownRow], ChildSize.y);
	});
	for (const auto& Column : m_Columns)
	{
		auto LabelWidth = Column.Label->ComputePreferredSize().x;
		MinWidth = std::max(MinWidth, LabelWidth * ScalingFactorSum / Column.ScalingFactor);
	}

	glm::vec2 Result = {
		MinWidth + (m_Columns.size() + 1) * m_LineThickness,
		std::accumulate(m_RowHeights.begin(), m_RowHeights.end(), m_LineThickness * (NumRows + 2)) + m_Columns[0].Label->ComputePreferredSize().y
	};

	return Result;
}

void TableContainer::Layout()
{
	BD_ASSERT(ChildCount() % m_Columns.size() == 0);

	auto NextLabelCoords = glm::vec2(ContentBoundingBox().Left() + m_LineThickness, ContentBoundingBox().Top() - m_LineThickness);
	auto HeadingLabelHeight = m_Columns[0].Label->ComputePreferredSize().y;
	for (size_t ColumnIndex = 0; ColumnIndex < m_Columns.size(); ColumnIndex++)
	{
		auto& Column = m_Columns[ColumnIndex];
		auto& Label = Column.Label;

		Label->BoundingBox().Left() = NextLabelCoords.x;
		Label->BoundingBox().Right() = NextLabelCoords.x + ColumnWidth(ColumnIndex);
		Label->BoundingBox().Top() = NextLabelCoords.y;
		Label->BoundingBox().Bottom() = NextLabelCoords.y - HeadingLabelHeight;

		Label->Layout();

		NextLabelCoords.x += ColumnWidth(ColumnIndex) + m_LineThickness;
	}

	size_t ChildIndex = 0;
	auto NextChildCoords = glm::vec2(ContentBoundingBox().Left() + m_LineThickness, ContentBoundingBox().Top() - 2 * m_LineThickness - HeadingLabelHeight);
	ForEachChild([&](Widget& Child)
	{
		auto Column = ChildIndex % m_Columns.size();
		auto Row = ChildIndex / m_Columns.size();
		ChildIndex++;

		if (Row < m_IndexOfFirstShownRow)
			return;
		if (m_MaxRowsShown.has_value() && m_MaxRowsShown.value() <= Row - m_IndexOfFirstShownRow)
			return;

		Child.BoundingBox().Left() = NextChildCoords.x;
		Child.BoundingBox().Right() = NextChildCoords.x + ColumnWidth(Column);
		Child.BoundingBox().Top() = NextChildCoords.y;
		Child.BoundingBox().Bottom() = NextChildCoords.y - m_RowHeights[Row - m_IndexOfFirstShownRow];

		NextChildCoords.x += ColumnWidth(Column) + m_LineThickness;
		if (Column == m_Columns.size() - 1)
		{
			NextChildCoords.x = ContentBoundingBox().Left() + m_LineThickness;
			NextChildCoords.y -= m_RowHeights[Row - m_IndexOfFirstShownRow] + m_LineThickness;
		}

		Child.Layout();
	});
}

void TableContainer::Render(RenderBuffer& Buffer) const
{
	Widget::Render(Buffer);

	size_t ChildIndex = 0;
	ForEachChild([&](const Widget& Child)
	{
		auto Row = ChildIndex / m_Columns.size();
		ChildIndex++;

		if (Row < m_IndexOfFirstShownRow)
			return;
		if (m_MaxRowsShown.has_value() && m_MaxRowsShown.value() <= Row - m_IndexOfFirstShownRow)
			return;

		Child.Render(Buffer);
	});

	auto Left = ContentBoundingBox().Left();
	auto Right = ContentBoundingBox().Right();
	auto Top = ContentBoundingBox().Top();
	auto Bottom = ContentBoundingBox().Bottom();

	auto NextHorizontalLineY = Bottom + m_LineThickness / 2.0f;
	for (auto RowHeight : m_RowHeights)
	{
		Buffer.Line(glm::vec2(Left, NextHorizontalLineY), glm::vec2(Right, NextHorizontalLineY), m_LineThickness, m_LineColor);
		NextHorizontalLineY += m_LineThickness + RowHeight;
	}
	Buffer.Line(glm::vec2(Left, NextHorizontalLineY), glm::vec2(Right, NextHorizontalLineY), m_LineThickness, m_LineColor);
	
	Buffer.Line(glm::vec2(Left, Top - m_LineThickness / 2.0f), glm::vec2(Right, Top - m_LineThickness / 2.0f), m_LineThickness, m_LineColor);

	auto NextVerticalLineX = Left + m_LineThickness / 2.0f;
	for (size_t Column = 0; Column < m_Columns.size(); Column++)
	{
		m_Columns[Column].Label->Render(Buffer);
		Buffer.Line(glm::vec2(NextVerticalLineX, Top), glm::vec2(NextVerticalLineX, Bottom), m_LineThickness, m_LineColor);
		NextVerticalLineX += m_LineThickness + ColumnWidth(Column);
	}
	Buffer.Line(glm::vec2(NextVerticalLineX, Top), glm::vec2(NextVerticalLineX, Bottom), m_LineThickness, m_LineColor);
}

TableContainer::TableContainer(
	std::span<std::pair<std::string, float>> Columns,
	uint32_t ColumnHeadingSize, std::shared_ptr<Font> ColumnHeadingFont,
	std::optional<size_t> MaxRowsShown)
	: m_MaxRowsShown(MaxRowsShown)
	, m_ColumnHeadingSize(ColumnHeadingSize)
	, m_ColumnHeadingFont(std::move(ColumnHeadingFont))
{
	for (auto& [ColumnName, ColumnScalingFactor] : Columns)
	{
		m_Columns.emplace_back(ColumnName, ColumnScalingFactor, Label::Create(ColumnName, m_ColumnHeadingSize, m_ColumnHeadingFont));
	}
}

float TableContainer::ColumnWidth(size_t Index) const
{
	float ScalingFactorSum = std::accumulate<std::vector<Column>::const_iterator, float>(m_Columns.begin(), m_Columns.end(), 0.0, [](const float& Value, const Column& Column) { return Value + Column.ScalingFactor; });
	auto UsableWidth = ContentBoundingBox().Width() - (m_Columns.size() + 1) * m_LineThickness;
	return UsableWidth * m_Columns[Index].ScalingFactor / ScalingFactorSum;
}
