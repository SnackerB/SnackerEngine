#include "Gui/GuiElements/GuiButton.h"
#include "Utility\Keys.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	Color4f GuiButton::defaultDefaultColor = Color4f(0.5f, 0.5f, 0.7f, 1.0f);
	Color4f GuiButton::defaultHoverColor = scaleRGB(GuiButton::defaultDefaultColor, 0.9f);
	Color4f GuiButton::defaultPressedColor = scaleRGB(GuiButton::defaultDefaultColor, 0.8f); 
	Color4f GuiButton::defaultPressedHoverColor = scaleRGB(GuiButton::defaultDefaultColor, 0.7f); 
	Color4f GuiButton::defaultLockedColor = Color4f(0.5f, 1.0f);
	//--------------------------------------------------------------------------------------------------
	void GuiButton::onButtonPress()
	{
		if (eventHandle) activate(*eventHandle);
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const Vec2i& position, const Vec2i& size, const std::string& text)
		: GuiTextBox(position, size, text)
	{
		if (size.y == 0 && !text.empty()) {
			setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setResizeMode(ResizeMode::RESIZE_RANGE);
		}
		setAlignment(StaticText::Alignment::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const std::string& text)
		: GuiButton(Vec2i(), Vec2i(), text) {}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		parseJsonOrReadFromData(defaultColor, "defaultColor", json, data, parameterNames);
		parseJsonOrReadFromData(hoverColor, "hoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(pressedColor, "pressedColor", json, data, parameterNames);
		parseJsonOrReadFromData(pressedHoverColor, "pressedHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(lockedColor, "lockedColor", json, data, parameterNames);
		parseJsonOrReadFromData(locked, "locked", json, data, parameterNames);
		if (!json.contains("size")) {
			if (!getText().empty()) {
				if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
				if (!json.contains("sizeHintModeMinSize")) setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			}
			if (!json.contains("resizeMode")) setResizeMode(ResizeMode::RESIZE_RANGE);
		}
		if (!json.contains("alignment")) setAlignment(StaticText::Alignment::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::~GuiButton()
	{
		if (eventHandle)
			signOffHandle(*eventHandle);
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const GuiButton& other) noexcept
		: GuiTextBox(other), eventHandle(nullptr), defaultColor(other.defaultColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor), lockedColor(other.lockedColor),
		isBeingPressed(false), isBeingHovered(false), locked(other.locked) {}
	//--------------------------------------------------------------------------------------------------
	GuiButton& GuiButton::operator=(const GuiButton& other) noexcept
	{
		GuiTextBox::operator=(other);
		if (eventHandle) signOffHandle(*eventHandle);
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
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(GuiButton&& other) noexcept
		: GuiTextBox(std::move(other)), eventHandle(other.eventHandle), defaultColor(other.defaultColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor), lockedColor(other.lockedColor),
		isBeingPressed(other.isBeingPressed), isBeingHovered(other.isBeingHovered), locked(other.locked)
	{
		other.eventHandle = nullptr;
		if (eventHandle) notifyHandleOnGuiElementMove(&other, *eventHandle);
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton& GuiButton::operator=(GuiButton&& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		if (eventHandle) signOffHandle(*eventHandle);
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
	//--------------------------------------------------------------------------------------------------
	bool GuiButton::setEventHandle(GuiEventHandle& eventHandle)
	{
		if (this->eventHandle) return false;
		this->eventHandle = &eventHandle;
		signUpHandle(eventHandle, 0);
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::lock()
	{
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		isBeingPressed = false;
		isBeingHovered = false;
		locked = true;
		setBackgroundColor(lockedColor);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::unlock()
	{
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		locked = false;
		if (isBeingHovered)
			setBackgroundColor(hoverColor);
		else
			setBackgroundColor(defaultColor);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setDefaultColor(const Color4f& defaultColor)
	{
		this->defaultColor = defaultColor;
		if (locked == false && isBeingHovered == false && isBeingPressed == false) {
			setBackgroundColor(this->defaultColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setHoverColor(const Color4f& hoverColor)
	{
		this->hoverColor = hoverColor;
		if (locked == false && isBeingHovered == true && isBeingPressed == false) {
			setBackgroundColor(this->hoverColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setPressedColor(const Color4f& pressedColor)
	{
		this->pressedColor = pressedColor;
		if (locked == false && isBeingHovered == false && isBeingPressed == true) {
			setBackgroundColor(this->pressedColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setPressedHoverColor(const Color4f& pressedHoverColor)
	{
		this->pressedHoverColor = pressedHoverColor;
		if (locked == false && isBeingHovered == true && isBeingPressed == true) {
			setBackgroundColor(this->pressedHoverColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setLockedColor(const Color4f& lockedColor)
	{
		this->lockedColor = lockedColor;
		if (locked) {
			setBackgroundColor(this->lockedColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::onRegister()
	{
		GuiTextBox::onRegister();
		setBackgroundColor(defaultColor);
		if (!locked) {
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		}
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		eventHandle = static_cast<GuiEventHandle*>(&guiHandle);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::onHandleDestruction(GuiHandle& guiHandle)
	{
		eventHandle = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::IsCollidingResult GuiButton::isColliding(const Vec2i& offset) const
	{
		return (offset.x > 0 && offset.x < getWidth()
			&& offset.y > 0 && offset.y < getHeight()) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE)
		{
			if (isBeingHovered)
			{
				onButtonPress();
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
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			isBeingPressed = true;
			setBackgroundColor(pressedHoverColor);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseEnter(const Vec2d& position)
	{
		isBeingHovered = true;
		if (!locked) {
			if (isBeingPressed) {
				setBackgroundColor(pressedHoverColor);
			}
			else {
				setBackgroundColor(hoverColor);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseLeave(const Vec2d& position)
	{
		isBeingHovered = false;
		if (!locked) {
			if (isBeingPressed) {
				setBackgroundColor(pressedColor);
			}
			else {
				setBackgroundColor(defaultColor);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
}