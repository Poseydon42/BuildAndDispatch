#include "TrackLayer.h"

#include <algorithm>

#include "Core/Transform.h"

static constexpr float DefaultPixelsPerMeter = 64.0f;
static constexpr float PointActivationRadius = 0.25f;

static constexpr auto SignalStateClearIconPath = "Resources/Graphics/SignalClear.vif";
static constexpr auto SignalStateDangerIconPath = "Resources/Graphics/SignalDanger.vif";

static float AngleFromDirection(TrackDirection Direction)
{
	switch (Direction)
	{
	case TrackDirection::N:
		return 0.0f;
	case TrackDirection::NE:
		return glm::quarter_pi<float>();
	case TrackDirection::E:
		return glm::half_pi<float>();
	case TrackDirection::SE:
		return 3.0f * glm::quarter_pi<float>();
	case TrackDirection::S:
		return glm::pi<float>();
	case TrackDirection::SW:
		return 5.0f * glm::quarter_pi<float>();
	case TrackDirection::W:
		return 3.0f * glm::half_pi<float>();
	case TrackDirection::NW:
		return 7.0f * glm::quarter_pi<float>();
	default:
		BD_ASSERT(false);
	}
}

static glm::mat4 TransformationMatrixForSignal(const Signal& Signal)
{
	auto Direction = TrackDirectionFromVector(Signal.Location.ToTile - Signal.Location.FromTile);
	auto Angle = AngleFromDirection(Direction);
	auto Position = 0.5f * glm::vec2(Signal.Location.FromTile + Signal.Location.ToTile);

	return TransformationMatrix(Position, Angle);
}

std::unique_ptr<TrackLayer> TrackLayer::Create()
{
	auto Result = std::unique_ptr<TrackLayer>(new TrackLayer);

	auto SignalStateClearIcon = VectorIcon::LoadFromFile(SignalStateClearIconPath);
	if (!SignalStateClearIcon)
		return nullptr;
	Result->m_SignalIcons[SignalState::Clear] = std::move(SignalStateClearIcon);

	auto SignalStateDangerIcon = VectorIcon::LoadFromFile(SignalStateDangerIconPath);
	if (!SignalStateDangerIcon)
		return nullptr;
	Result->m_SignalIcons[SignalState::Danger] = std::move(SignalStateDangerIcon);

	return Result;
}

bool TrackLayer::OnMousePress(MouseButton::Button Button, const InputState& InputState, World& World)
{
	auto WorldPos = CursorPositionToWorldCoordinates(InputState.MousePosition, InputState.MousePositionBoundaries);
	auto TilePos = WorldPositionToTileCoordinates(WorldPos);

	for (const auto& Signal : World.Signals())
	{
		if (m_SignalIcons.begin()->second->IsPointInside(WorldPos, TransformationMatrixForSignal(Signal)))
		{
			if (Button == MouseButton::Left)
				HandleSignalClick(World, Signal);
			else if (Button == MouseButton::Right)
				m_RouteStartSignalLocation = std::nullopt;
			return true;
		}
	}

	if (Button == MouseButton::Left && World.IsPoint(TilePos.x, TilePos.y) && glm::distance(glm::vec2(TilePos), WorldPos) < PointActivationRadius)
	{
		World.SwitchPoint(TilePos.x, TilePos.y);
		return true;
	}

	return false;
}

bool TrackLayer::OnMouseScroll(int32_t Offset, const InputState&, World&)
{
	m_CameraScale = glm::min(glm::max(m_CameraScale + Offset * 0.2f, 0.5f), 4.0f);

	return true;
}

void TrackLayer::Update(float DeltaTime, const InputState& InputState, World& World, Rect2D)
{
	if (InputState.MouseButtonStates[MouseButton::Right])
		m_CameraLocation += glm::vec2(InputState.MousePositionDelta) * glm::vec2(-1.0f, 1.0f) / PixelsPerMeter();
}

void TrackLayer::Render(Renderer& Renderer, const World& World) const
{
	auto ViewMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::scale(ViewMatrix, glm::vec3(PixelsPerMeter()));
	ViewMatrix = glm::translate(ViewMatrix, -glm::vec3(m_CameraLocation, 0.0f));

	auto ProjectionMatrix = glm::mat4();
	ProjectionMatrix[0][0] = 2.0f / Renderer.FramebufferSize().x;
	ProjectionMatrix[1][1] = 2.0f / Renderer.FramebufferSize().y;
	ProjectionMatrix[3][3] = 1.0f;

	Renderer.SetViewProjectionMatrix(ProjectionMatrix * ViewMatrix);

	std::ranges::for_each(World.TrackTiles(), [&](const auto& TrackPiece) { RenderTrackTile(Renderer, World, TrackPiece); });
	std::ranges::for_each(World.Signals(), [&](const auto& Signal) { RenderSignal(Renderer, Signal); });

	// FIXME: this should only be done in debug mode
	std::ranges::for_each(World.Trains(), [&](const auto& Train) { RenderTrain(Renderer, Train); });

#define DRAW_GRID
#ifdef DRAW_GRID
	auto FramebufferSize = glm::ivec2(Renderer.FramebufferSize());
	auto TopLeftTile = WorldPositionToTileCoordinates(CursorPositionToWorldCoordinates(glm::ivec2(0, 0), FramebufferSize));
	auto BottomRightTile = WorldPositionToTileCoordinates(CursorPositionToWorldCoordinates(FramebufferSize, FramebufferSize));

	constexpr auto GridLineColor = glm::vec4(1.0f);
	for (auto X = TopLeftTile.x; X <= BottomRightTile.x; X++)
		Renderer.Debug_PushLine(glm::vec2(X + 0.5f, TopLeftTile.y + 0.5f), glm::vec2(X + 0.5f, BottomRightTile.y - 0.5f), GridLineColor);
	for (auto Y = BottomRightTile.y; Y <= TopLeftTile.y; Y++)
		Renderer.Debug_PushLine(glm::vec2(TopLeftTile.x - 0.5f, Y + 0.5f), glm::vec2(BottomRightTile.x + 0.5f, Y + 0.5f), GridLineColor);
#endif
}

TrackLayer::TrackLayer()
{
	m_TrackColors[TrackState::Free] = { 0.4f, 0.4f, 0.4f };
	m_TrackColors[TrackState::Reserved] = { 0.76f, 0.75f, 0.14f };
	m_TrackColors[TrackState::Occupied] = { 1.0f, 0.0f, 0.1f };
}

float TrackLayer::PixelsPerMeter() const
{
	return m_CameraScale * DefaultPixelsPerMeter;
}

void TrackLayer::HandleSignalClick(World& World, const Signal& Signal)
{
	// If the player has already selected the start signal for a new route,
	// the current signal is the end signal.
	if (m_RouteStartSignalLocation)
	{
		BD_LOG_DEBUG("Trying to create route from ({},{}) to ({},{})",
			m_RouteStartSignalLocation->FromTile.x, m_RouteStartSignalLocation->FromTile.y,
			Signal.Location.FromTile.x, Signal.Location.FromTile.y);

		auto MaybeRoute = World.TryCreateRoute(*m_RouteStartSignalLocation, Signal.Location);
		if (MaybeRoute)
		{
			BD_LOG_DEBUG("Successfully created route from ({},{}) to ({},{})",
				m_RouteStartSignalLocation->FromTile.x, m_RouteStartSignalLocation->FromTile.y,
				Signal.Location.FromTile.x, Signal.Location.FromTile.y);
			BD_LOG_DEBUG("Route: ");
			for (auto Tile : MaybeRoute->Tiles)
				BD_LOG_DEBUG("\t({},{})", Tile.x, Tile.y);

			if (!World.TryOpenRoute(*MaybeRoute))
				BD_LOG_DEBUG("Route from ({},{}) to ({},{}) is available, but cannot be cleared",
					m_RouteStartSignalLocation->FromTile.x, m_RouteStartSignalLocation->FromTile.y,
					Signal.Location.FromTile.x, Signal.Location.FromTile.y);
		}

		m_RouteStartSignalLocation = std::nullopt;
	}
	else
	{
		BD_LOG_DEBUG("Route start signal set to ({},{})", Signal.Location.FromTile.x, Signal.Location.FromTile.y)
		m_RouteStartSignalLocation = Signal.Location;
	}
}

void TrackLayer::RenderTrackTile(Renderer& Renderer, const World& World, const TrackTile& Tile) const
{
	auto PossiblePaths = World.ListValidPathsInTile(Tile.Tile.x, Tile.Tile.y);
	BD_ASSERT(Tile.SelectedPath < PossiblePaths.size());
	auto ActiveDirection = PossiblePaths[Tile.SelectedPath];

	ForEachExistingDirection(Tile.ConnectedDirections, [&](TrackDirection Direction)
	{
		auto From = glm::vec2(Tile.Tile);
		auto To = From + 0.5f * glm::vec2(TrackDirectionToVector(Direction));

		if (!(Direction & ActiveDirection))
			From = 0.5f * (From + To);

		auto State = Tile.State(Direction);
		BD_ASSERT(m_TrackColors.contains(State));
		auto Color = m_TrackColors.at(State);

		Renderer.Debug_PushLine(From, To, Color);
	});
}

void TrackLayer::RenderSignal(Renderer& Renderer, const Signal& Signal) const
{
	BD_ASSERT(m_SignalIcons.contains(Signal.State));
	const auto& Icon = *m_SignalIcons.at(Signal.State);

	Renderer.Draw(Icon, TransformationMatrixForSignal(Signal));
}

void TrackLayer::RenderTrain(Renderer& Renderer, const Train& Train) const
{
	if (!Train.Timetable.IsPresentInTheWorld())
		return;

	constexpr float DebugTrainHalfSize = 0.06f;
	constexpr glm::vec3 DebugTrainColor = { 0.0f, 0.0f, 1.0f };

	auto TrainLocation = glm::vec2(Train.Tile) + 0.5f * glm::vec2(TrackDirectionToVector(Train.Direction)) * Train.OffsetInTile;

	auto V1 = TrainLocation + glm::vec2(DebugTrainHalfSize, DebugTrainHalfSize);
	auto V2 = TrainLocation + glm::vec2(DebugTrainHalfSize, -DebugTrainHalfSize);
	auto V3 = TrainLocation + glm::vec2(-DebugTrainHalfSize, -DebugTrainHalfSize);
	auto V4 = TrainLocation + glm::vec2(-DebugTrainHalfSize, DebugTrainHalfSize);

	Renderer.Debug_PushLine(V1, V2, DebugTrainColor);
	Renderer.Debug_PushLine(V2, V3, DebugTrainColor);
	Renderer.Debug_PushLine(V3, V4, DebugTrainColor);
	Renderer.Debug_PushLine(V4, V1, DebugTrainColor);
}

glm::vec2 TrackLayer::CursorPositionToWorldCoordinates(glm::ivec2 CursorPosition, glm::ivec2 CursorAreaBoundaries) const
{
	auto NormalizedCoordinates = glm::vec2(CursorPosition) / glm::vec2(CursorAreaBoundaries) * 2.0f - 1.0f;
	NormalizedCoordinates.y *= -1.0f; // Because in our world coordinate system +Y points upwards

	// Convert it back into pixels, but with (0, 0) at the center of the screen and axes pointing to the right and upward
	auto ScaledCoordinates = NormalizedCoordinates * 0.5f * glm::vec2(CursorAreaBoundaries);

	auto WorldCoordinates = ScaledCoordinates / PixelsPerMeter() + m_CameraLocation;

	return WorldCoordinates;
}

glm::ivec2 TrackLayer::WorldPositionToTileCoordinates(glm::vec2 WorldPosition) const
{
	return { std::round(WorldPosition.x), std::round(WorldPosition.y) };
}
