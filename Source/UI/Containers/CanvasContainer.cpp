#include "CanvasContainer.h"

std::unique_ptr<CanvasContainer> CanvasContainer::Create()
{
	return std::unique_ptr<CanvasContainer>(new CanvasContainer);
}

glm::vec2 CanvasContainer::ComputePreferredSize() const
{
	glm::vec2 Result = {};
	ForEachChild([&Result](const Widget& Widget)
	{
		Result = glm::max(Result, Widget.ComputePreferredSize());
	});
	return Result;
}

void CanvasContainer::Layout()
{
	ForEachChild([this](Widget& Child)
	{
		auto ChildSize = Child.ComputePreferredSize();

		auto LeftMargin = Child.LeftMargin().GetAbsoluteValue(BoundingBox().Width());
		auto RightMargin = Child.RightMargin().GetAbsoluteValue(BoundingBox().Width());
		auto TopMargin = Child.TopMargin().GetAbsoluteValue(BoundingBox().Height());
		auto BottomMargin = Child.BottomMargin().GetAbsoluteValue(BoundingBox().Height());

		if (LeftMargin + ChildSize.x + RightMargin > BoundingBox().Width())
		{
			auto Scale = (BoundingBox().Width() - ChildSize.x) / (LeftMargin + RightMargin + 0.00001f);
			LeftMargin *= Scale;
			RightMargin *= Scale;
		}

		if (TopMargin + ChildSize.y + BottomMargin > BoundingBox().Height())
		{
			auto Scale = (BoundingBox().Height() - ChildSize.y) / (TopMargin + BottomMargin + 0.00001f);
			TopMargin *= Scale;
			BottomMargin *= Scale;
		}

		Child.BoundingBox().Left() = BoundingBox().Left() + LeftMargin;
		Child.BoundingBox().Right() = BoundingBox().Right() - RightMargin;

		Child.BoundingBox().Top() = BoundingBox().Top() - TopMargin;
		Child.BoundingBox().Bottom() = BoundingBox().Bottom() + BottomMargin;

		Child.Layout();
	});
}
