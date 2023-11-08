#pragma once

#include <memory>

#include "UI/Brush.h"
#include "UI/Widget.h"

class Panel : public Widget
{
public:
	static std::unique_ptr<Panel> Create(std::unique_ptr<Brush> Brush, glm::vec2 CustomPreferredSize = {});

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Render(RenderBuffer& Buffer) const override;

protected:
	Panel(std::unique_ptr<Brush> Brush, glm::vec2 CustomPreferredSize);

private:
	std::unique_ptr<Brush> m_Brush;
	glm::vec2 m_PreferredSize;
};
