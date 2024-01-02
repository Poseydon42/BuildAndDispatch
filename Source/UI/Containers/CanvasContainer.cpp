#include "CanvasContainer.h"

std::unique_ptr<CanvasContainer> CanvasContainer::Create()
{
	return std::unique_ptr<CanvasContainer>(new CanvasContainer);
}

glm::vec2 CanvasContainer::ComputeContentPreferredSize() const
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

		auto LeftMargin = Child.Style().LeftMargin.GetAbsoluteValue(ContentBoundingBox().Width());
		auto RightMargin = Child.Style().RightMargin.GetAbsoluteValue(ContentBoundingBox().Width());
		auto TopMargin = Child.Style().TopMargin.GetAbsoluteValue(ContentBoundingBox().Height());
		auto BottomMargin = Child.Style().BottomMargin.GetAbsoluteValue(ContentBoundingBox().Height());

		if (LeftMargin + ChildSize.x + RightMargin > ContentBoundingBox().Width())
		{
			auto Scale = (ContentBoundingBox().Width() - ChildSize.x) / (LeftMargin + RightMargin + 0.00001f);
			LeftMargin *= Scale;
			RightMargin *= Scale;
		}

		if (TopMargin + ChildSize.y + BottomMargin > ContentBoundingBox().Height())
		{
			auto Scale = (ContentBoundingBox().Height() - ChildSize.y) / (TopMargin + BottomMargin + 0.00001f);
			TopMargin *= Scale;
			BottomMargin *= Scale;
		}

		Child.BoundingBox().Left() = ContentBoundingBox().Left() + LeftMargin;
		Child.BoundingBox().Right() = ContentBoundingBox().Right() - RightMargin;

		Child.BoundingBox().Top() = ContentBoundingBox().Top() - TopMargin;
		Child.BoundingBox().Bottom() = ContentBoundingBox().Bottom() + BottomMargin;

		Child.Layout();
	});
}
