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
	void AddChild(std::unique_ptr<Widget>&& Child)
	{
		m_Widgets.push_back(std::move(Child));
		m_Widgets.back()->SetParent(this);
	}

	void RemoveChild(size_t Index)
	{
		BD_ASSERT(Index < m_Widgets.size());
		m_Widgets.erase(m_Widgets.begin() + Index);
	}

	size_t ChildCount() const
	{
		return m_Widgets.size();
	}

	virtual void Render(RenderBuffer& Buffer) const override
	{
		ForEachChild([&Buffer](const Widget& Widget) { Widget.Render(Buffer); });
	}

	virtual void ForEachChild(const ForEachChildCallbackType& Callback) override
	{
		std::ranges::for_each(m_Widgets, [&Callback](const std::unique_ptr<Widget>& Widget) { Callback(*Widget); });
	}

	virtual void ForEachChild(const ForEachChildConstCallbackType& Callback) const override
	{
		std::ranges::for_each(m_Widgets, [&Callback](const std::unique_ptr<Widget>& Widget) { Callback(*Widget); });
	}

private:
	std::vector<std::unique_ptr<Widget>> m_Widgets;
};
