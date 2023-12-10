#include "Button.h"

std::unique_ptr<Button> Button::Create(std::unique_ptr<Widget> Label, ButtonCallbackType Callback)
{
	return std::unique_ptr<Button>(new Button(std::move(Label), std::move(Callback)));
}

glm::vec2 Button::ComputePreferredSize() const
{
	if (!m_Label)
		return Widget::ComputePreferredSize();

	return m_Label->ComputePreferredSize();
}

void Button::Layout()
{
	if (m_Label)
	{
		m_Label->BoundingBox() = BoundingBox();
		m_Label->Layout();
	}
}

void Button::Render(RenderBuffer& Buffer) const
{
	Widget::Render(Buffer);

	if (m_Label)
		m_Label->Render(Buffer);
}

bool Button::OnMouseDown(MouseButton::Button Button)
{
	if (m_Callback)
		m_Callback(true);
	return true;
}

bool Button::OnMouseUp(MouseButton::Button Button)
{
	if (m_Callback)
		m_Callback(false);
	return true;
}

void Button::ForEachChild(const ForEachChildCallbackType& Callback)
{
	if (m_Label)
		Callback(*m_Label);
}

void Button::ForEachChild(const ForEachChildConstCallbackType& Callback) const
{
	if (m_Label)
		Callback(*m_Label);
}

Button::Button(std::unique_ptr<Widget> Label, ButtonCallbackType Callback)
	: m_Label(std::move(Label))
	, m_Callback(std::move(Callback))
{
	if (m_Label)
		m_Label->SetParent(this);
}
