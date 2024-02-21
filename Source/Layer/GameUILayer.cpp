#include "GameUILayer.h"

#include <numeric>

#include "UI/Containers/StackContainer.h"
#include "UI/Widgets/Button.h"
#include "UI/Widgets/Image.h"
#include "UI/Widgets/Panel.h"

static glm::vec2 CursorPositionFromInput(const InputState& InputState)
{
	// Need to invert the Y axis since the UI system expects the origin to be at the bottom left corner
	return { InputState.MousePosition.x, InputState.MousePositionBoundaries.y - InputState.MousePosition.y };
}

static Widget* FindWidgetAt(Widget& Root, glm::vec2 Location)
{
	if (!Root.BoundingBox().Contains(Location))
		return nullptr;

	auto Current = &Root;
	while (Current)
	{
		bool FoundLowerWidget = false;
		Current->ForEachChild([&](Widget& Child)
		{
			if (!Child.BoundingBox().Contains(Location))
				return;
			Current = &Child;
			FoundLowerWidget = true;
		});

		if (!FoundLowerWidget)
			break;
	}
	return Current;
}

static bool HandleMouseEvent(Widget& RootWidget, MouseButton::Button Button, bool IsPress, const InputState& InputState)
{
	auto* Widget = FindWidgetAt(RootWidget, CursorPositionFromInput(InputState));
	if (!Widget)
		return false;

	auto* Current = Widget;
	while (Current)
	{
		if (IsPress)
		{
			if (Current->OnMouseDown(Button))
				return true;
		}
		else
		{
			if (Current->OnMouseUp(Button))
				return true;
		}
		Current = Current->Parent();
	}

	return false;
}

static bool HandleScrollEvent(Widget& RootWidget, int32_t Offset, const InputState& InputState)
{
	auto* Widget = FindWidgetAt(RootWidget, CursorPositionFromInput(InputState));
	if (!Widget)
		return false;

	auto* Current = Widget;
	while (Current)
	{
		if (Current->OnScroll(Offset))
			return true;
		Current = Current->Parent();
	}

	return false;
}

bool GameUILayer::OnMousePress(MouseButton::Button Button, const InputState& InputState, World&)
{
	return HandleMouseEvent(*m_RootWidget, Button, true, InputState);
}

bool GameUILayer::OnMouseRelease(MouseButton::Button Button, const InputState& InputState, World&)
{
	return HandleMouseEvent(*m_RootWidget, Button, true, InputState);
}

bool GameUILayer::OnMouseScroll(int32_t Offset, const InputState& InputState, World& World)
{
	return HandleScrollEvent(*m_RootWidget, Offset, InputState);
}

void GameUILayer::Update(float DeltaTime, const InputState& InputState, World& World, Rect2D UsableArea)
{
	Layer::Update(DeltaTime, InputState, World, UsableArea);

	m_CurrentWorld = &World;

	auto CurrentTime = World.CurrentTime();
	m_GameTimeLabel->Text() = std::format("{:02}:{:02}:{:02}", CurrentTime.Hours(), CurrentTime.Minutes(), CurrentTime.Seconds());

	auto TotalScore = std::accumulate(World.Trains().begin(), World.Trains().end(), 0u, [](uint32_t Score, const Train& Train)
	{
		return Score + Train.Timetable.Score();
	});
	m_GameScoreLabel->Text() = std::format("{}", TotalScore);

	UpdateTimetablePanel(World);

	m_RootWidget->BoundingBox() = UsableArea;
	m_RootWidget->Layout();
}

void GameUILayer::Render(Renderer& Renderer, const World& World) const
{
	Layer::Render(Renderer, World);

	Renderer.SetViewProjectionMatrix(glm::mat4(1.0f));

	RenderBuffer RenderBuffer(Renderer);
	m_RootWidget->Render(RenderBuffer);
}

GameUILayer::GameUILayer()
{
	m_UIFont = Font::Load("Resources/Fonts/RobotoRegular.json");

	auto RootContainer = StackContainer::Create(StackContainer::Direction::Vertical);
	RootContainer->Style().LeftMargin = RootContainer->Style().RightMargin = RootContainer->Style().TopMargin = RootContainer->Style().BottomMargin = Size1D::Absolute(8.0f);

	auto GameSpeedAndScoreContainer = StackContainer::Create(StackContainer::Direction::Horizontal);
	GameSpeedAndScoreContainer->Style().HorizontalStretchRatio = 1.0f;

	auto GameSpeedPanel = CreateGameSpeedPanel();
	GameSpeedAndScoreContainer->AddChild(std::move(GameSpeedPanel));

	auto GameSpeedAndScoreContainerLeftSpacer = Widget::Create();
	GameSpeedAndScoreContainerLeftSpacer->Style().HorizontalStretchRatio = 1.0f;
	GameSpeedAndScoreContainer->AddChild(std::move(GameSpeedAndScoreContainerLeftSpacer));

	auto GameScorePanel = CreateGameScorePanel();
	GameSpeedAndScoreContainer->AddChild(std::move(GameScorePanel));

	auto GameSpeedAndScoreContainerRightSpacer = Widget::Create();
	GameSpeedAndScoreContainerRightSpacer->Style().HorizontalStretchRatio = 3.0f;
	GameSpeedAndScoreContainer->AddChild(std::move(GameSpeedAndScoreContainerRightSpacer));

	RootContainer->AddChild(std::move(GameSpeedAndScoreContainer));

	auto Spacer = Widget::Create();
	Spacer->Style().VerticalStretchRatio = 1.0f;
	RootContainer->AddChild(std::move(Spacer));

	auto TimetablePanel = CreateTimetablePanel();
	TimetablePanel->Style().LeftMargin = TimetablePanel->Style().RightMargin = Size1D::Relative(0.15f);
	RootContainer->AddChild(std::move(TimetablePanel));

	auto RootContainerWrapper = Panel::Create(std::move(RootContainer));
	m_RootWidget = std::move(RootContainerWrapper);
}

std::unique_ptr<Widget> GameUILayer::CreateGameSpeedPanel()
{
	auto Container = StackContainer::Create(StackContainer::Direction::Horizontal);
	Container->Spacing() = 8.0f;

	auto Font = Font::Load("Resources/Fonts/ConsolaMono.json");
	std::shared_ptr TimeLabel = Label::Create("", 26, Font);
	m_GameTimeLabel = TimeLabel;

	auto PauseButton = Button::Create(Image::LoadFromFile("Resources/UI/pause.png"), [this](bool IsPress)
	{
		if (IsPress)
		{
			BD_ASSERT(m_CurrentWorld);
			m_CurrentWorld->SetSimulationSpeed(0.0f);
		}
	});
	
	auto PlayButton = Button::Create(Image::LoadFromFile("Resources/UI/play1.png"), [this](bool IsPress)
	{
		if (IsPress)
		{
			BD_ASSERT(m_CurrentWorld);
			m_CurrentWorld->SetSimulationSpeed(1.0f);
		}
	});

	auto Speed1Button = Button::Create(Image::LoadFromFile("Resources/UI/play2.png"), [this](bool IsPress)
	{
		if (IsPress)
		{
			BD_ASSERT(m_CurrentWorld);
			m_CurrentWorld->SetSimulationSpeed(3.0f);
		}
	});

	auto Speed2Button = Button::Create(Image::LoadFromFile("Resources/UI/play3.png"), [this](bool IsPress)
	{
		if (IsPress)
		{
			BD_ASSERT(m_CurrentWorld);
			m_CurrentWorld->SetSimulationSpeed(10.0f);
		}
	});

	TimeLabel->Style().BackgroundColor = PauseButton->Style().BackgroundColor = PlayButton->Style().BackgroundColor = Speed1Button->Style().BackgroundColor = Speed2Button->Style().BackgroundColor = glm::vec4(0.21f, 0.21f, 0.18f, 1.0f);
	TimeLabel->Style().BorderColor = PauseButton->Style().BorderColor = PlayButton->Style().BorderColor = Speed1Button->Style().BorderColor = Speed2Button->Style().BorderColor = glm::vec4(0.37f, 0.37f, 0.33f, 1.0f);
	TimeLabel->Style().BorderThickness = PauseButton->Style().BorderThickness = PlayButton->Style().BorderThickness = Speed1Button->Style().BorderThickness = Speed2Button->Style().BorderThickness = 4.0f;
	TimeLabel->Style().CornerRadius = PauseButton->Style().CornerRadius = PlayButton->Style().CornerRadius = Speed1Button->Style().CornerRadius = Speed2Button->Style().CornerRadius = 6.0f;
	TimeLabel->Style().PaddingLeft = TimeLabel->Style().PaddingRight = 6.0f;

	Container->AddChild(std::move(TimeLabel));
	Container->AddChild(std::move(PauseButton));
	Container->AddChild(std::move(PlayButton));
	Container->AddChild(std::move(Speed1Button));
	Container->AddChild(std::move(Speed2Button));

	return Container;
}

std::shared_ptr<Widget> GameUILayer::CreateGameScorePanel()
{
	auto Container = StackContainer::Create(StackContainer::Direction::Horizontal);
	Container->Style().BackgroundColor = glm::vec4(0.21f, 0.21f, 0.18f, 1.0f);
	Container->Style().BorderColor = glm::vec4(0.37f, 0.37f, 0.33f, 1.0f);
	Container->Style().BorderThickness = 4.0f;
	Container->Style().CornerRadius = 6.0f;
	Container->Style().PaddingLeft = Container->Style().PaddingRight = 6.0f;
	Container->Spacing() = 6.0f;

	auto Icon = Image::LoadFromFile("Resources/UI/score.png");
	Container->AddChild(std::move(Icon));

	constexpr auto GameScoreLabelFontSize = 28u;
	std::shared_ptr Label = Label::Create("0000", GameScoreLabelFontSize, m_UIFont);
	Container->AddChild(Label);

	m_GameScoreLabel = Label;
	return Container;
}

std::unique_ptr<Widget> GameUILayer::CreateTimetablePanel()
{
	// Columns: ID, Track, Enter, Enters At, Arrival, Departure, Leave, Leaves At
	std::pair<std::string, float> Columns[] = {
		std::make_pair(std::string("ID"), 1.0f),
		std::make_pair(std::string("Track"), 1.0f),
		std::make_pair(std::string("Enter"), 1.0f),
		std::make_pair(std::string("Arrives From"), 1.0f),
		std::make_pair(std::string("Arrival"), 1.0f),
		std::make_pair(std::string("Departure"), 1.0f),
		std::make_pair(std::string("Leave"), 1.0f),
		std::make_pair(std::string("Leaves Towards"), 1.0f),
	};
	auto Table = TableContainer::Create(Columns, m_UIFontSize, m_UIFont, 5);
	Table->Style().LeftMargin = Table->Style().RightMargin = Size1D::Relative(0.5f);
	Table->Style().BackgroundColor = glm::vec4(0.21f, 0.21f, 0.18f, 1.0f);
	Table->Style().BorderColor = glm::vec4(0.37f, 0.37f, 0.33f, 1.0f);
	Table->Style().BorderThickness = 4.0f;
	Table->Style().CornerRadius = 6.0f;
	Table->CellPadding() = 4.0f;
	Table->LineThickness() = 1.5f;
	Table->LineColor() = glm::vec4(0.96f, 0.96f, 0.96f, 1.0f);

	m_TimetablePanel = Table.get();

	return Table;
}

void GameUILayer::UpdateTimetablePanel(const World& World)
{
	m_TimetablePanel->ClearChildren();

	for (const auto& Train : World.Trains())
	{
		// Columns: ID, Track, Enters At, Enters From, Arrival, Departure, Leaves At, Leaves Towards
		auto ID = Label::Create(Train.ID, m_UIFontSize, m_UIFont);

		auto EnterTime = Train.Timetable.SpawnTime;
		auto ArrivalTime = Train.Timetable.ArrivalTime;
		auto DepartureTime = Train.Timetable.DepartureTime;
		auto LeaveTime = Train.Timetable.LeaveTime;

		auto Track = Label::Create(Train.Timetable.PreferredTrack, m_UIFontSize, m_UIFont);
		auto EntersAt = Label::Create(std::format("{:02}:{:02}:{:02}", EnterTime.Hours(), EnterTime.Minutes(), EnterTime.Seconds()), m_UIFontSize, m_UIFont);
		auto EntersFrom = Label::Create(Train.Timetable.SpawnLocation, m_UIFontSize, m_UIFont);
		auto Arrival = Label::Create(std::format("{:02}:{:02}:{:02}", ArrivalTime.Hours(), ArrivalTime.Minutes(), ArrivalTime.Seconds()), m_UIFontSize, m_UIFont);
		auto Departure = Label::Create(std::format("{:02}:{:02}:{:02}", DepartureTime.Hours(), DepartureTime.Minutes(), DepartureTime.Seconds()), m_UIFontSize, m_UIFont);
		auto LeavesAt = Label::Create(std::format("{:02}:{:02}:{:02}", LeaveTime.Hours(), LeaveTime.Minutes(), LeaveTime.Seconds()), m_UIFontSize, m_UIFont);
		auto LeavesTowards = Label::Create(Train.Timetable.LeaveLocation, m_UIFontSize, m_UIFont);

		m_TimetablePanel->AddChild(std::move(ID));
		m_TimetablePanel->AddChild(std::move(Track));
		m_TimetablePanel->AddChild(std::move(EntersAt));
		m_TimetablePanel->AddChild(std::move(EntersFrom));
		m_TimetablePanel->AddChild(std::move(Arrival));
		m_TimetablePanel->AddChild(std::move(Departure));
		m_TimetablePanel->AddChild(std::move(LeavesAt));
		m_TimetablePanel->AddChild(std::move(LeavesTowards));
	}
}
