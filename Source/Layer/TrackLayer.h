#pragma once

#include <glm/glm.hpp>
#include <optional>

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
	glm::vec2 m_CameraLocation = { 0.0f, 0.0f };
	float m_CameraScale = 1.0f;

	std::optional<SignalLocation> m_RouteStartSignalLocation = std::nullopt;

	std::unordered_map<TrackState, glm::vec3> m_TrackColors;
	std::unordered_map<SignalState, std::unique_ptr<VectorIcon>> m_SignalIcons;

	TrackLayer();

	float PixelsPerMeter() const;

	void HandleSignalClick(World& World, const Signal& Signal);

	void RenderTrackTile(Renderer& Renderer, const World& World, const TrackTile& Tile) const;

	void RenderSignal(Renderer& Renderer, const Signal& Signal) const;

	void RenderTrain(Renderer& Renderer, const Train& Train) const;

	glm::vec2 CursorPositionToWorldCoordinates(glm::ivec2 CursorPosition, glm::ivec2 CursorAreaBoundaries) const;

	glm::ivec2 WorldPositionToTileCoordinates(glm::vec2 WorldPosition) const;
};
