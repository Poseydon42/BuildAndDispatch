#pragma once

#include "UI/Container.h"

class CanvasContainer : public Container
{
public:
	static std::unique_ptr<CanvasContainer> Create();

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Layout() override;
};
