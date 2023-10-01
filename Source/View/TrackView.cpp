#include "TrackView.h"

#include <algorithm>

void TrackView::Render(Renderer& Renderer, const World& World) const
{
	std::ranges::for_each(World.TrackTiles(), [&](const auto& TrackPiece) { RenderTrackTile(Renderer, World, TrackPiece); });
	std::ranges::for_each(World.Signals(), [&](const auto& Signal) { RenderSignal(Renderer, Signal); });
}

void TrackView::RenderTrackTile(Renderer& Renderer, const World& World, const TrackTile& Tile) const
{
	auto Color = s_TrackColor;
	if (World.IsPoint(Tile.Tile.x, Tile.Tile.y))
		Color = s_PointColor;

	auto PossibleDirections = World.ListValidPathsInTile(Tile.Tile.x, Tile.Tile.y);
	BD_ASSERT(Tile.SelectedDirectionIndex < PossibleDirections.size());
	auto ActiveDirection = PossibleDirections[Tile.SelectedDirectionIndex];

	ForEachExistingDirection(Tile.ConnectedDirections, [&](TrackDirection Direction)
	{
		auto From = glm::vec2(Tile.Tile);
		auto To = From + 0.5f * glm::vec2(TrackDirectionToVector(Direction));

		if (!(Direction & ActiveDirection))
			From = 0.5f * (From + To);

		Renderer.Debug_PushLine(From, To, Color);
	});
}

void TrackView::RenderSignal(Renderer& Renderer, const Signal& Signal) const
{
	static std::unordered_map<SignalState, glm::vec3> SignalColors =
	{
		{ SignalState::Danger, { 0.9f, 0.0f, 0.0f } },
		{ SignalState::Clear, { 0.0f, 1.0f, 0.0f } },
	};

	BD_ASSERT(SignalColors.contains(Signal.State));
	auto Color = SignalColors[Signal.State];

	static constexpr float SignalIconLength = 0.4f;
	static constexpr float SignalIconHalfHeight = 0.16f;

	auto Center = 0.5f * (glm::vec2(Signal.From) + glm::vec2(Signal.To));
	auto Direction = normalize(glm::vec2(Signal.From - Signal.To));
	auto PerpendicularDirection = glm::vec2(Direction.y, -Direction.x);

	auto V1 = Center;
	auto V2 = Center + Direction * SignalIconLength + PerpendicularDirection * SignalIconHalfHeight;
	auto V3 = Center + Direction * SignalIconLength - PerpendicularDirection * SignalIconHalfHeight;

	Renderer.Debug_PushLine(V1, V2, Color);
	Renderer.Debug_PushLine(V2, V3, Color);
	Renderer.Debug_PushLine(V3, V1, Color);
}
