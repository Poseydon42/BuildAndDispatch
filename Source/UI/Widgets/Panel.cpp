#include "Panel.h"

std::unique_ptr<Panel> Panel::Create(std::optional<std::unique_ptr<Brush>> Brush, std::optional<std::unique_ptr<Widget>> Content)
{
	return std::unique_ptr<Panel>(new Panel(std::move(Brush), std::move(Content)));
}

glm::vec2 Panel::ComputePreferredSize() const
{
	if (m_Content)
		return m_Content->ComputePreferredSize();
	return {};
}

void Panel::Layout()
{
	if (!m_Content)
		return;

	auto ContentPreferredSize = m_Content->ComputePreferredSize();

	auto LeftMargin   = m_Content->LeftMargin()  .GetAbsoluteValue(BoundingBox().Width() );
	auto RightMargin  = m_Content->RightMargin() .GetAbsoluteValue(BoundingBox().Width() );
	if (LeftMargin + ContentPreferredSize.x + RightMargin > BoundingBox().Width())
	{
		float Scale = (BoundingBox().Width() - ContentPreferredSize.x) / (LeftMargin + RightMargin);
		LeftMargin *= Scale;
		RightMargin *= Scale;
	}
	m_Content->BoundingBox().Left() = BoundingBox().Left() + LeftMargin;
	m_Content->BoundingBox().Right() = BoundingBox().Right() - RightMargin;

	auto TopMargin = m_Content->TopMargin().GetAbsoluteValue(BoundingBox().Height());
	auto BottomMargin = m_Content->BottomMargin().GetAbsoluteValue(BoundingBox().Height());
	if (TopMargin + ContentPreferredSize.y + BottomMargin > BoundingBox().Height())
	{
		float Scale = (BoundingBox().Height() - ContentPreferredSize.y) / (TopMargin + BottomMargin);
		TopMargin *= Scale;
		BottomMargin *= Scale;
	}
	m_Content->BoundingBox().Top() = BoundingBox().Top() - TopMargin;
	m_Content->BoundingBox().Bottom() = BoundingBox().Bottom() + BottomMargin;

	m_Content->Layout();
}

void Panel::Render(RenderBuffer& Buffer) const
{
	Widget::Render(Buffer);

	if (m_Brush)
		Buffer.Rect(BoundingBox(), *m_Brush);

	if (m_Content)
		m_Content->Render(Buffer);
}

void Panel::ForEachChild(const ForEachChildCallbackType& Callback)
{
	if (m_Content)
		Callback(*m_Content);
}

void Panel::ForEachChild(const ForEachChildConstCallbackType& Callback) const
{
	if (m_Content)
		Callback(*m_Content);
}

Panel::Panel(std::optional<std::unique_ptr<Brush>> Brush, std::optional<std::unique_ptr<Widget>> Content)
	: m_Brush(Brush.has_value() ? std::move(Brush.value()) : nullptr)
	, m_Content(Content.has_value() ? std::move(Content.value()) : nullptr)
{
	if (m_Content)
		m_Content->SetParent(this);
}
