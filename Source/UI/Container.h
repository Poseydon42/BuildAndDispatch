#pragma once

#include <algorithm>
#include <concepts>
#include <memory>
#include <vector>

#include "Core/Assert.h"
#include "UI/Widget.h"

class Container : public Widget
{
public:
	void AddChild(std::shared_ptr<Widget>&& Child)
	{
		m_Widgets.push_back(std::move(Child));
		m_Widgets.back()->SetParent(this);
	}

	void RemoveChild(size_t Index)
	{
		BD_ASSERT(Index < m_Widgets.size());
		m_Widgets.erase(m_Widgets.begin() + Index);
	}

	void ClearChildren()
	{
		m_Widgets.clear();
	}

	size_t ChildCount() const
	{
		return m_Widgets.size();
	}

	virtual void Render(RenderBuffer& Buffer) const override
	{
		Widget::Render(Buffer);
		ForEachChild([&Buffer](const Widget& Widget) { Widget.Render(Buffer); });
	}

	virtual void ForEachChild(const ForEachChildCallbackType& Callback) override
	{
		std::ranges::for_each(m_Widgets, [&Callback](const std::shared_ptr<Widget>& Widget) { Callback(*Widget); });
	}

	virtual void ForEachChild(const ForEachChildConstCallbackType& Callback) const override
	{
		std::ranges::for_each(m_Widgets, [&Callback](const std::shared_ptr<Widget>& Widget) { Callback(*Widget); });
	}

private:
	std::vector<std::shared_ptr<Widget>> m_Widgets;
};
