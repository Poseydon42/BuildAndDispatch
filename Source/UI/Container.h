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
	}

	void RemoveChild(size_t Index)
	{
		BD_ASSERT(Index < m_Widgets.size());
		m_Widgets.erase(m_Widgets.begin() + Index);
	}

	template<typename FuncType>
	requires (std::invocable<FuncType, Widget&>)
	void ForEachChild(FuncType&& Func)
	{
		std::for_each(m_Widgets.begin(), m_Widgets.end(), [&Func](std::unique_ptr<Widget>& Widget) { std::invoke(Func, *Widget); });
	}

	template<typename FuncType>
	requires (std::invocable<FuncType, const Widget&>)
	void ForEachChild(FuncType&& Func) const
	{
		std::for_each(m_Widgets.begin(), m_Widgets.end(), [&Func](const std::unique_ptr<Widget>& Widget) { std::invoke(Func, *Widget); });
	}

	virtual void Render(RenderBuffer& Buffer) const override
	{
		ForEachChild([&Buffer](const Widget& Widget) { Widget.Render(Buffer); });
	}

private:
	std::vector<std::unique_ptr<Widget>> m_Widgets;
};
