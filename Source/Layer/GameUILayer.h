#pragma once

#include "Layer/Layer.h"
#include "UI/Widget.h"
#include "UI/Widgets/Label.h"

class GameUILayer : public Layer
{
public:
	GameUILayer();

	virtual bool OnMousePress(MouseButton::Button Button, const InputState& InputState, World& World) override;

	virtual bool OnMouseRelease(MouseButton::Button Button, const InputState& InputState, World& World) override;

	virtual bool OnMouseScroll(int32_t Offset, const InputState& InputState, World& World) override;

	virtual void Update(float DeltaTime, const InputState& InputState, World& World, Rect2D UsableArea) override;

	virtual void Render(Renderer& Renderer, const World& World) const override;

private:
	std::unique_ptr<Widget> m_RootWidget;
	World* m_CurrentWorld = nullptr;

	Label* m_GameTimeLabel = nullptr;

	std::unique_ptr<Widget> CreateGameSpeedPanel();
};
