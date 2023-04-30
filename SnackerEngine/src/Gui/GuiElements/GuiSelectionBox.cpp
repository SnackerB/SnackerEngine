#include "Gui/GuiElements/GuiSelectionBox.h"
#include "Gui/GuiStyle.h"
#include "Gui/GuiManager.h"
#include "Core/Keys.h"

namespace SnackerEngine
{

	void GuiSelectionBox::draw(const Vec2i& worldPosition)
	{
		if (!isOpen) {
			pushClippingBox(worldPosition);
			drawElement(activeSelectionButton.getGuiID(), worldPosition + activeSelectionButton.getPosition());
			drawElement(downArrowButton.getGuiID(), worldPosition + downArrowButton.getPosition());
			GuiElement::draw(worldPosition);
			popClippingBox();
		}
	}

	void GuiSelectionBox::callbackDrawOnTop(const Vec2i& worldPosition)
	{
		if (isOpen) {
			drawElement(activeSelectionButton.getGuiID(), worldPosition + activeSelectionButton.getPosition());
			drawElement(downArrowButton.getGuiID(), worldPosition + downArrowButton.getPosition());
			for (auto& element : selectionOptionButtons) {
				drawElement(element.getGuiID(), worldPosition + element.getPosition());
			}
		}
	}

	void GuiSelectionBox::onRegister()
	{
		registerElementAsChild(activeSelectionButton);
		registerElementAsChild(downArrowButton);
		signUpHandle(activeSelectionButtonHandle, 1);
		for (unsigned int i = 0; i < selectionOptionButtons.size(); ++i) {
			registerElementAsChild(selectionOptionButtons[i]);
			signUpHandle(selectionOptionHandles[i], i + 2);
		}
	}

	void GuiSelectionBox::onSizeChange()
	{
		if (getWidth() < getHeight()) {
			// Set size of activeSelectionButton
			setSizeOfChild(activeSelectionButton.getGuiID(), Vec2i(0, getHeight()));
			// Set position and size of downArrowButton
			setPositionOfChild(downArrowButton.getGuiID(), Vec2i(0, 0));
			setSizeOfChild(downArrowButton.getGuiID(), getSize());
		}
		else {
			// Set size of activeSelectionButton
			setSizeOfChild(activeSelectionButton.getGuiID(), Vec2i(getWidth() - getHeight(), getHeight()));
			// Set position and size of downArrowButton
			setPositionOfChild(downArrowButton.getGuiID(), Vec2i(getWidth() - getHeight(), 0));
			setSizeOfChild(downArrowButton.getGuiID(), Vec2i(getHeight(), getHeight()));
		}
		Vec2i preferredSize = activeSelectionButton.getPreferredSize();
		preferredSize.x += preferredSize.y;
		Vec2i minSize = activeSelectionButton.getMinSize();
		minSize.x += minSize.y;
		// Set sizes of the selection options
		int offsetY = activeSelectionButton.getHeight();
		for (unsigned int i = 0; i < selectionOptionButtons.size(); ++i) {
			setPositionAndSizeOfChild(selectionOptionButtons[i].getGuiID(), Vec2i(0, offsetY), getSize());
			if (selectionOptionButtons[i].getPreferredWidth() > preferredSize.x)
				preferredSize.x = selectionOptionButtons[i].getPreferredWidth();
			if (selectionOptionButtons[i].getMinWidth() > minSize.x)
				minSize.x = selectionOptionButtons[i].getMinWidth();
			offsetY += activeSelectionButton.getHeight();
		}
		setPreferredSize(preferredSize);
		setMinSize(minSize);
	}

	GuiSelectionBox::IsCollidingResult GuiSelectionBox::isColliding(const Vec2i& offset)
	{
		if (offset.x >= 0 && offset.x <= getWidth() && offset.y >= 0) {
			if (offset.y <= getHeight()) {
				return IsCollidingResult::COLLIDE_CHILD;
			}
			else if (isOpen && offset.y <= getHeight() * (selectionOptionButtons.size() + 1)) {
				return IsCollidingResult::COLLIDE_CHILD;
			}
		}
		return IsCollidingResult::NOT_COLLIDING;
	}

	GuiSelectionBox::GuiID GuiSelectionBox::getCollidingChild(const Vec2i& offset)
	{
		if (offset.y <= activeSelectionButton.getHeight()) {
			if (offset.x <= activeSelectionButton.getWidth()) {
				return activeSelectionButton.getGuiID();
			}
			return downArrowButton.getGuiID();
		}
		if (isOpen) {
			int selectionOptionIndex = (offset.y - getHeight()) / getHeight();
			if (0 <= selectionOptionIndex && selectionOptionIndex < selectionOptionButtons.size()) {
				return selectionOptionButtons[selectionOptionIndex].getGuiID();
			}
		}
		return -1;
	}

	void GuiSelectionBox::onHandleDestruction(GuiHandle& guiHandle)
	{
		selectionHandle = nullptr;
	}

	void GuiSelectionBox::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		auto handleID = guiHandle.getHandleID(*this);
		if (!handleID.has_value()) return;
		if (handleID.value() == 0) {
			selectionHandle = static_cast<GuiVariableHandle<int>*>(&guiHandle);
		}
	}

	void GuiSelectionBox::onHandleUpdate(GuiHandle& guiHandle)
	{
		auto handleID = guiHandle.getHandleID(*this);
		if (!handleID.has_value()) return;
		if (handleID.value() == 0) {
			// selectionHandle
			if (selectionHandle) {
				int selectedIndex = selectionHandle->get();
				if (selectedIndex >= 0 && selectedIndex < selectionOptionButtons.size()) {
					activeSelectionButton.setText(selectionOptionButtons[selectedIndex].getText());
				}
				else {
					selectedIndex = std::min(std::max(0, selectedIndex), static_cast<int>(selectionOptionButtons.size() - 1));
					selectionHandle->set(selectedIndex);
					activeSelectionButton.setText(selectionOptionButtons[selectedIndex].getText());
				}
			}
		}
		else if (handleID.value() == 1) {
			// activeSelectionButton or downArrowButton was pressed
			if (isOpen) {
				isOpen = false;
				signOffEvent(CallbackType::DRAW_ON_TOP);
				signOffEvent(CallbackType::MOUSE_BUTTON);
			}
			else {
				isOpen = true;
				signUpEvent(CallbackType::DRAW_ON_TOP);
				signUpEvent(CallbackType::MOUSE_BUTTON);
			}
		}
		else if (handleID.value() >= 2 && handleID.value() <= selectionOptionButtons.size() + 1 && isOpen) {
			// An option button was pressed
			int selectedIndex = handleID.value() - 2;
			activeSelectionButton.setText(selectionOptionButtons[selectedIndex].getText());
			isOpen = false;
			signOffEvent(CallbackType::DRAW_ON_TOP);
			if (selectionHandle) {
				selectionHandle->set(selectedIndex);
			}
		}
	}

	void GuiSelectionBox::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS && isOpen && isColliding(getMouseOffset(getGuiID())) == IsCollidingResult::NOT_COLLIDING)
		{
			isOpen = false;
			signOffEvent(CallbackType::DRAW_ON_TOP);
			signOffEvent(CallbackType::MOUSE_BUTTON);
		}
	}

	GuiSelectionBox::GuiSelectionBox(const Vec2i& position, const Vec2i& size, const std::string& defaultLabel, const std::vector<std::string>& options, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& defaultColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, int border)
		: GuiElement(position, size, ResizeMode::RESIZE_RANGE), activeSelectionButton{}, activeSelectionButtonHandle{}, downArrowButton{},
		selectionOptionButtons{}, selectionOptionHandles{}, isOpen(false), selectionHandle(nullptr)
	{
		GuiButton templateButton(Vec2i(), Vec2i(), ResizeMode::RESIZE_RANGE, defaultColor, hoverColor, pressedColor, pressedHoverColor, {}, "", font,
			fontSize, textColor, StaticText::ParseMode::SINGLE_LINE, StaticText::Alignment::CENTER, border, GuiDynamicTextBox::TextScaleMode::DONT_SCALE);
		templateButton.setSizeHintModePreferredSize(GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_SIZE);
		templateButton.setSizeHintModeMinSize(GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_SIZE);
		// Setup selectionButton
		activeSelectionButton = templateButton;
		activeSelectionButton.setText(defaultLabel);
		activeSelectionButton.setEventHandle(activeSelectionButtonHandle);
		// Setup downArrowButton
		downArrowButton = templateButton;
		downArrowButton.setEventHandle(activeSelectionButtonHandle);
		// Setup selection option buttons
		for (unsigned int i = 0; i < options.size(); ++i)
		{
			selectionOptionButtons.push_back(GuiButton(templateButton));
			selectionOptionHandles.push_back(GuiEventHandle());
			selectionOptionButtons.back().setText(options[i]);
			selectionOptionButtons.back().setEventHandle(selectionOptionHandles.back());
		}
		onSizeChange();
	}

	GuiSelectionBox::GuiSelectionBox(GuiVariableHandle<int>& handle, const Vec2i& position, const Vec2i& size, const std::string& defaultLabel, const std::vector<std::string>& options, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& defaultColor, const Color4f& hoverColor, const Color4f& pressedColor, const Color4f& pressedHoverColor, int border)
		: GuiSelectionBox(position, size, defaultLabel, options, font, fontSize, textColor, defaultColor, hoverColor, pressedColor, pressedHoverColor, border)
	{
		setVariableHandle(handle);
	}

	void GuiSelectionBox::setVariableHandle(GuiVariableHandle<int>& variableHandle)
	{
		if (!this->selectionHandle) {
			this->selectionHandle = &variableHandle;
			variableHandle.set(-1);
			signUpHandle(variableHandle, 0);
		}
	}

	GuiSelectionBox::GuiSelectionBox(const std::string& defaultLabel, const std::vector<std::string>& options, const GuiStyle& style)
		: GuiSelectionBox(Vec2i(), Vec2i(), defaultLabel, options, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor,
			style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor,
			style.guiButtonBorder) {}

	GuiSelectionBox::GuiSelectionBox(GuiVariableHandle<int>& handle, const std::string& defaultLabel, const std::vector<std::string>& options, const GuiStyle& style)
		: GuiSelectionBox(handle, Vec2i(), Vec2i(), defaultLabel, options, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor,
			style.guiButtonBackgroundColor, style.guiButtonHoverColor, style.guiButtonPressedColor, style.guiButtonPressedHoverColor,
			style.guiButtonBorder) {}

	GuiSelectionBox::GuiSelectionBox(const GuiSelectionBox& other) noexcept
		: GuiElement(other), activeSelectionButton(other.activeSelectionButton), activeSelectionButtonHandle(),
		downArrowButton(other.downArrowButton), selectionOptionButtons{}, selectionOptionHandles{}, isOpen(false), selectionHandle(nullptr)
	{
		signUpHandle(activeSelectionButtonHandle, 0);
		activeSelectionButton.setEventHandle(activeSelectionButtonHandle);
		downArrowButton.setEventHandle(activeSelectionButtonHandle);
		for (unsigned int i = 0; i < other.selectionOptionButtons.size(); ++i) {
			selectionOptionButtons.push_back(GuiButton(other.selectionOptionButtons[i]));
			selectionOptionHandles.push_back(GuiEventHandle());
			signUpHandle(selectionOptionHandles.back(), i + 1);
			selectionOptionButtons.back().setEventHandle(selectionOptionHandles.back());
		}
	}

	GuiSelectionBox& GuiSelectionBox::operator=(const GuiSelectionBox& other) noexcept
	{
		GuiElement::operator=(other);
		activeSelectionButton = other.activeSelectionButton;
		activeSelectionButtonHandle = GuiEventHandle();
		downArrowButton = other.downArrowButton;
		selectionOptionButtons.clear();
		selectionOptionHandles.clear();
		isOpen = false;
		if (selectionHandle) {
			signOffHandle(*selectionHandle);
			selectionHandle = nullptr;
		}
		signUpHandle(activeSelectionButtonHandle, 0);
		activeSelectionButton.setEventHandle(activeSelectionButtonHandle);
		downArrowButton.setEventHandle(activeSelectionButtonHandle);
		for (unsigned int i = 0; i < other.selectionOptionButtons.size(); ++i) {
			selectionOptionButtons.push_back(GuiButton(other.selectionOptionButtons[i]));
			selectionOptionHandles.push_back(GuiEventHandle());
			signUpHandle(selectionOptionHandles.back(), i + 1);
			selectionOptionButtons.back().setEventHandle(selectionOptionHandles.back());
		}
		return *this;
	}

	GuiSelectionBox::GuiSelectionBox(GuiSelectionBox&& other) noexcept
		: GuiElement(std::move(other)), activeSelectionButton(std::move(other.activeSelectionButton)), activeSelectionButtonHandle(std::move(other.activeSelectionButtonHandle)),
		downArrowButton(std::move(other.downArrowButton)), selectionOptionButtons(std::move(other.selectionOptionButtons)), selectionOptionHandles(std::move(other.selectionOptionHandles)),
		isOpen(other.isOpen), selectionHandle(other.selectionHandle)
	{
		if (selectionHandle) notifyHandleOnGuiElementMove(&other, *selectionHandle);
		other.selectionHandle = nullptr;
		notifyHandleOnGuiElementMove(&other, activeSelectionButtonHandle);
		for (auto& handle : selectionOptionHandles) {
			notifyHandleOnGuiElementMove(&other, handle);
		}
	}

	GuiSelectionBox& GuiSelectionBox::operator=(GuiSelectionBox&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		activeSelectionButton = std::move(other.activeSelectionButton);
		activeSelectionButtonHandle = std::move(other.activeSelectionButtonHandle);
		downArrowButton = std::move(other.downArrowButton);
		selectionOptionButtons.clear();
		selectionOptionHandles.clear();
		isOpen = other.isOpen;
		selectionHandle = other.selectionHandle;
		if (selectionHandle) notifyHandleOnGuiElementMove(&other, *selectionHandle);
		other.selectionHandle = nullptr;
		selectionOptionButtons = std::move(other.selectionOptionButtons);
		selectionOptionHandles = std::move(other.selectionOptionHandles);
		notifyHandleOnGuiElementMove(&other, activeSelectionButtonHandle);
		for (auto& handle : selectionOptionHandles) {
			notifyHandleOnGuiElementMove(&other, handle);
		}
		return *this;
	}

	GuiSelectionBox::~GuiSelectionBox()
	{
		if (selectionHandle) {
			signOffHandle(*selectionHandle);
			selectionHandle = nullptr;
		}
	}

}