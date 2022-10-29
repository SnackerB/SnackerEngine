#include "Gui/GuiLayout2.h"

namespace SnackerEngine
{

	GuiLayout2::GuiLayout2()
		: GuiElement2(Vec2i(), Vec2i(), ResizeMode::SAME_AS_PARENT) {}

	GuiLayout2::~GuiLayout2() {}

	GuiLayout2::GuiLayout2(const GuiLayout2& other) noexcept
		: GuiElement2(other) {}

	GuiLayout2& GuiLayout2::operator=(const GuiLayout2& other) noexcept
	{
		GuiElement2::operator=(other);
		return *this;
	}

	GuiLayout2::GuiLayout2(GuiLayout2&& other) noexcept
		: GuiElement2(std::move(other)) {}

	GuiLayout2& GuiLayout2::operator=(GuiLayout2&& other) noexcept
	{
		GuiElement2::operator=(std::move(other));
		return *this;
	}

}