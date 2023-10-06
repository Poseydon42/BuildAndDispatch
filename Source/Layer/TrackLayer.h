#pragma once

#include <glm/glm.hpp>

#include "Layer/Layer.h"

class TrackLayer : public Layer
{
public:
	virtual bool OnMousePress(MouseButton Button, glm::ivec2 ScreenCursorPos, glm::vec2 WorldCursorPos, World& World) const override;

	virtual void Render(Renderer& Renderer, const World& World) const override;

private:
	static constexpr glm::vec3 s_PointColor = { 0.97f, 0.97f, 0.97f };
	static constexpr glm::vec3 s_TrackColor = { 0.4f, 0.4f, 0.4f };

	glm::vec2 m_CameraLocation = { 0.0f, 0.0f };
	float m_CameraScale = 1.0f;

	void RenderTrackTile(Renderer& Renderer, const World& World, const TrackTile& Tile) const;

	void RenderSignal(Renderer& Renderer, const Signal& Signal) const;
};