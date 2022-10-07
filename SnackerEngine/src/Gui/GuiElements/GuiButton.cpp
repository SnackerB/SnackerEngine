#include "GuiButton.h"
#include "Core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{
	void GuiButton::onHandleDestruction(GuiHandle& guiHandle)
	{
		eventHandle = nullptr;
	}

	void GuiButton::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		eventHandle = static_cast<GuiEventHandle*>(&guiHandle);
	}

	void GuiButton::onRegister()
	{
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
	}

	void GuiButton::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			isBeingPressed = true;
			setBackgroundColor(pressedHoverColor);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	void GuiButton::callbackMouseEnter(const Vec2d& position)
	{
		isBeingHovered = true;
		if (isBeingPressed) {
			setBackgroundColor(pressedHoverColor);
		}
		else {
			setBackgroundColor(hoverColor);
		}
	}

	void GuiButton::callbackMouseLeave(const Vec2d& position)
	{
		isBeingHovered = false;
		if (isBeingPressed) {
			setBackgroundColor(pressedColor);
		}
		else {
			setBackgroundColor(backgroundColor);
		}
	}

	void GuiButton::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE)
		{
			if (isBeingHovered)
			{
				if (eventHandle) activate(*eventHandle);
				setBackgroundColor(hoverColor);
			}
			else
			{
				setBackgroundColor(backgroundColor);
			}
			isBeingPressed = false;
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	GuiButton::GuiButton(const Vec2i& position, const Vec2i& size, const Color4f& backgroundColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode)
		: GuiDynamicTextBox(position, size, label, font, fontSize, textColor, backgroundColor, parseMode, alignment, textBoxMode),
		eventHandle(nullptr), backgroundColor(backgroundColor), hoverColor(hoverColor), pressedColor(pressedColor), pressedHoverColor(pressedHoverColor),
		isBeingPressed(false), isBeingHovered(false) {}

	GuiButton::GuiButton(GuiEventHandle& eventHandle, const Vec2i& position, const Vec2i& size, const Color4f& backgroundColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode)
		: GuiButton(position, size, backgroundColor, hoverColor, pressedColor, pressedHoverColor, label, font, fontSize, textColor, parseMode, alignment, textBoxMode)
	{
		setEventHandle(eventHandle);
	}

	GuiButton::GuiButton(const std::string& label, const GuiStyle& style)
		: GuiButton(Vec2i(), style.guiButtonSize, style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, style.guiButtonParseMode, style.guiButtonAlignment, style.guiButtonTextBoxMode) {}

	GuiButton::GuiButton(GuiEventHandle& eventHandle, const std::string& label, const GuiStyle& style)
		: GuiButton(eventHandle, Vec2i(), style.guiButtonSize, style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, style.guiButtonParseMode, style.guiButtonAlignment, style.guiButtonTextBoxMode) {}

	void GuiButton::setEventHandle(GuiEventHandle& eventHandle)
	{
		if (!this->eventHandle) {
			this->eventHandle = &eventHandle;
			signUpHandle(eventHandle, 0);
		}
	}

	GuiButton::GuiButton(const GuiButton& other) noexcept
		: GuiDynamicTextBox(other), eventHandle(nullptr), backgroundColor(other.backgroundColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor),
		isBeingPressed(other.isBeingPressed), isBeingHovered(other.isBeingHovered) {}

	GuiButton& GuiButton::operator=(const GuiButton& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		eventHandle = nullptr;
		backgroundColor = other.backgroundColor;
		hoverColor = other.hoverColor;
		pressedColor = other.pressedColor;
		pressedHoverColor = other.pressedHoverColor;
		isBeingPressed = other.isBeingPressed;
		isBeingHovered = other.isBeingHovered;
		return *this;
	}

	GuiButton::GuiButton(GuiButton&& other) noexcept
		: GuiDynamicTextBox(other), eventHandle(other.eventHandle), backgroundColor(other.backgroundColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor),
		isBeingPressed(other.isBeingPressed), isBeingHovered(other.isBeingHovered)
	{
		other.eventHandle = nullptr;
		if (eventHandle) notifyHandleOnGuiElementMove(*eventHandle);
	}

	GuiButton& GuiButton::operator=(GuiButton&& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		eventHandle = other.eventHandle;
		backgroundColor = other.backgroundColor;
		hoverColor = other.hoverColor;
		pressedColor = other.pressedColor;
		pressedHoverColor = other.pressedHoverColor;
		isBeingPressed = other.isBeingPressed;
		isBeingHovered = other.isBeingHovered;
		other.eventHandle = nullptr;
		if (eventHandle) notifyHandleOnGuiElementMove(*eventHandle);
		return *this;
	}

	GuiButton::~GuiButton()
	{
		if (eventHandle)
			signOffHandle(*eventHandle);
	}

}
