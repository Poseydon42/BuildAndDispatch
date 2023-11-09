#pragma once

#include "UI/Container.h"
#include "UI/Widget.h"

/**
 * \brief A vertical container positions its children one under the other, with the first child at the top.
 *
 * A spacing (in pixels) can be specified to separate the children. Horizontal stretching factor is ignored,
 * whereas vertical stretching is used to determine how much space each child will take. If a child does not
 * wish to be stretched, their vertical stretching factor should be set to 0. Vertical margins are ignored.
 *
 * Horizontal position is determined from the child's left and right margins, however the priority is given
 * to give the child its preferred width first, therefore the margins might be scaled down to respect this.
 * Therefore, to e.g. position children horizontally in the center, the left and right margins should be set
 * to relative values of 0.5.
 */
class VerticalContainer : public Container
{
public:
	static std::unique_ptr<VerticalContainer> Create();

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Layout() override;;

	float& Spacing() { return m_Spacing; }
	const float& Spacing() const { return m_Spacing; }

private:
	VerticalContainer() = default;

	float m_Spacing = 0.0f;
};
