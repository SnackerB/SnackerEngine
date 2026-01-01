#include "Gui\GuiElements\GuiSelectionBox.h"
#include "Gui\GuiManager.h"
#include "Utility\Keys.h"
#include "Graphics\Renderer.h"
#include "Gui\Layouts\PositioningLayout.h"

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

	Color4f GuiSelectionBox::defaultDividerColor = Color4f(0.0f, 1.0f);

	Color4f GuiSelectionBox::defaultTextColor = Color4f(1.0f, 1.0f);

	Color4f GuiSelectionBox::defaultDropDownSymbolColorClosed = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	Color4f GuiSelectionBox::defaultDropDownSymbolColorOpen = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	Texture GuiSelectionBox::defaultDropdownSymbolTextureClosed{};
	Texture GuiSelectionBox::defaultDropdownSymbolTextureOpen{};
	Shader GuiSelectionBox::defaultDropdownSymbolShader{};

	void GuiSelectionBox::notifyEvent(int eventID)
	{
		if (optionListLayout && optionListLayout->isValid()) {
			// the selectionBox is currently open
			if (eventID < 0) {
				// currently selected option button was pressed.
				// We just need to close the selectionBox
				selectedOptionButtonEventHandle.reset();
			}
			else if (eventID < options.size()) {
				// An option button was pressed. Change the currently selected
				// option and close the selectionBox
				selectedOptionIndex.set(eventID);
				setText(options[eventID]);
			}
			closeSelectionBox();
		}
		else {
			// the selectionBox is currently closed
			if (eventID < 0) {
				// currently selected option button or arrow button was pressed.
				// We just need to open the selectionBox
				selectedOptionButtonEventHandle.reset();
				openSelectionBox();
			}
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
		optionListLayout->setSize(Vec2i(getWidth(), getHeight() * static_cast<int>(options.size()) + dividerThickness));
		optionListLayout->setPosition(getWorldPosition() + Vec2i(0, getHeight()));
		guiManager->registerElement(*optionListLayout);
		// Push back divider
		if (dividerThickness > 0) {
			Vec2i dividerBackgroundSize(getWidth(), dividerThickness);
			GuiPanel dividerBackground(Vec2i(), dividerBackgroundSize, ResizeMode::RESIZE_RANGE, optionButtonColor);
			dividerBackground.setMinSize(dividerBackgroundSize);
			dividerBackground.setPreferredSize(dividerBackgroundSize);
			optionListLayout->registerChild(dividerBackground);
			GuiPositioningLayout positioningLayout(GuiPositioningLayout::Mode::CENTER);
			dividerBackground.registerChild(positioningLayout);
			Vec2i dividerForegroundSize = Vec2i(static_cast<int>(static_cast<float>(getWidth()) * dividerWidthPercentage), dividerThickness);
			GuiPanel dividerForeground(Vec2i(), dividerForegroundSize, ResizeMode::RESIZE_RANGE, dividerColor, Color4f(), dividerRoundedCorners);
			dividerForeground.setMinSize(dividerForegroundSize);
			dividerForeground.setPreferredSize(dividerForegroundSize);
			positioningLayout.registerChild(dividerForeground);
			guiManager->moveElement(std::move(dividerForeground));
			guiManager->moveElement(std::move(positioningLayout));
			guiManager->moveElement(std::move(dividerBackground));
		}
		// Push back option buttons
		GuiButton templateButton(*this);
		templateButton.setSize(getSize());
		templateButton.setPreferredSize(getSize());
		templateButton.setSizeHintModePreferredSize(GuiTextBox::SizeHintMode::ARBITRARY);
		templateButton.setDefaultColor(optionButtonColor);
		templateButton.setPressedColor(optionButtonPressedColor);
		templateButton.setHoverColor(optionButtonHoverColor);
		templateButton.setPressedHoverColor(optionButtonPressedHoverColor);
		templateButton.setTextColor(optionButtonTextColor);
		templateButton.setRoundedCorners(0.0f);
		optionButtonEventHandles.clear(); // Clear handles
		for (std::size_t i = 0; i < options.size(); ++i) {
			GuiButton optionButton(templateButton);
			optionButton.setText(options[i]);
			optionButtonEventHandles.push_back(GuiSelectionBoxEventHandle(*this, static_cast<int>(i)));
			optionButton.subscribeToEventButtonPress(optionButtonEventHandles.back());
			optionListLayout->registerChild(optionButton);
			if (i == options.size() - 1) optionButton.setRoundedCorners(Vec4f(0.0f, 0.0f, roundedCorners, roundedCorners));
			guiManager->moveElement<GuiButton>(std::move(optionButton));
		}
		// Fix corners
		GuiScaleButton::setRoundedCorners(Vec4f(roundedCorners, roundedCorners, 0.0f, 0.0f));
		// Fix button scale
		GuiScaleButton::setHoverButtonScale(1.0f);
		GuiScaleButton::setPressedButtonScale(1.0f);
		GuiScaleButton::setPressedHoverButtonScale(1.0f);
		// bring to foreground
		bringToForeground();
		optionListLayout->bringToForeground();
	}

	void GuiSelectionBox::computeWidthHints() 
	{
		int textWidth = dropDownSymbolSize.x;
		if (textWidth < 0) textWidth = getMinHeight();
		textWidth += getLeftBorder() + getRightBorder() + minimumWidthOfLongestOption;
		switch (getSizeHintModes().sizeHintModeMinSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: setMinWidth(textWidth); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE:  setMinWidth(textWidth); break;
		default: break;
		}
		switch (getSizeHintModes().sizeHintModeMaxSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setMaxHeight(getMinHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: setMaxWidth(textWidth); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setMaxWidth(textWidth); break;
		default: break;
		}
		switch (getSizeHintModes().sizeHintModePreferredSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setPreferredHeight(getMinHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: setPreferredWidth(textWidth); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setPreferredWidth(textWidth); break;
		default:
			break;
		}
	}

	void GuiSelectionBox::computeHeightHints() 
	{
		switch (getSizeHintModes().sizeHintModeMinSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setMinHeight(getTextHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setMinHeight(getTextHeight()); break;
		default: break;
		}
		switch (getSizeHintModes().sizeHintModeMaxSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setMaxHeight(getTextHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setMaxHeight(getTextHeight()); break;
		default: break;
		}
		switch (getSizeHintModes().sizeHintModePreferredSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: setPreferredHeight(getTextHeight()); break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: break;
		case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: setPreferredHeight(getTextHeight()); break;
		default:
			break;
		}
	}

	void GuiSelectionBox::closeSelectionBox()
	{
		if (optionListLayout && optionListLayout->isValid()) optionListLayout->signOffAtNextUpdate();
		// Fix corners
		GuiScaleButton::setRoundedCorners(roundedCorners);
		// Fix button scale
		GuiScaleButton::setHoverButtonScale(selectedOptionHoverButtonScale);
		GuiScaleButton::setPressedButtonScale(selectedOptionPressedButtonScale);
		GuiScaleButton::setPressedHoverButtonScale(selectedOptionPressedHoverButtonScale);
	}

	void GuiSelectionBox::draw(const Vec2i& worldPosition)
	{
		GuiScaleButton::draw(worldPosition);
		// Draw drop down symbol
		if (drawDropDownSymbol && ((optionListLayout && optionListLayout->isValid() && dropDownSymbolColorOpen.alpha != 0.0f) || ((!optionListLayout || !optionListLayout->isValid()) && dropDownSymbolColorClosed.alpha != 0.0f))) {
			//pushClippingBox(worldPosition);
			dropDownSymbolShader.bind();
			getGuiManager()->setUniformViewAndProjectionMatrices(dropDownSymbolShader);
			Vec2i trueDropDownSymbolSize = dropDownSymbolSize;
			if (trueDropDownSymbolSize.x < 0) trueDropDownSymbolSize.x = getHeight();
			if (trueDropDownSymbolSize.y < 0) trueDropDownSymbolSize.y = getHeight();
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			Mat4f modelMatrix = Mat4f::TranslateAndScale(
				Vec3f(static_cast<float>(getWidth() - trueDropDownSymbolSize.x - getRightBorder()), static_cast<float>(-trueDropDownSymbolSize.y/2-getHeight()/2), 0.0f),
				Vec3f(static_cast<float>(trueDropDownSymbolSize.x), static_cast<float>(trueDropDownSymbolSize.y), 0.0f));
			dropDownSymbolShader.setUniform<Mat4f>("u_model", translationMatrix * getTransformMatrix() * modelMatrix);
			if (optionListLayout && optionListLayout->isValid()) {
				dropDownSymbolShader.setUniform<Color4f>("u_color", dropDownSymbolColorOpen);
				dropDownSymbolTextureOpen.bind();
			}
			else {
				dropDownSymbolShader.setUniform<Color4f>("u_color", dropDownSymbolColorClosed);
				dropDownSymbolTextureClosed.bind();
			}
			dropDownSymbolShader.setUniform<float>("u_pxRange", 4.0);
			dropDownSymbolShader.setUniform<int>("u_msdf", 0);
			Renderer::draw(getGuiManager()->getModelSquare());
			//popClippingBox();
		}
	}

	void GuiSelectionBox::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (action == ACTION_PRESS) {
			GuiManager* guiManager = getGuiManager();
			if (guiManager && (guiManager->isMouseHoveringOver(getGuiID()) || guiManager->isMouseHoveringOver(optionListLayout->getGuiID()))) return;
			closeSelectionBox();
		}
		GuiScaleButton::callbackMouseButton(button, action, mods);
	}

	void GuiSelectionBox::onRegister()
	{
		GuiScaleButton::onRegister();
		// Set attributes of selectedOption button
		setRoundedCorners(roundedCorners);
		setDefaultColor(selectedOptionButtonColor);
		setHoverColor(selectedOptionButtonHoverColor);
		setPressedColor(selectedOptionButtonPressedColor);
		setPressedHoverColor(selectedOptionButtonPressedHoverColor);
		setTextColor(selectedOptionButtonTextColor);
		setAlignmentHorizontal(AlignmentHorizontal::LEFT);
		setAlignmentVertical(AlignmentVertical::CENTER);
		GuiScaleButton::setHoverButtonScale(selectedOptionHoverButtonScale);
		GuiScaleButton::setPressedButtonScale(selectedOptionPressedButtonScale);
		GuiScaleButton::setPressedHoverButtonScale(selectedOptionPressedHoverButtonScale);
		// Set event handles
		subscribeToEventButtonPress(selectedOptionButtonEventHandle);
		// Obtain information on width
		for (const std::string& option : options) {
			// Compute the size of the text box
			setText(option);
			if (getMinWidth() > minimumWidthOfLongestOption) minimumWidthOfLongestOption = getMinWidth();
		}
		if (!options.empty()) setText(options[selectedOptionIndex]);
		// Finalize
		onSizeChange();
	}

	void GuiSelectionBox::onSizeChange()
	{
		GuiScaleButton::onSizeChange();
		computeSizeHints();
	}

	GuiID GuiSelectionBox::getCollidingChild(const Vec2i& offset) const
	{
		return GuiScaleButton::getCollidingChild(offset);
		// TODO: Remove?
	}

	void GuiSelectionBox::updateSelection()
	{
		if (!options.empty()) setText(options[std::min(selectedOptionIndex.get(), options.size() - 1)]);
		closeSelectionBox();
	}

	GuiSelectionBox::GuiSelectionBox(const std::vector<std::string>& options)
		: GuiScaleButton(), options(options)
	{
		setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_SIZE);
		setSizeHintModePreferredSize(SizeHintMode::ARBITRARY);
		setSizeHintModeMaxSize(SizeHintMode::ARBITRARY);
	}

	GuiSelectionBox::GuiSelectionBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiScaleButton(json, data, parameterNames)
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
		parseJsonOrReadFromData(optionButtonTextColor, "optionButtonTextColor", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionButtonTextColor, "selectedOptionButtonTextColor", json, data, parameterNames);
		// TODO: Sort in the following parseJSONS
		parseJsonOrReadFromData(roundedCorners, "roundedCorners", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionHoverButtonScale, "hoverButtonScale", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionPressedHoverButtonScale, "pressedHoverButtonScale", json, data, parameterNames);
		parseJsonOrReadFromData(selectedOptionPressedButtonScale, "pressedButtonScale", json, data, parameterNames);
		parseJsonOrReadFromData(dropDownSymbolColorOpen, "dropDownSymbolColorOpen", json, data, parameterNames);
		parseJsonOrReadFromData(dropDownSymbolColorClosed, "dropDownSymbolColorClosed", json, data, parameterNames);
		parseJsonOrReadFromData(dropDownSymbolTextureOpen, "dropDownSymbolTextureOpen", json, data, parameterNames);
		parseJsonOrReadFromData(dropDownSymbolTextureClosed, "dropDownSymbolTextureClosed", json, data, parameterNames);
		parseJsonOrReadFromData(dropDownSymbolShader, "dropDownSymbolShader", json, data, parameterNames);
		parseJsonOrReadFromData(drawDropDownSymbol, "drawDropDownSymbol", json, data, parameterNames);
		parseJsonOrReadFromData(dropDownSymbolSize, "dropDownSymbolSize", json, data, parameterNames);
		if (!json.contains("sizeHintModeMinSize")) setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_SIZE);
		if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(SizeHintMode::ARBITRARY);
		if (!json.contains("sizeHintModeMaxSize")) setSizeHintModeMaxSize(SizeHintMode::ARBITRARY);
		parseJsonOrReadFromData(dividerThickness, "dividerThickness", json, data, parameterNames);
		parseJsonOrReadFromData(dividerRoundedCorners, "dividerRoundedCorners", json, data, parameterNames);
		parseJsonOrReadFromData(dividerWidthPercentage, "dividerWidthPercentage", json, data, parameterNames);
		parseJsonOrReadFromData(dividerColor, "dividerColor", json, data, parameterNames);
	}

	GuiSelectionBox::~GuiSelectionBox()
	{
		closeSelectionBox();
	}

	GuiSelectionBox::GuiSelectionBox(const GuiSelectionBox& other) noexcept
		: GuiScaleButton(other), options(other.options), selectedOptionIndex(other.selectedOptionIndex), 
		optionListLayout(nullptr), optionButtonEventHandles(), selectedOptionButtonEventHandle(*this, -1), 
		minimumWidthOfLongestOption(other.minimumWidthOfLongestOption), roundedCorners(other.roundedCorners),
		selectedOptionHoverButtonScale(other.selectedOptionHoverButtonScale), 
		selectedOptionPressedButtonScale(other.selectedOptionPressedButtonScale),
		selectedOptionPressedHoverButtonScale(other.selectedOptionPressedHoverButtonScale),
		optionButtonColor(other.optionButtonColor), optionButtonPressedColor(other.optionButtonPressedColor),
		optionButtonHoverColor(other.optionButtonHoverColor), optionButtonPressedHoverColor(other.optionButtonPressedHoverColor),
		selectedOptionButtonColor(other.selectedOptionButtonColor),
		selectedOptionButtonPressedColor(other.selectedOptionButtonPressedColor),
		selectedOptionButtonHoverColor(other.selectedOptionButtonHoverColor),
		selectedOptionButtonPressedHoverColor(other.selectedOptionButtonPressedHoverColor),
		optionButtonTextColor(other.optionButtonTextColor), selectedOptionButtonTextColor(other.selectedOptionButtonTextColor),
		dividerThickness(other.dividerThickness), dividerRoundedCorners(other.dividerRoundedCorners),
		dividerWidthPercentage(other.dividerWidthPercentage), dividerColor(other.dividerColor),
		dropDownSymbolColorOpen(other.dropDownSymbolColorOpen), dropDownSymbolColorClosed(other.dropDownSymbolColorClosed),
		dropDownSymbolTextureOpen(other.dropDownSymbolTextureOpen), dropDownSymbolTextureClosed(other.dropDownSymbolTextureClosed),
		dropDownSymbolShader(other.dropDownSymbolShader), drawDropDownSymbol(other.drawDropDownSymbol), dropDownSymbolSize(other.dropDownSymbolSize)
	{
	}

	GuiSelectionBox& GuiSelectionBox::operator=(const GuiSelectionBox& other) noexcept
	{
		closeSelectionBox();
		GuiScaleButton::operator=(other);
		options = other.options;
		selectedOptionIndex = other.selectedOptionIndex;
		optionListLayout = nullptr;
		optionButtonEventHandles.clear();
		selectedOptionButtonEventHandle = GuiSelectionBoxEventHandle(*this, -1);
		minimumWidthOfLongestOption = other.minimumWidthOfLongestOption;
		roundedCorners = other.roundedCorners;
		selectedOptionHoverButtonScale = other.selectedOptionHoverButtonScale;
		selectedOptionPressedButtonScale = other.selectedOptionPressedButtonScale;
		selectedOptionPressedHoverButtonScale = other.selectedOptionPressedHoverButtonScale;
		optionButtonColor = other.optionButtonColor;
		optionButtonPressedColor = other.optionButtonPressedColor;
		optionButtonHoverColor = other.optionButtonHoverColor;
		optionButtonPressedHoverColor = other.optionButtonPressedHoverColor;
		selectedOptionButtonColor = other.selectedOptionButtonColor;
		selectedOptionButtonPressedColor = other.selectedOptionButtonPressedColor;
		selectedOptionButtonHoverColor = other.selectedOptionButtonHoverColor;
		selectedOptionButtonPressedHoverColor = other.selectedOptionButtonPressedHoverColor;
		optionButtonTextColor = other.optionButtonTextColor;
		selectedOptionButtonTextColor = other.selectedOptionButtonTextColor;
		dividerThickness = other.dividerThickness; 
		dividerRoundedCorners = other.dividerRoundedCorners;
		dividerWidthPercentage = other.dividerWidthPercentage; 
		dividerColor = other.dividerColor;
		dropDownSymbolColorOpen = other.dropDownSymbolColorOpen;
		dropDownSymbolColorClosed = other.dropDownSymbolColorClosed;
		dropDownSymbolTextureOpen = other.dropDownSymbolTextureOpen; 
		dropDownSymbolTextureClosed = other.dropDownSymbolTextureClosed;
		dropDownSymbolShader = other.dropDownSymbolShader; 
		drawDropDownSymbol = other.drawDropDownSymbol; 
		dropDownSymbolSize = other.dropDownSymbolSize;
		return *this;
	}

	GuiSelectionBox::GuiSelectionBox(GuiSelectionBox&& other) noexcept
		: GuiScaleButton(std::move(other)), options(std::move(other.options)), selectedOptionIndex(std::move(other.selectedOptionIndex)),
		optionListLayout(std::move(other.optionListLayout)), optionButtonEventHandles(std::move(other.optionButtonEventHandles)), 
		selectedOptionButtonEventHandle(std::move(selectedOptionButtonEventHandle)),
		minimumWidthOfLongestOption(std::move(other.minimumWidthOfLongestOption)), roundedCorners(std::move(other.roundedCorners)),
		selectedOptionHoverButtonScale(std::move(other.selectedOptionHoverButtonScale)),
		selectedOptionPressedButtonScale(std::move(other.selectedOptionPressedButtonScale)),
		selectedOptionPressedHoverButtonScale(std::move(other.selectedOptionPressedHoverButtonScale)),
		optionButtonColor(std::move(other.optionButtonColor)), optionButtonPressedColor(std::move(other.optionButtonPressedColor)),
		optionButtonHoverColor(std::move(other.optionButtonHoverColor)), optionButtonPressedHoverColor(std::move(other.optionButtonPressedHoverColor)),
		selectedOptionButtonColor(std::move(other.selectedOptionButtonColor)),
		selectedOptionButtonPressedColor(std::move(other.selectedOptionButtonPressedColor)),
		selectedOptionButtonHoverColor(std::move(other.selectedOptionButtonHoverColor)),
		selectedOptionButtonPressedHoverColor(std::move(other.selectedOptionButtonPressedHoverColor)),
		optionButtonTextColor(std::move(other.optionButtonTextColor)), selectedOptionButtonTextColor(std::move(other.selectedOptionButtonTextColor)),
		dividerThickness(std::move(other.dividerThickness)), dividerRoundedCorners(std::move(other.dividerRoundedCorners)),
		dividerWidthPercentage(std::move(other.dividerWidthPercentage)), dividerColor(std::move(other.dividerColor)),
		dropDownSymbolColorOpen(std::move(other.dropDownSymbolColorOpen)), dropDownSymbolColorClosed(std::move(other.dropDownSymbolColorClosed)),
		dropDownSymbolTextureOpen(std::move(other.dropDownSymbolTextureOpen)), dropDownSymbolTextureClosed(std::move(other.dropDownSymbolTextureClosed)),
		dropDownSymbolShader(std::move(other.dropDownSymbolShader)), drawDropDownSymbol(std::move(other.drawDropDownSymbol)), 
		dropDownSymbolSize(std::move(other.dropDownSymbolSize))
	{
		selectedOptionButtonEventHandle.guiSelectionBox = this;
		for (auto& optionHandle : optionButtonEventHandles) optionHandle.guiSelectionBox = this;
	}

	GuiSelectionBox& GuiSelectionBox::operator=(GuiSelectionBox&& other) noexcept
	{
		closeSelectionBox();
		GuiScaleButton::operator=(std::move(other));
		options = std::move(other.options); 
		selectedOptionIndex = std::move(other.selectedOptionIndex);
		optionListLayout = std::move(other.optionListLayout);
		optionButtonEventHandles = std::move(other.optionButtonEventHandles);
		selectedOptionButtonEventHandle = std::move(selectedOptionButtonEventHandle);
		minimumWidthOfLongestOption = std::move(other.minimumWidthOfLongestOption);
		roundedCorners = std::move(other.roundedCorners);
		selectedOptionHoverButtonScale = std::move(other.selectedOptionHoverButtonScale);
		selectedOptionPressedButtonScale = std::move(other.selectedOptionPressedButtonScale);
		selectedOptionPressedHoverButtonScale = std::move(other.selectedOptionPressedHoverButtonScale);
		optionButtonColor = std::move(other.optionButtonColor);
		optionButtonPressedColor = std::move(other.optionButtonPressedColor);
		optionButtonHoverColor = std::move(other.optionButtonHoverColor);
		optionButtonPressedHoverColor = std::move(other.optionButtonPressedHoverColor);
		selectedOptionButtonColor = std::move(other.selectedOptionButtonColor);
		selectedOptionButtonPressedColor = std::move(other.selectedOptionButtonPressedColor);
		selectedOptionButtonHoverColor = std::move(other.selectedOptionButtonHoverColor);
		selectedOptionButtonPressedHoverColor = std::move(other.selectedOptionButtonPressedHoverColor);
		optionButtonTextColor = std::move(other.optionButtonTextColor);
		selectedOptionButtonTextColor = std::move(other.selectedOptionButtonTextColor);
		dividerThickness = std::move(other.dividerThickness);
		dividerRoundedCorners = std::move(other.dividerRoundedCorners);
		dividerWidthPercentage = std::move(other.dividerWidthPercentage);
		dividerColor = std::move(other.dividerColor);
		dropDownSymbolColorOpen = std::move(other.dropDownSymbolColorOpen);
		dropDownSymbolColorClosed = std::move(other.dropDownSymbolColorClosed);
		dropDownSymbolTextureOpen = std::move(other.dropDownSymbolTextureOpen);
		dropDownSymbolTextureClosed = std::move(other.dropDownSymbolTextureClosed);
		dropDownSymbolShader = std::move(other.dropDownSymbolShader);
		drawDropDownSymbol = std::move(other.drawDropDownSymbol);
		dropDownSymbolSize = std::move(other.dropDownSymbolSize);
		selectedOptionButtonEventHandle.guiSelectionBox = this;
		for (auto& optionHandle : optionButtonEventHandles) optionHandle.guiSelectionBox = this;
		return *this;
	}

	void GuiSelectionBox::addOption(const std::string& option)
	{
		options.push_back(option);
		// Update information on width
		setText(option);
		if (getMinWidth() > minimumWidthOfLongestOption) minimumWidthOfLongestOption = getMinWidth();
		setText(options[std::min(selectedOptionIndex.get(), options.size() - 1)]);
		closeSelectionBox();
		computeSizeHints();
		computeTransformMatrixAndClippingBox();
	}

	void GuiSelectionBox::setOptions(const std::vector<std::string>& options)
	{
		closeSelectionBox();
		this->options = options;
		this->selectedOptionIndex.set(0);
		// Obtain information on width
		for (const std::string& option : options) {
			// Compute the size of the text box
			setText(option);
			if (getMinWidth() > minimumWidthOfLongestOption) minimumWidthOfLongestOption = getMinWidth();
		}
		if (!options.empty()) setText(options[selectedOptionIndex.get()]);
		// Finalize
		onSizeChange();
		closeSelectionBox();
		computeSizeHints();
		computeTransformMatrixAndClippingBox();
	}

	void GuiSelectionBox::setRoundedCorners(float roundedCorners)
	{
		if (this->roundedCorners != roundedCorners) {
			this->roundedCorners = roundedCorners;
			if (optionListLayout && optionListLayout->isValid()) {
				GuiScaleButton::setRoundedCorners(Vec4f(roundedCorners, roundedCorners, 0.0f, 0.0f));
			}
			else {
				setRoundedCorners(roundedCorners);
			}
		}
	}

	void GuiSelectionBox::setSelectedOptionIndex(std::size_t selectedOptionIndex)
	{
		std::size_t temp = std::min(selectedOptionIndex, options.size() - 1);
		if (temp != this->selectedOptionIndex.get()) {
			this->selectedOptionIndex.set(temp);
			updateSelection();
		}
	}

	void GuiSelectionBox::setHoverButtonScale(float hoverButtonScale)
	{
		this->selectedOptionHoverButtonScale = hoverButtonScale;
		if (!optionListLayout || !optionListLayout->isValid()) GuiScaleButton::setHoverButtonScale(hoverButtonScale);
	}

	void GuiSelectionBox::setPressedButtonScale(float pressedButtonScale)
	{
		this->selectedOptionPressedButtonScale = pressedButtonScale;
		if (!optionListLayout || !optionListLayout->isValid()) GuiScaleButton::setPressedButtonScale(pressedButtonScale);
	}

	void GuiSelectionBox::setPressedHoverButtonScale(float pressedHoverButtonScale)
	{
		this->selectedOptionPressedHoverButtonScale = pressedHoverButtonScale;
		if (!optionListLayout || !optionListLayout->isValid()) GuiScaleButton::setPressedHoverButtonScale(pressedHoverButtonScale);
	}

	void GuiSelectionBox::setDrawDropDownSymbol(bool drawDropDownSymbol)
	{
		if (this->drawDropDownSymbol != drawDropDownSymbol) {
			this->drawDropDownSymbol = drawDropDownSymbol;
			onSizeChange();
		}
	}

	void GuiSelectionBox::getDropDownSymbolSize(const Vec2i& dropDownSymbolSize)
	{
		if (this->dropDownSymbolSize != dropDownSymbolSize) {
			this->dropDownSymbolSize = dropDownSymbolSize;
			onSizeChange();
		}
	}

}