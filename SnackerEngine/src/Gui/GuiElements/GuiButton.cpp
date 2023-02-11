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
		if (!locked) {
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
		}
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

	GuiButton::GuiButton(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const Color4f& defaultColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, const Color4f& lockedColor, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const int& border, const TextScaleMode& textScaleMode, const SizeHintModes sizeHintModes, bool doRecomputeOnSizeChange)
		: GuiDynamicTextBox(position, size, resizeMode, label, font, fontSize, textColor, defaultColor, parseMode, alignment, border, textScaleMode, sizeHintModes, doRecomputeOnSizeChange),
		eventHandle(nullptr), defaultColor(defaultColor), hoverColor(hoverColor), pressedColor(pressedColor), pressedHoverColor(pressedHoverColor), lockedColor(lockedColor),
		isBeingPressed(false), isBeingHovered(false), locked(false)
	{
	}

	GuiButton::GuiButton(GuiEventHandle& eventHandle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const Color4f& defaultColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, const Color4f& lockedColor, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const int& border, const TextScaleMode& textScaleMode, const SizeHintModes sizeHintModes, bool doRecomputeOnSizeChange)
		: GuiButton(position, size, resizeMode, defaultColor, hoverColor, pressedColor, pressedHoverColor, lockedColor, label, font, fontSize, textColor, parseMode, alignment, border, textScaleMode, sizeHintModes, doRecomputeOnSizeChange)
	{
		setEventHandle(eventHandle);
	}

	GuiButton::GuiButton(const std::string& label, const GuiStyle& style)
		: GuiButton(Vec2i(), style.guiButtonSize, style.guiButtonResizeMode, style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor, style.guiButtonLockedColor, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, style.guiButtonParseMode, style.guiButtonAlignment, style.guiButtonBorder, style.guiButtonTextScaleMode, style.guiButtonSizeHintModes, style.guiButtonDoRecomputeOnSizeChange) {}

	GuiButton::GuiButton(GuiEventHandle& eventHandle, const std::string& label, const GuiStyle& style)
		: GuiButton(eventHandle, Vec2i(), style.guiButtonSize, style.guiButtonResizeMode, style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor, style.guiButtonLockedColor, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, style.guiButtonParseMode, style.guiButtonAlignment, style.guiButtonBorder, style.guiButtonTextScaleMode, style.guiButtonSizeHintModes, style.guiButtonDoRecomputeOnSizeChange) {}

	void GuiButton::setEventHandle(GuiEventHandle& eventHandle)
	{
		if (!this->eventHandle) {
			this->eventHandle = &eventHandle;
			signUpHandle(eventHandle, 0);
		}
	}

	void GuiButton::lock()
	{
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		isBeingHovered = false;
		isBeingPressed = false;
		locked = true;
		setBackgroundColor(lockedColor);
	}

	void GuiButton::unlock()
	{
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
		locked = false;
		setBackgroundColor(defaultColor);
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

	void GuiButton::setPressedColor(const Color4f& pressedColor)
	{
		this->pressedColor = pressedColor;
		if (isBeingHovered == false && isBeingPressed == true) {
			setBackgroundColor(this->pressedColor);
		}
	}

	void GuiButton::setPressedHoverColor(const Color4f& pressedHoverColor)
	{
		this->pressedHoverColor = pressedHoverColor;
		if (isBeingHovered == true && isBeingPressed == true) {
			setBackgroundColor(this->pressedHoverColor);
		}
	}

	void GuiButton::setLockedColor(const Color4f& lockedColor)
	{
		this->lockedColor = lockedColor;
		if (locked) {
			setBackgroundColor(this->lockedColor);
		}
	}

	GuiButton::GuiButton(const GuiButton& other) noexcept
		: GuiDynamicTextBox(other), eventHandle(nullptr), defaultColor(other.defaultColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor), lockedColor(other.lockedColor),
		isBeingPressed(false), isBeingHovered(false), locked(other.locked) {}

	GuiButton& GuiButton::operator=(const GuiButton& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		eventHandle = nullptr;
		defaultColor = other.defaultColor;
		hoverColor = other.hoverColor;
		pressedColor = other.pressedColor;
		pressedHoverColor = other.pressedHoverColor;
		lockedColor = other.lockedColor;
		isBeingPressed = false;
		isBeingHovered = false;
		locked = other.locked;
		return *this;
	}

	GuiButton::GuiButton(GuiButton&& other) noexcept
		: GuiDynamicTextBox(std::move(other)), eventHandle(other.eventHandle), defaultColor(other.defaultColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor), lockedColor(other.lockedColor),
		isBeingPressed(other.isBeingPressed), isBeingHovered(other.isBeingHovered), locked(other.locked)
	{
		other.eventHandle = nullptr;
		if (eventHandle) notifyHandleOnGuiElementMove(&other, *eventHandle);
	}

	GuiButton& GuiButton::operator=(GuiButton&& other) noexcept
	{
		GuiDynamicTextBox::operator=(std::move(other));
		eventHandle = other.eventHandle;
		defaultColor = other.defaultColor;
		hoverColor = other.hoverColor;
		pressedColor = other.pressedColor;
		pressedHoverColor = other.pressedHoverColor;
		lockedColor = other.lockedColor;
		isBeingPressed = other.isBeingPressed;
		isBeingHovered = other.isBeingHovered;
		locked = other.locked;
		other.eventHandle = nullptr;
		if (eventHandle) notifyHandleOnGuiElementMove(&other, *eventHandle);
		return *this;
	}

	GuiButton::~GuiButton()
	{
		if (eventHandle)
			signOffHandle(*eventHandle);
	}

}