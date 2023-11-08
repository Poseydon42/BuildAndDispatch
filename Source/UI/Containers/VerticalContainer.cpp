#include "VerticalContainer.h"

std::unique_ptr<VerticalContainer> VerticalContainer::Create()
{
	return std::unique_ptr<VerticalContainer>(new VerticalContainer);
}

glm::vec2 VerticalContainer::ComputePreferredSize() const
{
	glm::vec2 Result = {};
	ForEachChild([&Result](const Widget& Widget)
	{
		auto PreferredSize = Widget.ComputePreferredSize();
		Result.x = std::max(Result.x, PreferredSize.x);
		Result.y += PreferredSize.y;
	});
	return Result;
}

void VerticalContainer::Layout()
{
	float Y = BoundingBox().Top();
	ForEachChild([&](Widget& Child)
	{
		auto PreferredSize = Child.ComputePreferredSize();

		Child.BoundingBox().Left() = BoundingBox().Left();
		Child.BoundingBox().Right() = BoundingBox().Right();

		Child.BoundingBox().Top() = Y;
		Child.BoundingBox().Bottom() = Y - PreferredSize.y;

		Child.Layout();

		Y -= PreferredSize.y;
	});
}
