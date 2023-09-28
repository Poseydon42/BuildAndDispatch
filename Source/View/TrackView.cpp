#include "TrackView.h"

#include <algorithm>

void TrackView::Render(Renderer& Renderer, const World& World) const
{
	std::ranges::for_each(World.TrackTiles(), [&](const auto& TrackPiece) { RenderTrackTile(Renderer, World, TrackPiece); });
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
