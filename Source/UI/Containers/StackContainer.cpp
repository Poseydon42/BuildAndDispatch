#include "StackContainer.h"

static uint32_t MajorCoordinateIndexFromDirection(StackContainer::Direction Direction)
{
	switch (Direction)
	{
	case StackContainer::Direction::Horizontal:
		return 0; // x
	case StackContainer::Direction::Vertical:
		return 1; // y;
	default:
		BD_UNREACHABLE();
	}
}

static uint32_t MinorCoordinateIndexFromDirection(StackContainer::Direction Direction)
{
	return 1 - MajorCoordinateIndexFromDirection(Direction);
}

std::unique_ptr<StackContainer> StackContainer::Create(Direction Direction)
{
	return std::unique_ptr<StackContainer>(new StackContainer(Direction));
}

glm::vec2 StackContainer::ComputePreferredSize() const
{
	auto MajorCoordinateIndex = MajorCoordinateIndexFromDirection(m_Direction);
	auto MinorCoordinateIndex = MinorCoordinateIndexFromDirection(m_Direction);

	glm::vec2 Result = {};
	bool First = true;
	ForEachChild([&](const Widget& Widget)
	{
		auto PreferredSize = Widget.ComputePreferredSize();
		if (PreferredSize[MajorCoordinateIndex] <= 0.0f)
			return;

		Result[MinorCoordinateIndex] = std::max(Result[MinorCoordinateIndex], PreferredSize[MinorCoordinateIndex]);
		Result[MajorCoordinateIndex] += PreferredSize[MajorCoordinateIndex];

		if (First)
			First = false;
		else
			Result[MajorCoordinateIndex] += Spacing();
	});
	return Result;
}

void StackContainer::Layout()
{
	auto MajorCoordinateIndex = MajorCoordinateIndexFromDirection(m_Direction);
	auto MinorCoordinateIndex = MinorCoordinateIndexFromDirection(m_Direction);

	if (ChildCount() == 0)
		return;

	// First pass - calculate the total preferred length and the total stretch ratio

	float TotalPreferredLength = 0.0f;
	float TotalStretchRatio = 0.00000001f; // Avoid division by zero if no children want to be stretched
	ForEachChild([&](const Widget& Child)
	{
		TotalPreferredLength += Child.ComputePreferredSize()[MajorCoordinateIndex];
		TotalStretchRatio += m_Direction == Direction::Horizontal ? Child.HorizontalStretchRatio() : Child.VerticalStretchRatio();
	});
	TotalPreferredLength += Spacing() * (ChildCount() - 1);

	float AvailableStretch = BoundingBox().Dimensions()[MajorCoordinateIndex] - TotalPreferredLength;
	if (AvailableStretch < 0.0f)
		AvailableStretch = 0.0f;

	// Second pass - position each child

	bool First = true;
	float NextCoordinate = m_Direction == Direction::Horizontal ? BoundingBox().Left() : BoundingBox().Top();
	ForEachChild([&](Widget& Child)
	{
		if (First)
			First = false;
		else
			NextCoordinate += m_Direction == Direction::Horizontal ? Spacing() : -Spacing();

		auto BoundingBoxPerpendicularLength = m_Direction == Direction::Horizontal ? BoundingBox().Height() : BoundingBox().Width();
		auto AbsoluteLeftOrTopMargin = (m_Direction == Direction::Horizontal ? Child.TopMargin() : Child.LeftMargin()).GetAbsoluteValue(BoundingBoxPerpendicularLength);
		auto AbsoluteRightOrBottomMargin = (m_Direction == Direction::Horizontal ? Child.BottomMargin() : Child.RightMargin()).GetAbsoluteValue(BoundingBoxPerpendicularLength);

		if (AbsoluteLeftOrTopMargin + AbsoluteRightOrBottomMargin > BoundingBoxPerpendicularLength - Child.ComputePreferredSize()[MinorCoordinateIndex])
		{
			float Scale = (BoundingBoxPerpendicularLength - Child.ComputePreferredSize()[MinorCoordinateIndex]) / (AbsoluteLeftOrTopMargin + AbsoluteRightOrBottomMargin + 0.0000001f);
			AbsoluteLeftOrTopMargin *= Scale;
			AbsoluteRightOrBottomMargin *= Scale;
		}

		auto PreferredLength = Child.ComputePreferredSize()[MajorCoordinateIndex];
		auto StretchLength = (m_Direction == Direction::Horizontal ? Child.HorizontalStretchRatio() : Child.VerticalStretchRatio()) / TotalStretchRatio * AvailableStretch;
		auto TotalLength = PreferredLength + StretchLength;

		switch (m_Direction)
		{
		case Direction::Horizontal:
			Child.BoundingBox().Left() = NextCoordinate;
			Child.BoundingBox().Right() = NextCoordinate + TotalLength;

			Child.BoundingBox().Top() = BoundingBox().Top() - AbsoluteLeftOrTopMargin;
			Child.BoundingBox().Bottom() = BoundingBox().Bottom() + AbsoluteRightOrBottomMargin;

			NextCoordinate += TotalLength;

			break;
		case Direction::Vertical:
			Child.BoundingBox().Left() = BoundingBox().Left() + AbsoluteLeftOrTopMargin;
			Child.BoundingBox().Right() = BoundingBox().Right() - AbsoluteRightOrBottomMargin;

			Child.BoundingBox().Top() = NextCoordinate;
			Child.BoundingBox().Bottom() = NextCoordinate - TotalLength;

			NextCoordinate -= TotalLength;

			break;
		default:
			BD_UNREACHABLE();
		}
		Child.Layout();
	});
}
