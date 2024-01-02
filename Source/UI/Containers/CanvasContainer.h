#pragma once

#include "UI/Container.h"

class CanvasContainer : public Container
{
public:
	static std::unique_ptr<CanvasContainer> Create();

	virtual void Layout() override;

private:
	virtual glm::vec2 ComputeContentPreferredSize() const override;
};
