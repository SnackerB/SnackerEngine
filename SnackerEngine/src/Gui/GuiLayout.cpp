#include "Gui/GuiLayout.h"

namespace SnackerEngine
{

	GuiLayout::GuiLayout()
		: GuiElement(Vec2i(), Vec2i(), ResizeMode::SAME_AS_PARENT) {}

	GuiLayout::~GuiLayout() {}

	GuiLayout::GuiLayout(const GuiLayout& other) noexcept
		: GuiElement(other) {}

	GuiLayout& GuiLayout::operator=(const GuiLayout& other) noexcept
	{
		GuiElement::operator=(other);
		return *this;
	}

	GuiLayout::GuiLayout(GuiLayout&& other) noexcept
		: GuiElement(std::move(other)) {}

	GuiLayout& GuiLayout::operator=(GuiLayout&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		return *this;
	}

}