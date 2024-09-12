#include "Gui\GuiElements\GuiSelectionBox.h"
#include "Gui\GuiManager.h"
#include "Utility\Keys.h"

namespace SnackerEngine
{

	Color4f GuiSelectionBox::defaultOptionButtonColor = scaleRGB(Color4f(0.5f, 0.5f, 0.7f, 1.0f), 0.8f);
	Color4f GuiSelectionBox::defaultOptionButtonPressedColor = scaleRGB(GuiSelectionBox::defaultOptionButtonColor, 0.8f);
	Color4f GuiSelectionBox::defaultOptionButtonHoverColor = scaleRGB(GuiSelectionBox::defaultOptionButtonColor, 0.9f);
	Color4f GuiSelectionBox::defaultOptionButtonPressedHoverColor = scaleRGB(GuiSelectionBox::defaultOptionButtonColor, 0.7f);

	Color4f GuiSelectionBox::defaultSelectedOptionButtonColor = Color4f(0.5f, 0.5f, 0.7f, 1.0f);
	Color4f GuiSelectionBox::defaultSelectedOptionButtonPressedColor = scaleRGB(GuiSelectionBox::defaultSelectedOptionButtonColor, 0.8f);
	Color4f GuiSelectionBox::defaultSelectedOptionButtonHoverColor = scaleRGB(GuiSelectionBox::defaultSelectedOptionButtonColor, 0.9f);
	Color4f GuiSelectionBox::defaultSelectedOptionButtonPressedHoverColor = scaleRGB(GuiSelectionBox::defaultSelectedOptionButtonColor, 0.7f);
	
	Color4f GuiSelectionBox::defaultArrowButtonMenuClosedColor = scaleRGB(Color4f(0.5f, 0.5f, 0.7f, 1.0f), 0.9f);
	Color4f GuiSelectionBox::defaultArrowButtonMenuClosedPressedColor = scaleRGB(GuiSelectionBox::defaultArrowButtonMenuClosedColor, 0.8f);
	Color4f GuiSelectionBox::defaultArrowButtonMenuClosedHoverColor = scaleRGB(GuiSelectionBox::defaultArrowButtonMenuClosedColor, 0.9f);
	Color4f GuiSelectionBox::defaultArrowButtonMenuClosedPressedHoverColor = scaleRGB(GuiSelectionBox::defaultArrowButtonMenuClosedColor, 0.7f);
	
	Color4f GuiSelectionBox::defaultArrowButtonMenuOpenColor = scaleRGB(Color4f(0.5f, 0.5f, 0.7f, 1.0f), 0.9f);
	Color4f GuiSelectionBox::defaultArrowButtonMenuOpenPressedColor = scaleRGB(GuiSelectionBox::defaultArrowButtonMenuOpenColor, 0.8f);
	Color4f GuiSelectionBox::defaultArrowButtonMenuOpenHoverColor = scaleRGB(GuiSelectionBox::defaultArrowButtonMenuOpenColor, 0.9f);
	Color4f GuiSelectionBox::defaultArrowButtonMenuOpenPressedHoverColor = scaleRGB(GuiSelectionBox::defaultArrowButtonMenuOpenColor, 0.7f);
	
	Color4f GuiSelectionBox::defaultTextColor = Color4f(1.0f, 1.0f);

	void GuiSelectionBox::notifyEvent(int eventID)
	{
		if (optionListLayout && optionListLayout->isValid()) {
			// the selectionBox is currently open
			if (eventID < 0) {
				// currently selected option button or arrow button was pressed.
				// We just need to close the selectionBox
				arrowButtonEventHandle.reset();
			}
			else if (eventID < options.size()) {
				// An option button was pressed. Change the currently selected
				// option and close the selectionBox
				selectedOptionIndex.set(eventID);
				selectedOption.setText(options[eventID]);
			}
			closeSelectionBox();
		}
		else {
			// the selectionBox is currently closed
			if (eventID < 0) {
				// currently selected option button or arrow button was pressed.
				// We just need to open the selectionBox
				arrowButtonEventHandle.reset();
				openSelectionBox();
			}
		}
	}

	void GuiSelectionBox::computeSizeHints()
	{
		switch (sizeHintModes.sizeHintModeMinSize)
		{
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setMinHeight(selectedOption.getMinHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: setMinWidth(selectedOption.getMinHeight() + minimumWidthOfLongestOption); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setMinSize(Vec2i(selectedOption.getMinHeight() + minimumWidthOfLongestOption, selectedOption.getMinHeight())); break;
		default: break;
		}
		switch (sizeHintModes.sizeHintModeMaxSize)
		{
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setMaxHeight(selectedOption.getMinHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: setMaxWidth(selectedOption.getMinHeight() + minimumWidthOfLongestOption); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setMaxSize(Vec2i(selectedOption.getMinHeight() + minimumWidthOfLongestOption, selectedOption.getMinHeight())); break;
		default: break;
		}
		switch (sizeHintModes.sizeHintModePreferredSize)
		{
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setPreferredHeight(selectedOption.getMinHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: setPreferredWidth(selectedOption.getMinHeight() + minimumWidthOfLongestOption); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setPreferredSize(Vec2i(selectedOption.getMinHeight() + minimumWidthOfLongestOption, selectedOption.getMinHeight())); break;
		default:
			break;
		}
	}

	void GuiSelectionBox::openSelectionBox()
	{
		if (optionListLayout && optionListLayout->isValid()) return;
		GuiManager* guiManager = getGuiManager();
		if (!guiManager) return;
		optionListLayout = std::make_unique<GuiVerticalListLayout>();
		optionListLayout->setVerticalBorder(0);
		optionListLayout->setOuterVerticalBorder(0);
		optionListLayout->setHorizontalBorder(0);
		optionListLayout->setResizeMode(ResizeMode::RESIZE_RANGE);
		optionListLayout->setSize(Vec2i(getWidth(), getHeight() * options.size()));
		optionListLayout->setPosition(getWorldPosition() + Vec2i(0, getHeight()));
		guiManager->registerElement(*optionListLayout);
		// Push back option buttons
		GuiButton templateButton(selectedOption);
		templateButton.setSize(getSize());
		templateButton.setPreferredSize(getSize());
		templateButton.setSizeHintModePreferredSize(GuiTextBox::SizeHintMode::ARBITRARY);
		templateButton.setDefaultColor(optionButtonColor);
		templateButton.setPressedColor(optionButtonPressedColor);
		templateButton.setHoverColor(optionButtonHoverColor);
		templateButton.setPressedHoverColor(optionButtonPressedHoverColor);
		templateButton.setTextColor(optionButtonTextColor);
		templateButton.setBorder(border);
		templateButton.setFontSize(fontSize);
		templateButton.setFont(font);
		optionButtonEventHandles.clear(); // Clear handles
		for (std::size_t i = 0; i < options.size(); ++i) {
			GuiButton optionButton(templateButton);
			optionButton.setText(options[i]);
			optionButtonEventHandles.push_back(GuiSelectionBoxEventHandle(*this, i));
			optionButton.subscribeToEventButtonPress(optionButtonEventHandles.back());
			optionListLayout->registerChild(optionButton);
			guiManager->moveElement<GuiButton>(std::move(optionButton));
		}
		// change arrow button colors
		arrowButton.setDefaultColor(arrowButtonMenuOpenColor);
		arrowButton.setPressedColor(arrowButtonMenuOpenPressedColor);
		arrowButton.setHoverColor(arrowButtonMenuOpenHoverColor);
		arrowButton.setPressedHoverColor(arrowButtonMenuOpenPressedHoverColor);
		arrowButton.setTextColor(arrowButtonMenuOpenTextColor);
		// bring to foreground
		bringToForeground();
		optionListLayout->bringToForeground();
		// Register for mouse callback
		signUpEvent(CallbackType::MOUSE_BUTTON);
	}

	void GuiSelectionBox::closeSelectionBox()
	{
		if (!optionListLayout || !optionListLayout->isValid()) return;
		optionListLayout->signOffAtNextUpdate();
		// change arrow button colors
		arrowButton.setDefaultColor(arrowButtonMenuClosedColor);
		arrowButton.setPressedColor(arrowButtonMenuClosedPressedColor);
		arrowButton.setHoverColor(arrowButtonMenuClosedHoverColor);
		arrowButton.setPressedHoverColor(arrowButtonMenuClosedPressedHoverColor);
		arrowButton.setTextColor(arrowButtonMenuClosedTextColor);
		// Sign off callback
		signOffEvent(CallbackType::MOUSE_BUTTON);
	}

	void GuiSelectionBox::draw(const Vec2i& worldPosition)
	{
		pushClippingBox(worldPosition);
		drawElement(selectedOption.getGuiID(), worldPosition);
		drawElement(arrowButton.getGuiID(), worldPosition + Vec2i(selectedOption.getWidth(), 0));
		popClippingBox();
	}

	void GuiSelectionBox::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (action == ACTION_PRESS) {
			GuiManager* guiManager = getGuiManager();
			if (guiManager && (guiManager->isMouseHoveringOver(getGuiID()) || guiManager->isMouseHoveringOver(optionListLayout->getGuiID()))) return;
			closeSelectionBox();
			signOffEvent(CallbackType::MOUSE_BUTTON);
		}
	}

	void GuiSelectionBox::onRegister()
	{
		// Set attributes of selectedOption button
		selectedOption.setDefaultColor(selectedOptionButtonColor);
		selectedOption.setHoverColor(selectedOptionButtonHoverColor);
		selectedOption.setPressedColor(selectedOptionButtonPressedColor);
		selectedOption.setPressedHoverColor(selectedOptionButtonPressedHoverColor);
		selectedOption.setTextColor(selectedOptionButtonTextColor);
		selectedOption.setBorder(border);
		selectedOption.setFontSize(fontSize);
		selectedOption.setFont(font);
		selectedOption.setAlignment(StaticText::Alignment::LEFT);
		selectedOption.setSizeHintModeMinSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE);
		// Set attributes of arrowButton
		arrowButton.setDefaultColor(arrowButtonMenuClosedColor);
		arrowButton.setHoverColor(arrowButtonMenuClosedHoverColor);
		arrowButton.setPressedColor(arrowButtonMenuClosedPressedColor);
		arrowButton.setPressedHoverColor(arrowButtonMenuClosedPressedHoverColor);
		arrowButton.setTextColor(arrowButtonMenuClosedTextColor);
		arrowButton.setBorder(border);
		arrowButton.setFontSize(fontSize);
		arrowButton.setFont(font);
		arrowButton.setSizeHintModeMinSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE);
		arrowButton.setText("v");
		// Register child elements
		registerElementAsChild(selectedOption);
		registerElementAsChild(arrowButton);
		// Set event handles
		selectedOption.subscribeToEventButtonPress(arrowButtonEventHandle);
		arrowButton.subscribeToEventButtonPress(arrowButtonEventHandle);
		// Obtain information on width
		for (const std::string& option : options) {
			// Compute the size of the text box
			selectedOption.setText(option);
			if (selectedOption.getMinWidth() > minimumWidthOfLongestOption) minimumWidthOfLongestOption = selectedOption.getMinWidth();
		}
		if (!options.empty()) selectedOption.setText(options[selectedOptionIndex]);
		// Finalize
		computeSizeHints();
		onSizeChange();
	}

	void GuiSelectionBox::onSizeChange()
	{
		computeSizeHints();
		arrowButton.setPositionAndSize(Vec2i(getWidth() - getHeight(), 0), Vec2i(getHeight(), getHeight()));
		selectedOption.setPositionAndSize(Vec2i(0, 0), Vec2i(getWidth() - arrowButton.getWidth(), getHeight()));
	}

	GuiElement::GuiID GuiSelectionBox::getCollidingChild(const Vec2i& offset) const
	{
		GuiID result = GuiElement::getCollidingChild(offset);
		if (result > 0) return result;
		IsCollidingResult collidingResult = selectedOption.isColliding(offset - selectedOption.getPosition());
		GuiID childCollision = GuiElement::getCollidingChild(collidingResult, selectedOption.getGuiID(), offset);
		if (childCollision > 0) return childCollision;
		collidingResult = arrowButton.isColliding(offset - arrowButton.getPosition());
		childCollision = GuiElement::getCollidingChild(collidingResult, arrowButton.getGuiID(), offset);
		if (childCollision > 0) return childCollision;
		return 0;
	}

	void GuiSelectionBox::updateSelection()
	{
		selectedOption.setText(options[std::min(selectedOptionIndex.get(), options.size() - 1)]);
		closeSelectionBox();
	}

	GuiSelectionBox::GuiSelectionBox(const std::vector<std::string>& options)
		: GuiElement(), options(options)
	{
	}

	GuiSelectionBox::GuiSelectionBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiElement(json, data, parameterNames)
	{
		if (json.contains("options") && json["options"].is_array()) {
			for (const auto& option : json["options"]) {
				if (option.is_string()) options.push_back(option);
			}
			if (parameterNames) parameterNames->erase("options");
		}
		std::size_t selectedOptionIndexTemp = 0;
		parseJsonOrReadFromData(selectedOptionIndexTemp, "selectedOptionIndex", json, data, parameterNames);
		selectedOptionIndex.set(selectedOptionIndexTemp);
		if (!options.empty()) selectedOptionIndex.set(std::min(selectedOptionIndex.get(), options.size() - 1));
		parseJsonOrReadFromData(optionButtonColor, "optionButtonColor", json, data, parameterNames);
		parseJsonOrReadFromData(optionButtonPressedColor, "optionButtonPressedColor", json, data, parameterNames);
		parseJsonOrReadFromData(optionButtonHoverColor, "optionButtonHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(optionButtonPressedHoverColor, "optionButtonPressedHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionButtonColor, "selectedOptionButtonColor", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionButtonPressedColor, "selectedOptionButtonPressedColor", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionButtonHoverColor, "selectedOptionButtonHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionButtonPressedHoverColor, "selectedOptionButtonPressedHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuClosedColor, "arrowButtonMenuClosedColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuClosedPressedColor, "arrowButtonMenuClosedPressedColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuClosedHoverColor, "arrowButtonMenuClosedHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuClosedPressedHoverColor, "arrowButtonMenuClosedPressedHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuOpenColor, "arrowButtonMenuOpenColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuOpenPressedColor, "arrowButtonMenuOpenPressedColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuOpenHoverColor, "arrowButtonMenuOpenHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuOpenPressedHoverColor, "arrowButtonMenuOpenPressedHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(optionButtonTextColor, "optionButtonTextColor", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionButtonTextColor, "selectedOptionButtonTextColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuClosedTextColor, "arrowButtonMenuClosedTextColor", json, data, parameterNames);
		parseJsonOrReadFromData(arrowButtonMenuOpenTextColor, "arrowButtonMenuOpenTextColor", json, data, parameterNames);
		parseJsonOrReadFromData(border, "border", json, data, parameterNames);
		parseJsonOrReadFromData(fontSize, "fontSize", json, data, parameterNames);
		parseJsonOrReadFromData(font, "font", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHintModes.sizeHintModeMinSize, "sizeHintModeMinSize", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHintModes.sizeHintModeMaxSize, "sizeHintModeMaxSize", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHintModes.sizeHintModePreferredSize, "sizeHintModePreferredSize", json, data, parameterNames);
	}

	GuiSelectionBox::~GuiSelectionBox()
	{
		closeSelectionBox();
	}

	GuiSelectionBox::GuiSelectionBox(const GuiSelectionBox& other) noexcept
		: GuiElement(other), options(other.options), selectedOption(other.selectedOption), arrowButton(other.arrowButton),
		selectedOptionIndex(other.selectedOptionIndex), optionListLayout(nullptr), optionButtonEventHandles(),
		arrowButtonEventHandle(*this, -1), minimumWidthOfLongestOption(other.minimumWidthOfLongestOption),
		optionButtonColor(other.optionButtonColor), optionButtonPressedColor(other.optionButtonPressedColor),
		optionButtonHoverColor(other.optionButtonHoverColor), optionButtonPressedHoverColor(other.optionButtonPressedHoverColor),
		selectedOptionButtonColor(other.selectedOptionButtonColor), 
		selectedOptionButtonPressedColor(other.selectedOptionButtonPressedColor), 
		selectedOptionButtonHoverColor(other.selectedOptionButtonHoverColor), 
		selectedOptionButtonPressedHoverColor(other.selectedOptionButtonPressedHoverColor),
		arrowButtonMenuClosedColor(other.arrowButtonMenuClosedColor),
		arrowButtonMenuClosedPressedColor(other.arrowButtonMenuClosedPressedColor),
		arrowButtonMenuClosedHoverColor(other.arrowButtonMenuClosedHoverColor),
		arrowButtonMenuClosedPressedHoverColor(other.arrowButtonMenuClosedPressedHoverColor),
		arrowButtonMenuOpenColor(other.arrowButtonMenuOpenColor),
		arrowButtonMenuOpenPressedColor(other.arrowButtonMenuOpenPressedColor),
		arrowButtonMenuOpenHoverColor(other.arrowButtonMenuOpenHoverColor),
		arrowButtonMenuOpenPressedHoverColor(other.arrowButtonMenuOpenPressedHoverColor),
		optionButtonTextColor(other.optionButtonTextColor), selectedOptionButtonTextColor(other.selectedOptionButtonTextColor),
		arrowButtonMenuClosedTextColor(other.arrowButtonMenuClosedTextColor), 
		arrowButtonMenuOpenTextColor(other.arrowButtonMenuOpenTextColor), border(other.border), fontSize(other.fontSize),
		font(other.font)
	{
	}

	GuiSelectionBox& GuiSelectionBox::operator=(const GuiSelectionBox& other) noexcept
	{
		closeSelectionBox();
		GuiElement::operator=(other);
		options = other.options;
		selectedOption = other.selectedOption;
		arrowButton = other.arrowButton;
		selectedOptionIndex = other.selectedOptionIndex;
		optionListLayout = nullptr;
		optionButtonEventHandles.clear();
		arrowButtonEventHandle = GuiSelectionBoxEventHandle(*this, -1);
		minimumWidthOfLongestOption = other.minimumWidthOfLongestOption;
		optionButtonColor = other.optionButtonColor;
		optionButtonPressedColor = other.optionButtonPressedColor;
		optionButtonHoverColor = other.optionButtonHoverColor;
		optionButtonPressedHoverColor = other.optionButtonPressedHoverColor;
		selectedOptionButtonColor = other.selectedOptionButtonColor;
		selectedOptionButtonPressedColor = other.selectedOptionButtonPressedColor;
		selectedOptionButtonHoverColor = other.selectedOptionButtonHoverColor;
		selectedOptionButtonPressedHoverColor = other.selectedOptionButtonPressedHoverColor;
		arrowButtonMenuClosedColor = other.arrowButtonMenuClosedColor;
		arrowButtonMenuClosedPressedColor = other.arrowButtonMenuClosedPressedColor;
		arrowButtonMenuClosedHoverColor = other.arrowButtonMenuClosedHoverColor;
		arrowButtonMenuClosedPressedHoverColor = other.arrowButtonMenuClosedPressedHoverColor;
		arrowButtonMenuOpenColor = other.arrowButtonMenuOpenColor;
		arrowButtonMenuOpenPressedColor = other.arrowButtonMenuOpenPressedColor;
		arrowButtonMenuOpenHoverColor = other.arrowButtonMenuOpenHoverColor;
		arrowButtonMenuOpenPressedHoverColor = other.arrowButtonMenuOpenPressedHoverColor;
		optionButtonTextColor = other.optionButtonTextColor; 
		selectedOptionButtonTextColor = other.selectedOptionButtonTextColor;
		arrowButtonMenuClosedTextColor = other.arrowButtonMenuClosedTextColor;
		arrowButtonMenuOpenTextColor = other.arrowButtonMenuOpenTextColor;
		border = other.border;
		fontSize = other.fontSize;
		font = other.font;
		return *this;
	}

	GuiSelectionBox::GuiSelectionBox(GuiSelectionBox&& other) noexcept
		: GuiElement(std::move(other)), options(std::move(other.options)), selectedOption(std::move(other.selectedOption)),
		arrowButton(std::move(other.arrowButton)), selectedOptionIndex(std::move(other.selectedOptionIndex)),
		optionListLayout(std::move(other.optionListLayout)), optionButtonEventHandles(std::move(other.optionButtonEventHandles)),
		arrowButtonEventHandle(std::move(other.arrowButtonEventHandle)), minimumWidthOfLongestOption(std::move(other.minimumWidthOfLongestOption)),
		optionButtonColor(std::move(other.optionButtonColor)), optionButtonPressedColor(std::move(other.optionButtonPressedColor)),
		optionButtonHoverColor(std::move(other.optionButtonHoverColor)), optionButtonPressedHoverColor(std::move(other.optionButtonPressedHoverColor)),
		selectedOptionButtonColor(std::move(other.selectedOptionButtonColor)),
		selectedOptionButtonPressedColor(std::move(other.selectedOptionButtonPressedColor)),
		selectedOptionButtonHoverColor(std::move(other.selectedOptionButtonHoverColor)),
		selectedOptionButtonPressedHoverColor(std::move(other.selectedOptionButtonPressedHoverColor)),
		arrowButtonMenuClosedColor(std::move(other.arrowButtonMenuClosedColor)),
		arrowButtonMenuClosedPressedColor(std::move(other.arrowButtonMenuClosedPressedColor)),
		arrowButtonMenuClosedHoverColor(std::move(other.arrowButtonMenuClosedHoverColor)),
		arrowButtonMenuClosedPressedHoverColor(std::move(other.arrowButtonMenuClosedPressedHoverColor)),
		arrowButtonMenuOpenColor(std::move(other.arrowButtonMenuOpenColor)),
		arrowButtonMenuOpenPressedColor(std::move(other.arrowButtonMenuOpenPressedColor)),
		arrowButtonMenuOpenHoverColor(std::move(other.arrowButtonMenuOpenHoverColor)),
		arrowButtonMenuOpenPressedHoverColor(std::move(other.arrowButtonMenuOpenPressedHoverColor)),
		optionButtonTextColor(std::move(other.optionButtonTextColor)), selectedOptionButtonTextColor(std::move(other.selectedOptionButtonTextColor)),
		arrowButtonMenuClosedTextColor(std::move(other.arrowButtonMenuClosedTextColor)),
		arrowButtonMenuOpenTextColor(std::move(other.arrowButtonMenuOpenTextColor)), border(std::move(other.border)), fontSize(std::move(other.fontSize)),
		font(std::move(other.font))
	{
		arrowButtonEventHandle.guiSelectionBox = this;
		for (auto& optionHandle : optionButtonEventHandles) optionHandle.guiSelectionBox = this;
	}

	GuiSelectionBox& GuiSelectionBox::operator=(GuiSelectionBox&& other) noexcept
	{
		closeSelectionBox();
		GuiElement::operator=(std::move(other));
		options = std::move(other.options);
		selectedOption = std::move(other.selectedOption);
		arrowButton = std::move(other.arrowButton);
		selectedOptionIndex = std::move(other.selectedOptionIndex);
		optionListLayout = std::move(other.optionListLayout);
		optionButtonEventHandles = std::move(other.optionButtonEventHandles);
		arrowButtonEventHandle = std::move(other.arrowButtonEventHandle);
		minimumWidthOfLongestOption = std::move(other.minimumWidthOfLongestOption);
		optionButtonColor = std::move(other.optionButtonColor);
		optionButtonPressedColor = std::move(other.optionButtonPressedColor);
		optionButtonHoverColor = std::move(other.optionButtonHoverColor);
		optionButtonPressedHoverColor = std::move(other.optionButtonPressedHoverColor);
		selectedOptionButtonColor = std::move(other.selectedOptionButtonColor);
		selectedOptionButtonPressedColor = std::move(other.selectedOptionButtonPressedColor);
		selectedOptionButtonHoverColor = std::move(other.selectedOptionButtonHoverColor);
		selectedOptionButtonPressedHoverColor = std::move(other.selectedOptionButtonPressedHoverColor);
		arrowButtonMenuClosedColor = std::move(other.arrowButtonMenuClosedColor);
		arrowButtonMenuClosedPressedColor = std::move(other.arrowButtonMenuClosedPressedColor);
		arrowButtonMenuClosedHoverColor = std::move(other.arrowButtonMenuClosedHoverColor);
		arrowButtonMenuClosedPressedHoverColor = std::move(other.arrowButtonMenuClosedPressedHoverColor);
		arrowButtonMenuOpenColor = std::move(other.arrowButtonMenuOpenColor);
		arrowButtonMenuOpenPressedColor = std::move(other.arrowButtonMenuOpenPressedColor);
		arrowButtonMenuOpenHoverColor = std::move(other.arrowButtonMenuOpenHoverColor);
		arrowButtonMenuOpenPressedHoverColor = std::move(other.arrowButtonMenuOpenPressedHoverColor);
		optionButtonTextColor = std::move(other.optionButtonTextColor);
		selectedOptionButtonTextColor = std::move(other.selectedOptionButtonTextColor);
		arrowButtonMenuClosedTextColor = std::move(other.arrowButtonMenuClosedTextColor);
		arrowButtonMenuOpenTextColor = std::move(other.arrowButtonMenuOpenTextColor);
		border = std::move(other.border);
		fontSize = std::move(other.fontSize);
		font = std::move(other.font);
		arrowButtonEventHandle.guiSelectionBox = this;
		for (auto& optionHandle : optionButtonEventHandles) optionHandle.guiSelectionBox = this;
		return *this;
	}

	void GuiSelectionBox::addOption(const std::string& option)
	{
		options.push_back(option);
		// Update information on width
		selectedOption.setText(option);
		if (selectedOption.getMinWidth() > minimumWidthOfLongestOption) minimumWidthOfLongestOption = selectedOption.getMinWidth();
		selectedOption.setText(options[std::min(selectedOptionIndex.get(), options.size() - 1)]);
		closeSelectionBox();
		computeSizeHints();
	}

	void GuiSelectionBox::setOptions(const std::vector<std::string>& options)
	{
		this->options = options;
		this->selectedOptionIndex.set(0);
		// Obtain information on width
		for (const std::string& option : options) {
			// Compute the size of the text box
			selectedOption.setText(option);
			if (selectedOption.getMinWidth() > minimumWidthOfLongestOption) minimumWidthOfLongestOption = selectedOption.getMinWidth();
		}
		closeSelectionBox();
		computeSizeHints();
		updateSelection();
	}

	void GuiSelectionBox::setSelectedOptionIndex(std::size_t selectedOptionIndex)
	{
		std::size_t temp = std::min(selectedOptionIndex, options.size() - 1);
		if (temp != this->selectedOptionIndex.get()) {
			this->selectedOptionIndex.set(temp);
			updateSelection();
		}
	}

}