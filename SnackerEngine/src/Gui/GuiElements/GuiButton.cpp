#include "Gui/GuiElements/GuiButton.h"
#include "Core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{

	void GuiButton::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		eventHandle = static_cast<GuiEventHandle*>(&guiHandle);
	}

	void GuiButton::onHandleDestruction(GuiHandle& guiHandle)
	{
		eventHandle = nullptr;
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
			setBackgroundColor(defaultColor);
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
				setBackgroundColor(defaultColor);
			}
			isBeingPressed = false;
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	GuiButton::IsCollidingResult GuiButton::isColliding(const Vec2i& position)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (position.x > myPosition.x && position.x < myPosition.x + mySize.x
			&& position.y > myPosition.y && position.y < myPosition.y + mySize.y) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}

	GuiButton::GuiButton(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const Color4f& defaultColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiDynamicTextBox(position, size, resizeMode, label, font, fontSize, textColor, defaultColor, parseMode, alignment, textBoxMode, singleLine),
		eventHandle(nullptr), defaultColor(defaultColor), hoverColor(hoverColor), pressedColor(pressedColor), pressedHoverColor(pressedHoverColor),
		isBeingPressed(false), isBeingHovered(false)
	{
		if (resizeMode == ResizeMode::RESIZE_RANGE) {
			const Vec2i& textSize = getTextSize();
			setMinSize({ textSize.x, textSize.y });
			setMaxSize({ -1, textSize.y });
		}
	}

	GuiButton::GuiButton(GuiEventHandle& eventHandle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const Color4f& defaultColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiButton(position, size, resizeMode, defaultColor, hoverColor, pressedColor, pressedHoverColor, label, font, fontSize, textColor, parseMode, alignment, textBoxMode, singleLine)
	{
		setEventHandle(eventHandle);
	}

	GuiButton::GuiButton(const std::string& label, const GuiStyle& style)
		: GuiButton(Vec2i(), style.guiButtonSize, style.guiButtonResizeMode, style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, style.guiButtonParseMode, style.guiButtonAlignment, style.guiButtonTextBoxMode2, style.guiButtonSingleLine) {}

	GuiButton::GuiButton(GuiEventHandle& eventHandle, const std::string& label, const GuiStyle& style)
		: GuiButton(eventHandle, Vec2i(), style.guiButtonSize, style.guiButtonResizeMode, style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, style.guiButtonParseMode, style.guiButtonAlignment, style.guiButtonTextBoxMode2, style.guiButtonSingleLine) {}

	void GuiButton::setEventHandle(GuiEventHandle& eventHandle)
	{
		if (!this->eventHandle) {
			this->eventHandle = &eventHandle;
			signUpHandle(eventHandle, 0);
		}
	}

	void GuiButton::setDefaultColor(const Color4f& defaultColor)
	{
		this->defaultColor = defaultColor;
		if (isBeingHovered == false && isBeingPressed == false) {
			setBackgroundColor(this->defaultColor);
		}
	}

	void GuiButton::setHoverColor(const Color4f& hoverColor)
	{
		this->hoverColor = hoverColor;
		if (isBeingHovered == true && isBeingPressed == false) {
			setBackgroundColor(this->hoverColor);
		}
	}

	void GuiButton::setPressedCOlor(const Color4f& pressedColor)
	{
		this->pressedColor = pressedColor;
		if (isBeingHovered == false && isBeingPressed == true) {
			setBackgroundColor(this->pressedColor);
		}
	}

	void GuiButton::setPressedHoverColor(const Color4f& pressedHoverColor)
	{
		this->pressedHoverColor;
		if (isBeingHovered == true && isBeingPressed == true) {
			setBackgroundColor(this->pressedHoverColor);
		}
	}

	GuiButton::GuiButton(const GuiButton& other) noexcept
		: GuiDynamicTextBox(other), eventHandle(nullptr), defaultColor(other.defaultColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor),
		isBeingPressed(false), isBeingHovered(false) {}

	GuiButton& GuiButton::operator=(const GuiButton& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		eventHandle = nullptr;
		defaultColor = other.defaultColor;
		hoverColor = other.hoverColor;
		pressedColor = other.pressedColor;
		pressedHoverColor = other.pressedHoverColor;
		isBeingPressed = false;
		isBeingHovered = false;
		return *this;
	}

	GuiButton::GuiButton(GuiButton&& other) noexcept
		: GuiDynamicTextBox(std::move(other)), eventHandle(other.eventHandle), defaultColor(other.defaultColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor),
		isBeingPressed(other.isBeingPressed), isBeingHovered(other.isBeingHovered)
	{
		other.eventHandle = nullptr;
		if (eventHandle) notifyHandleOnGuiElementMove(*eventHandle);
	}

	GuiButton& GuiButton::operator=(GuiButton&& other) noexcept
	{
		GuiDynamicTextBox::operator=(std::move(other));
		eventHandle = other.eventHandle;
		defaultColor = other.defaultColor;
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