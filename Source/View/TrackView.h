#pragma once

#include <glm/glm.hpp>

#include "View/View.h"

class TrackView : public View
{
public:
	virtual void Render(Renderer& Renderer, const World& World) const override;

private:
	static constexpr glm::vec3 s_PointColor = { 0.97f, 0.97f, 0.97f };
	static constexpr glm::vec3 s_TrackColor = { 0.4f, 0.4f, 0.4f };

	void RenderTrackTile(Renderer& Renderer, const World& World, const TrackTile& Tile) const;
};