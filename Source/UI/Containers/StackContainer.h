#pragma once

#include "UI/Container.h"
#include "UI/Widget.h"

/**
 * \brief A stack container positions its children one after the other, either vertically or horizontally,
 * with the first child at the top or the left
 *
 * A spacing (in pixels) can be specified to separate the children. Stretching factor perpendicular to container's
 * direction is ignored, whereas stretching factor in container's direction is used to determine how much space
 * each child will take. If a child does not wish to be stretched, their stretching factor should be set to 0.
 * Margins in the container's direction are ignored.
 */
class StackContainer : public Container
{
public:
	enum class Direction
	{
		Vertical,
		Horizontal
	};

	static std::unique_ptr<StackContainer> Create(Direction Direction);

	virtual void Layout() override;;

	float& Spacing() { return m_Spacing; }
	const float& Spacing() const { return m_Spacing; }

private:
	explicit StackContainer(Direction Direction)
		: m_Direction(Direction)
	{
	}

	Direction m_Direction;
	float m_Spacing = 0.0f;

	virtual glm::vec2 ComputeContentPreferredSize() const override;
};
