#pragma once

#include <glm/glm.hpp>

#include "Layer/Layer.h"

class TrackLayer : public Layer
{
public:
	static std::unique_ptr<TrackLayer> Create();

	virtual bool OnMousePress(MouseButton::Button Button, const InputState& InputState, World& World) override;

	virtual bool OnMouseScroll(int32_t Offset, const InputState& InputState, World& World) override;

	virtual void Update(float DeltaTime, const InputState& InputState, World& World) override;

	virtual void Render(Renderer& Renderer, const World& World) const override;

private:
	static constexpr glm::vec3 s_PointColor = { 0.97f, 0.97f, 0.97f };
	static constexpr glm::vec3 s_TrackColor = { 0.4f, 0.4f, 0.4f };

	glm::vec2 m_CameraLocation = { 0.0f, 0.0f };
	float m_CameraScale = 1.0f;

	std::unordered_map<SignalState, std::unique_ptr<VectorIcon>> m_SignalIcons;

	TrackLayer() = default;

	float PixelsPerMeter() const;

	void RenderTrackTile(Renderer& Renderer, const World& World, const TrackTile& Tile) const;

	void RenderSignal(Renderer& Renderer, const Signal& Signal) const;

	glm::vec2 CursorPositionToWorldCoordinates(glm::ivec2 CursorPosition, glm::ivec2 CursorAreaBoundaries) const;

	glm::ivec2 WorldPositionToTileCoordinates(glm::vec2 WorldPosition) const;
};
