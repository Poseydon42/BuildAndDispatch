#pragma once

#include "UI/Container.h"
#include "UI/Widget.h"

class VerticalContainer : public Container
{
public:
	static std::unique_ptr<VerticalContainer> Create();

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Layout() override;;

private:
	VerticalContainer() = default;
};
