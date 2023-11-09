#include "VerticalContainer.h"

std::unique_ptr<VerticalContainer> VerticalContainer::Create()
{
	return std::unique_ptr<VerticalContainer>(new VerticalContainer);
}

glm::vec2 VerticalContainer::ComputePreferredSize() const
{
	glm::vec2 Result = {};
	bool First = true;
	ForEachChild([&](const Widget& Widget)
	{
		auto PreferredSize = Widget.ComputePreferredSize();
		if (PreferredSize.y <= 0.0f)
			return;

		Result.x = std::max(Result.x, PreferredSize.x);
		Result.y += PreferredSize.y;

		if (!First)
		{
			Result.y += Spacing();
			First = false;
		}
	});
	return Result;
}

void VerticalContainer::Layout()
{
	if (ChildCount() == 0)
		return;

	// First pass - calculate the total preferred height and the total vertical stretch ratio

	float TotalPreferredHeight = 0.0f;
	float TotalVerticalStretchRatio = 0.00000001f; // Avoid division by zero if no children want to be stretched
	ForEachChild([&](const Widget& Child)
	{
		TotalPreferredHeight += Child.ComputePreferredSize().y;
		TotalVerticalStretchRatio += Child.VerticalStretchRatio();
	});
	TotalPreferredHeight += Spacing() * (ChildCount() - 1);

	float AvailableStretch = BoundingBox().Height() - TotalPreferredHeight;
	if (AvailableStretch < 0.0f)
		AvailableStretch = 0.0f;

	// Second pass - position each child

	float Y = BoundingBox().Top();
	ForEachChild([&](Widget& Child)
	{
		auto AbsoluteLeftMargin = Child.LeftMargin().GetAbsoluteValue(BoundingBox().Width());
		auto AbsoluteRightMargin = Child.RightMargin().GetAbsoluteValue(BoundingBox().Width());

		if (AbsoluteLeftMargin + AbsoluteRightMargin > BoundingBox().Width())
		{
			AbsoluteRightMargin = BoundingBox().Width() - AbsoluteLeftMargin;
		}

		auto PreferredHeight = Child.ComputePreferredSize().y;
		auto StretchHeight = Child.VerticalStretchRatio() / TotalVerticalStretchRatio * AvailableStretch;
		auto TotalHeight = PreferredHeight + StretchHeight;

		Child.BoundingBox().Left() = BoundingBox().Left() + AbsoluteLeftMargin;
		Child.BoundingBox().Right() = BoundingBox().Right() - AbsoluteRightMargin;

		Child.BoundingBox().Top() = Y;
		Child.BoundingBox().Bottom() = Y - TotalHeight;
		Y -= (TotalHeight + Spacing());

		Child.Layout();
	});
}
