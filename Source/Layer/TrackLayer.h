#pragma once

#include <glm/glm.hpp>

#include "Layer/Layer.h"

class TrackLayer : public Layer
{
public:
	virtual void OnMousePress(MouseButton Button, int32_t CursorX, int32_t CursorY, World& World) const override;

	virtual void Render(Renderer& Renderer, const World& World) const override;

private:
	static constexpr glm::vec3 s_PointColor = { 0.97f, 0.97f, 0.97f };
	static constexpr glm::vec3 s_TrackColor = { 0.4f, 0.4f, 0.4f };

	void RenderTrackTile(Renderer& Renderer, const World& World, const TrackTile& Tile) const;

	void RenderSignal(Renderer& Renderer, const Signal& Signal) const;
};