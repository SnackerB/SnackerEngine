#include "Gui\GuiElements\GuiEditBox.h"
#include "Core\Engine.h"
#include "Math\Utility.h"
#include "Gui\GuiManager.h"
#include "Graphics\Renderer.h"
#include "Utility\Keys.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	Color4f GuiEditBox::defaultSelectionBoxColor = Color4f(0.8f, 0.8f, 1.0f, 0.5f);
	Color4f GuiEditBox::defaultBackgroundColor = Color4f(0.0f, 1.0f);
	float GuiEditBox::defaultCursorWidth{};
	double GuiEditBox::defaultCursorBlinkingTime = 0.5;
	GuiTextBox::SizeHintModes GuiEditBox::defaultSizeHintModes = { GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE, GuiTextBox::SizeHintMode::ARBITRARY, GuiTextBox::SizeHintMode::ARBITRARY };
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::computeTextOffsetAndCursorOffset()
	{
		if (!dynamicText) return;
		// Compute "normal" text position (without cursor)
		textPosition = computeTextPosition();
		// Compute cursorOffset relative to text
		cursorOffsetFromText = static_cast<EditableText&>(*dynamicText).getCursorPos() * static_cast<float>(scaleFactor);
		cursorOffsetFromText.x = pointsToPixels(cursorOffsetFromText.x);
		cursorOffsetFromText.y = pointsToPixels(cursorOffsetFromText.y);
		// Reposition text to fit cursor in bounding box
		textOffset = Vec2f(0.0f, 0.0f);
		if (repositionTextToFitCursor && active) {
			cursorPosition = textPosition + textOffset + cursorOffsetFromText;
			if (cursorPosition.x > getWidth() - getLeftBorder()) textOffset.x = getWidth() - getLeftBorder() - cursorOffsetFromText.x - textPosition.x;
			else {
				float cursorWidth = pointsToPixels(static_cast<EditableText&>(*dynamicText).getCursorSize().x);
				if (cursorPosition.x < getLeftBorder() - cursorWidth) textOffset.x = getLeftBorder() - cursorWidth - cursorOffsetFromText.x - textPosition.x;

			}
		}
		textPosition += textOffset;
		cursorPosition = textPosition + cursorOffsetFromText;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::computeTextOffsetAndCursorOffsetKeepTextOffset()
	{
		if (!dynamicText) return;
		// If the text width is small enough that the text can fit, or if we anyways scale the text to fit, call default procedure
		if (!repositionTextToFitCursor || !active || getTextSize().x < getWidth() - getLeftBorder() - getRightBorder()) {
			computeTextOffsetAndCursorOffset();
			return;
		}
		// Compute cursorOffset relative to text
		cursorOffsetFromText = static_cast<EditableText&>(*dynamicText).getCursorPos() * static_cast<float>(scaleFactor);
		cursorOffsetFromText.x = pointsToPixels(cursorOffsetFromText.x);
		cursorOffsetFromText.y = pointsToPixels(cursorOffsetFromText.y);
		// Reposition text to fit cursor in bounding box
		if (repositionTextToFitCursor) {
			cursorPosition = textPosition + cursorOffsetFromText;
			if (cursorPosition.x > getWidth() - getLeftBorder()) {
				textPosition = computeTextPosition();
				textOffset.x = getWidth() - getLeftBorder() - cursorOffsetFromText.x - textPosition.x;
				textPosition += textOffset;
			}
			else {
				float cursorWidth = pointsToPixels(static_cast<EditableText&>(*dynamicText).getCursorSize().x);
				if (cursorPosition.x < getLeftBorder() - cursorWidth) {
					textPosition = computeTextPosition();
					textOffset.x = getLeftBorder() - cursorWidth - cursorOffsetFromText.x - textPosition.x;
					textPosition += textOffset;
				}
			}
		}
		cursorPosition = textPosition + cursorOffsetFromText;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::computeTextOffsetAndCursorOffsetKeepCursorOffset()
	{
		if (!dynamicText) return;
		// If the text width is small enough that the text can fit, call default procedure
		if (!repositionTextToFitCursor || !active || getTextSize().x < getWidth() - getLeftBorder() - getRightBorder()) {
			computeTextOffsetAndCursorOffset();
			return;
		}
		// Compute cursorOffset relative to text
		cursorOffsetFromText = static_cast<EditableText&>(*dynamicText).getCursorPos() * static_cast<float>(scaleFactor);
		cursorOffsetFromText.x = pointsToPixels(cursorOffsetFromText.x);
		cursorOffsetFromText.y = pointsToPixels(cursorOffsetFromText.y);
		// Reposition text to fit cursor in bounding box
		if (repositionTextToFitCursor) {
			textPosition = cursorPosition - cursorOffsetFromText;
			Vec2i defaultTextPosition = computeTextPosition();
			if (textPosition.x > defaultTextPosition.x) {
				computeTextOffsetAndCursorOffset();
				return;
			}
			if (textPosition.x + getTextSize().x < getWidth() - getLeftBorder()) {
				textPosition.x = getWidth() - getLeftBorder() - getTextSize().x;
				textOffset = textPosition - defaultTextPosition;
				computeTextOffsetAndCursorOffsetKeepTextOffset();
				return;
			}
			textOffset = textPosition - defaultTextPosition;
		}
		cursorPosition = textPosition + cursorOffsetFromText;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::computeTextOffsetAndCursorOffsetAdvanceCursor()
	{
		if (!dynamicText) return;
		// If the text width is small enough that the text can fit, call default procedure
		if (!repositionTextToFitCursor || !active || getTextSize().x < getWidth() - getLeftBorder() - getRightBorder()) {
			computeTextOffsetAndCursorOffset();
			return;
		}
		// Compute the updated cursorOffset relative to text
		Vec2f newCursorOffsetFromText = static_cast<EditableText&>(*dynamicText).getCursorPos() * static_cast<float>(scaleFactor);
		newCursorOffsetFromText.x = pointsToPixels(newCursorOffsetFromText.x);
		newCursorOffsetFromText.y = pointsToPixels(newCursorOffsetFromText.y);
		// We want to advance the cursor position by the difference!
		if (repositionTextToFitCursor) {
			textPosition = cursorPosition + newCursorOffsetFromText - cursorOffsetFromText*2;
			cursorOffsetFromText = newCursorOffsetFromText;
			cursorPosition = textPosition + cursorOffsetFromText;
			Vec2i defaultTextPosition = computeTextPosition();
			if (textPosition.x > defaultTextPosition.x) {
				computeTextOffsetAndCursorOffset();
				return;
			}
			if (cursorPosition.x > getWidth() - getLeftBorder()) {
				textPosition = computeTextPosition();
				textOffset.x = getWidth() - getLeftBorder() - cursorOffsetFromText.x - textPosition.x;
				textPosition += textOffset;
			}
			else {
				textOffset = textPosition - defaultTextPosition;
			}
		}
		cursorPosition = textPosition + cursorOffsetFromText;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::computeModelMatrixCursor()
	{
		Vec2f cursorSize = static_cast<EditableText&>(*dynamicText).getCursorSize();
		cursorSize.x = pointsToPixels(cursorSize.x);
		cursorSize.y = pointsToPixels(cursorSize.y) * scaleFactor;
		modelMatrixCursor = Mat4f::TranslateAndScale(Vec3f(cursorPosition.x, cursorPosition.y * scaleFactor, 0.0f), cursorSize);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::computeModelMatricesSelectionBoxes()
	{
		if (!dynamicText) return;
		modelMatricesSelectionBoxes.clear();
		auto result = static_cast<EditableText&>(*dynamicText).getSelectionBoxes();
		if (result.empty()) return;
		for (const auto& selectionBox : result) {
			modelMatricesSelectionBoxes.push_back(Mat4f::TranslateAndScale(textPosition + Vec2f(pointsToPixels(selectionBox.position.x), pointsToPixels(selectionBox.position.y)) * static_cast<float>(scaleFactor), Vec2f(pointsToPixels(selectionBox.size.x), pointsToPixels(selectionBox.size.y)) * static_cast<float>(scaleFactor)));
			modelMatricesSelectionBoxes.push_back(Mat4f::TranslateAndScale(textPosition + Vec2f(pointsToPixels(selectionBox.position.x), pointsToPixels(selectionBox.position.y)) * static_cast<float>(scaleFactor), Vec2f(pointsToPixels(selectionBox.size.x), pointsToPixels(selectionBox.size.y)) * static_cast<float>(scaleFactor)));
		}
	}
	//--------------------------------------------------------------------------------------------------
	Vec2d GuiEditBox::getMouseOffsetToText()
	{
		// Get mouse offset to upper left corner of element
		Vec2d mousePos = getMouseOffset();
		mousePos = Vec2i(-static_cast<int>(textPosition.x), -static_cast<int>(textPosition.y)) + Vec2i(static_cast<int>(mousePos.x), -static_cast<int>(mousePos.y));
		mousePos /= pointsToPixels(1.0);
		return mousePos;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::OnTextEdit()
	{
		eventTextWasEdited.trigger();
		if (dynamicText) setText(dynamicText->getText());
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox::GuiEditBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font, const double& fontSize, double lineHeightMultiplier, const Color4f& backgroundColor)
		: GuiTextBox(position, size, text, font, fontSize, lineHeightMultiplier, backgroundColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox::GuiEditBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		parseJsonOrReadFromData(selectionBoxColor, "selectionBoxColor", json, data, parameterNames);
		parseJsonOrReadFromData(cursorWidth, "cursorWidth", json, data, parameterNames);
		parseJsonOrReadFromData(repositionTextToFitCursor, "repositionTextToFitCursor", json, data, parameterNames);
		std::optional<double> cursorBlinkTime = parseJsonOrReadFromData<double>("cursorBlinkTime", json, data, parameterNames);
		if (cursorBlinkTime.has_value()) cursorBlinkingTimer.setTimeStep(cursorBlinkTime.value());
		if (!json.contains("backgroundColor")) setBackgroundColor(defaultBackgroundColor);
		//if (!json.contains("sizeHintModeMinSize") && getTextScaleMode() != TextScaleMode::SCALE_DOWN && getTextScaleMode() != TextScaleMode::SCALE_UP_DOWN &&
		//	getTextScaleMode() != TextScaleMode::RECOMPUTE_DOWN && getTextScaleMode() != TextScaleMode::RECOMPUTE_UP_DOWN) setSizeHintModeMinSize(defaultSizeHintModes.sizeHintModeMinSize);
		//if (!json.contains("sizeHintModePreferredSize") && getTextScaleMode() == TextScaleMode::DONT_SCALE) setSizeHintModePreferredSize(defaultSizeHintModes.sizeHintModePreferredSize);
		//if (!json.contains("sizeHintModeMaxSize") && getTextScaleMode() != TextScaleMode::SCALE_UP && getTextScaleMode() != TextScaleMode::SCALE_UP_DOWN
		//	&& getTextScaleMode() != TextScaleMode::RECOMPUTE_UP_DOWN) setSizeHintModeMaxSize(defaultSizeHintModes.sizeHintModeMaxSize);
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox::GuiEditBox(const GuiEditBox& other) noexcept
		: GuiTextBox(other), selectionBoxColor(other.selectionBoxColor), cursorWidth(other.cursorWidth), 
		active(false), cursorIsVisible(false),
		cursorBlinkingTimer(other.cursorBlinkingTimer), modelMatrixCursor{}, modelMatricesSelectionBoxes{},
		eventTextWasEdited{}, eventEnterWasPressed{} {}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox& GuiEditBox::operator=(const GuiEditBox& other) noexcept
	{
		// TODO: Change all constructors and move/copy operators
		GuiTextBox::operator=(other);
		selectionBoxColor = other.selectionBoxColor;
		cursorWidth = other.cursorWidth;
		active = false;
		cursorIsVisible = false;
		cursorBlinkingTimer = other.cursorBlinkingTimer;
		modelMatrixCursor = Mat4f();
		modelMatricesSelectionBoxes.clear();
		eventTextWasEdited = EventHandle::Observable();
		eventEnterWasPressed = EventHandle::Observable();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox::GuiEditBox(GuiEditBox&& other) noexcept
		: GuiTextBox(std::move(other)), selectionBoxColor(std::move(other.selectionBoxColor)), 
		cursorWidth(std::move(cursorWidth)), active(std::move(other.active)), 
		cursorIsVisible(std::move(other.cursorIsVisible)),
		cursorBlinkingTimer(std::move(other.cursorBlinkingTimer)), modelMatrixCursor(std::move(other.modelMatrixCursor)),
		modelMatricesSelectionBoxes(std::move(other.modelMatricesSelectionBoxes)),
		eventTextWasEdited(std::move(other.eventTextWasEdited)),
		eventEnterWasPressed(std::move(other.eventEnterWasPressed))
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox& GuiEditBox::operator=(GuiEditBox&& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		selectionBoxColor = std::move(other.selectionBoxColor);
		cursorWidth = std::move(other.cursorWidth);
		active = std::move(other.active);
		cursorIsVisible = std::move(other.cursorIsVisible);
		cursorBlinkingTimer = std::move(other.cursorBlinkingTimer);
		modelMatrixCursor = std::move(other.modelMatrixCursor);
		modelMatricesSelectionBoxes = std::move(other.modelMatricesSelectionBoxes);
		eventTextWasEdited = std::move(other.eventTextWasEdited);
		eventEnterWasPressed = std::move(other.eventEnterWasPressed);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::setCursorWidth(float cursorWidth)
	{
		this->cursorWidth = cursorWidth;
		if (dynamicText) static_cast<EditableText&>(*dynamicText).setCursorWidth(cursorWidth);
		computeModelMatrixCursor();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::subscribeEventTextWasEdited(EventHandle& eventHandle)
	{
		eventTextWasEdited.subscribe(eventHandle);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::setEventHandleEnterWasPressed(EventHandle& eventHandle)
	{
		eventEnterWasPressed.subscribe(eventHandle);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		// Draw background
		GuiPanel::draw(worldPosition);
		// Draw selection boxes
		pushClippingBox(worldPosition);
		for (const auto& modelMatrixSelectionBox : modelMatricesSelectionBoxes) {
			Shader tempShader = guiManager->getAlphaColorShader();
			tempShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(tempShader);
			tempShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixSelectionBox);
			tempShader.setUniform<Color4f>("u_color", selectionBoxColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		// Draw text
		drawText(worldPosition + textOffset);
		// Draw cursor
		if (cursorIsVisible) {
			Shader tempShader = guiManager->getAlphaColorShader();
			tempShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(tempShader);
			tempShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixCursor);
			tempShader.setUniform<Color4f>("u_color", getTextColor());
			Renderer::draw(guiManager->getModelSquare());
		}
		popClippingBox();
		// Draw children
		GuiElement::draw(worldPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::onRegister()
	{
		GuiTextBox::onRegister(std::move(std::make_unique<EditableText>(getText(), getFont(), getFontSize(), pixelsToPoints(static_cast<double>(getWidth() - getLeftBorder() - getRightBorder())), cursorWidth, getLineHeightMultiplier(), getParseMode(), getAlignmentHorizontal())));
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(CallbackType::MOUSE_ENTER);
		signUpEvent(CallbackType::MOUSE_LEAVE);
		computeTextOffsetAndCursorOffset();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::onSizeChange()
	{
		GuiTextBox::onSizeChange();
		computeTextOffsetAndCursorOffset();
		computeModelMatrixCursor();
		computeModelMatricesSelectionBoxes();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiEditBox::isColliding(const Vec2i& offset) const
	{
		return isCollidingBoundingBox(offset) ? IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiEditBox::animateSelectionBoxColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiEditBoxSelectionBoxColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiEditBox*>(element)->setSelectionBoxColor(currentVal); }
		public:
			GuiEditBoxSelectionBoxColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiEditBoxSelectionBoxColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiEditBox::animateCursorWidth(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiEditBoxCursorWidthAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiEditBox*>(element)->setCursorWidth(currentVal); }
		public:
			GuiEditBoxCursorWidthAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiEditBoxCursorWidthAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiEditBox::animateCursorBlinkTime(const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiEditBoxCursorBlinkTimeAnimatable : public GuiElementValueAnimatable<double>
		{
			virtual void onAnimate(const double& currentVal) override { if (element) static_cast<GuiEditBox*>(element)->setCursorBlinkTime(currentVal); }
		public:
			GuiEditBoxCursorBlinkTimeAnimatable(GuiElement& element, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<double>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiEditBoxCursorBlinkTimeAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (active && button == MOUSE_BUTTON_LEFT) {
			if (action == ACTION_PRESS) {
				if (isColliding(getMouseOffset()) == IsCollidingResult::NOT_COLLIDING) {
					active = false;
					cursorIsVisible = false;
					signOffEvent(CallbackType::CHARACTER_INPUT);
					signOffEvent(CallbackType::KEYBOARD);
					signOffEvent(CallbackType::UPDATE);
					signOffEvent(CallbackType::MOUSE_BUTTON);
					signOffEvent(CallbackType::MOUSE_MOTION);
					static_cast<EditableText&>(*dynamicText).setSelectionIndexToCursor();
					computeTextOffsetAndCursorOffset();
					computeModelMatricesSelectionBoxes();
					OnTextEdit();
				}
			}
			else if (action == ACTION_RELEASE) {
				signOffEvent(CallbackType::MOUSE_MOTION);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::callbackMouseMotion(const Vec2d& position)
	{
		Vec2d mousePos = getMouseOffsetToText();
		static_cast<EditableText&>(*dynamicText).computeCursorPosFromMousePos(mousePos / scaleFactor, false);
		computeTextOffsetAndCursorOffsetKeepTextOffset();
		computeModelMatrixCursor();
		computeModelMatricesSelectionBoxes();
		cursorBlinkingTimer.reset();
		cursorIsVisible = true;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		if (active && (action == ACTION_PRESS || action == ACTION_REPEAT))
		{
			if (key == KEY_LEFT) {
				if (mods & KEY_MOD_CONTROL) {
					static_cast<EditableText&>(*dynamicText).moveCursorToLeftWordBeginning(!(mods & KEY_MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeTextOffsetAndCursorOffsetKeepTextOffset();
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*dynamicText).moveCursorToLeft(!(mods & KEY_MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeTextOffsetAndCursorOffsetKeepTextOffset();
					computeModelMatrixCursor();
				}
			}
			else if (key == KEY_RIGHT) {
				if (mods & KEY_MOD_CONTROL) {
					static_cast<EditableText&>(*dynamicText).moveCursorToRightWordEnd(!(mods & KEY_MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeTextOffsetAndCursorOffsetKeepTextOffset();
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*dynamicText).moveCursorToRight(!(mods & KEY_MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeTextOffsetAndCursorOffsetKeepTextOffset();
					computeModelMatrixCursor();
				}
			}
			else if (key == KEY_BACKSPACE) {
				if (mods & KEY_MOD_CONTROL) {
					static_cast<EditableText&>(*dynamicText).deleteWordBeforeCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				else {
					static_cast<EditableText&>(*dynamicText).deleteCharacterBeforeCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				recomputeText();
				computeTextOffsetAndCursorOffsetKeepCursorOffset();
				computeModelMatrixCursor();
			}
			else if (key == KEY_DELETE) {
				if (mods & KEY_MOD_CONTROL) {
					static_cast<EditableText&>(*dynamicText).deleteWordAfterCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				else {
					static_cast<EditableText&>(*dynamicText).deleteCharacterAfterCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				recomputeText();
				computeTextOffsetAndCursorOffsetKeepCursorOffset();
				computeModelMatrixCursor();
			}
			else if (key == KEY_ENTER) {
				if (dynamicText->getParseMode() == StaticText::ParseMode::SINGLE_LINE) {
					// Special single line-mode
					active = false;
					cursorIsVisible = false;
					signOffEvent(CallbackType::CHARACTER_INPUT);
					signOffEvent(CallbackType::KEYBOARD);
					signOffEvent(CallbackType::UPDATE);
					signOffEvent(CallbackType::MOUSE_BUTTON);
					signOffEvent(CallbackType::MOUSE_MOTION);
					OnTextEdit();
					computeTextOffsetAndCursorOffset();
					eventEnterWasPressed.trigger();
					// Kill selection
					static_cast<EditableText&>(*dynamicText).setSelectionIndexToCursor();
				}
				else {
					static_cast<EditableText&>(*dynamicText).inputNewlineAtCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				recomputeText();
				computeTextOffsetAndCursorOffset();
				computeModelMatrixCursor();
			}
			else if (key == KEY_ESCAPE) {
				active = false;
				cursorIsVisible = false;
				signOffEvent(CallbackType::CHARACTER_INPUT);
				signOffEvent(CallbackType::KEYBOARD);
				signOffEvent(CallbackType::UPDATE);
				signOffEvent(CallbackType::MOUSE_BUTTON);
				signOffEvent(CallbackType::MOUSE_MOTION);
				// Kill selection
				static_cast<EditableText&>(*dynamicText).setSelectionIndexToCursor();
				computeTextOffsetAndCursorOffset();
			}
			else if (key == KEY_C && mods & KEY_MOD_CONTROL) {
				// Put selected text as UTF-8 encoded string into the clipboard
				if (static_cast<EditableText&>(*dynamicText).isSelecting()) {
					Engine::setClipboardString(static_cast<EditableText&>(*dynamicText).getSelectedText());
				}
			}
			else if (key == KEY_V && mods & KEY_MOD_CONTROL) {
				std::optional<std::string> clipboard = Engine::getClipboardString();
				// Input (hopefully) UTF-8 encoded string into text object
				if (clipboard.has_value()) static_cast<EditableText&>(*dynamicText).inputAtCursor(clipboard.value());
				cursorBlinkingTimer.reset();
				cursorIsVisible = true;
				recomputeText();
				computeTextOffsetAndCursorOffset();
				computeModelMatrixCursor();
			}
			else if (key == KEY_A && mods & KEY_MOD_CONTROL) {
				// Select all
				static_cast<EditableText&>(*dynamicText).setCursorPos(0);
				static_cast<EditableText&>(*dynamicText).setCursorPos(static_cast<EditableText&>(*dynamicText).getNumCharacters(), false);
				cursorBlinkingTimer.reset();
				cursorIsVisible = true;
				computeTextOffsetAndCursorOffset();
				computeModelMatrixCursor();
			}
			else if (key == KEY_X && mods & KEY_MOD_CONTROL) {
				// Delete selection and copy
				if (static_cast<EditableText&>(*dynamicText).isSelecting()) {
					Engine::setClipboardString(static_cast<EditableText&>(*dynamicText).getSelectedText());
					static_cast<EditableText&>(*dynamicText).deleteWordAfterCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					recomputeText();
					computeTextOffsetAndCursorOffset();
					computeModelMatrixCursor();
				}
			}
			// Compute selection boxes
			computeModelMatricesSelectionBoxes();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::callbackCharacterInput(const unsigned int& codepoint)
	{
		if (active)
		{
			static_cast<EditableText&>(*dynamicText).inputAtCursor(codepoint);
			cursorBlinkingTimer.reset();
			cursorIsVisible = true;
			recomputeText();
			computeTextOffsetAndCursorOffset();
			computeModelMatrixCursor();
			computeModelMatricesSelectionBoxes();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			active = true;
			signUpEvent(CallbackType::CHARACTER_INPUT);
			signUpEvent(CallbackType::KEYBOARD);
			signUpEvent(CallbackType::UPDATE);
			signUpEvent(CallbackType::MOUSE_BUTTON);
			signUpEvent(CallbackType::MOUSE_MOTION);
			Vec2d mousePos = getMouseOffsetToText();
			static_cast<EditableText&>(*dynamicText).computeCursorPosFromMousePos(mousePos / scaleFactor);
			computeTextOffsetAndCursorOffsetKeepTextOffset();
			computeModelMatrixCursor();
			computeModelMatricesSelectionBoxes();
			cursorBlinkingTimer.reset();
			cursorIsVisible = true;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::IBEAM);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::update(const double& dt)
	{
		if (active) {
			if (cursorBlinkingTimer.tick(dt).first) {
				cursorIsVisible = !cursorIsVisible;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
}