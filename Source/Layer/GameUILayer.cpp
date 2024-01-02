#include "GameUILayer.h"

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
		if (Current->OnMouseDown(Button))
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

void GameUILayer::Update(float DeltaTime, const InputState& InputState, World& World, Rect2D UsableArea)
{
	Layer::Update(DeltaTime, InputState, World, UsableArea);

	m_CurrentWorld = &World;

	auto CurrentTime = World.CurrentTime();
	m_GameTimeLabel->Text() = std::format("{:02}:{:02}:{:02}", CurrentTime.Hours(), CurrentTime.Minutes(), CurrentTime.Seconds());

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
	auto RootContainer = StackContainer::Create(StackContainer::Direction::Vertical);
	RootContainer->Style().LeftMargin = RootContainer->Style().RightMargin = RootContainer->Style().TopMargin = RootContainer->Style().BottomMargin = Size1D::Absolute(8.0f);
	
	auto GameSpeedPanel = CreateGameSpeedPanel();
	RootContainer->AddChild(std::move(GameSpeedPanel));
	RootContainer->Style().LeftMargin = RootContainer->Style().RightMargin = RootContainer->Style().TopMargin = RootContainer->Style().BottomMargin = Size1D::Absolute(8.0f);

	auto RootContainerWrapper = Panel::Create(std::move(RootContainer));
	m_RootWidget = std::move(RootContainerWrapper);
}

std::unique_ptr<Widget> GameUILayer::CreateGameSpeedPanel()
{
	auto Container = StackContainer::Create(StackContainer::Direction::Horizontal);
	Container->Spacing() = 8.0f;

	auto Font = Font::Load("Resources/Fonts/RobotoRegular.json");
	auto TimeLabel = Label::Create("", 24, Font);
	m_GameTimeLabel = TimeLabel.get();

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
