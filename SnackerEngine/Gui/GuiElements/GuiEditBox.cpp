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
	void GuiEditBox::computeModelMatrixCursor()
	{
		unsigned int DPI = Engine::getDPI().y;
		if (!dynamicText) return;
		Vec2f cursorOffset = static_cast<EditableText&>(*dynamicText).getCursorPos() * static_cast<float>(scaleFactor);
		cursorOffset.x = pointsToInches(cursorOffset.x);
		cursorOffset.y = pointsToInches(cursorOffset.y);
		cursorOffset *= static_cast<float>(DPI);
		Vec2f cursorSize = static_cast<EditableText&>(*dynamicText).getCursorSize() * static_cast<float>(scaleFactor);
		cursorSize.x = pointsToInches(cursorSize.x);
		cursorSize.y = pointsToInches(cursorSize.y);
		cursorSize *= static_cast<float>(DPI);
		Vec2f textOffset = computeTextPosition();
		modelMatrixCursor = Mat4f::TranslateAndScale(Vec3f(textOffset.x + cursorOffset.x, textOffset.y + cursorOffset.y, 0.0f), cursorSize);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::computeModelMatricesSelectionBoxes()
	{
		if (!dynamicText) return;
		modelMatricesSelectionBoxes.clear();
		auto result = static_cast<EditableText&>(*dynamicText).getSelectionBoxes();
		if (result.empty()) return;
		Vec2f textPosition = computeTextPosition();
		unsigned int DPI = Engine::getDPI().y;
		for (const auto& selectionBox : result) {
			modelMatricesSelectionBoxes.push_back(Mat4f::TranslateAndScale(textPosition + pointsToInches(selectionBox.position) * static_cast<float>(scaleFactor) * static_cast<float>(DPI), pointsToInches(selectionBox.size) * static_cast<float>(scaleFactor) * static_cast<float>(DPI)));
		}
	}
	//--------------------------------------------------------------------------------------------------
	Vec2d GuiEditBox::getMouseOffsetToText()
	{
		Vec2d DPI = Engine::getDPI();
		// Compute offset due to centering
		Vec2f textOffset = computeTextPosition();
		// Get mouse offset to upper left corner of element
		Vec2d mousePos = getMouseOffset();
		mousePos = Vec2i(-static_cast<int>(textOffset.x), -static_cast<int>(textOffset.y)) + Vec2i(static_cast<int>(mousePos.x), -static_cast<int>(mousePos.y));
		mousePos.x /= DPI.x;
		mousePos.y /= DPI.y;
		mousePos /= pointsToInches(1.0);
		return mousePos;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::OnTextEdit()
	{
		eventTextWasEdited.trigger();
		if (dynamicText) setText(dynamicText->getText());
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox::GuiEditBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font, const double& fontSize, const Color4f& backgroundColor)
		: GuiTextBox(position, size, text, font, fontSize, backgroundColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiEditBox::GuiEditBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		parseJsonOrReadFromData(selectionBoxColor, "selectionBoxColor", json, data, parameterNames);
		parseJsonOrReadFromData(cursorWidth, "cursorWidth", json, data, parameterNames);
		std::optional<double> cursorBlinkTime = parseJsonOrReadFromData<double>("cursorBlinkTime", json, data, parameterNames);
		if (cursorBlinkTime.has_value()) cursorBlinkingTimer.setTimeStep(cursorBlinkTime.value());
		if (!json.contains("backgroundColor")) setBackgroundColor(defaultBackgroundColor);
		if (!json.contains("sizeHintModeMinSize") && getTextScaleMode() != TextScaleMode::SCALE_DOWN && getTextScaleMode() != TextScaleMode::SCALE_UP_DOWN &&
			getTextScaleMode() != TextScaleMode::RECOMPUTE_DOWN && getTextScaleMode() != TextScaleMode::RECOMPUTE_UP_DOWN) setSizeHintModeMinSize(defaultSizeHintModes.sizeHintModeMinSize);
		if (!json.contains("sizeHintModePreferredSize") && getTextScaleMode() == TextScaleMode::DONT_SCALE) setSizeHintModePreferredSize(defaultSizeHintModes.sizeHintModePreferredSize);
		if (!json.contains("sizeHintModeMaxSize") && getTextScaleMode() != TextScaleMode::SCALE_UP && getTextScaleMode() != TextScaleMode::SCALE_UP_DOWN
			&& getTextScaleMode() != TextScaleMode::RECOMPUTE_UP_DOWN) setSizeHintModeMaxSize(defaultSizeHintModes.sizeHintModeMaxSize);
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
			getPanelShader().bind();
			guiManager->setUniformViewAndProjectionMatrices(getPanelShader());
			getPanelShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixSelectionBox);
			getPanelShader().setUniform<Color4f>("u_color", selectionBoxColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		// Draw text
		drawText(worldPosition);
		// Draw cursor
		if (cursorIsVisible) {
			getPanelShader().bind();
			guiManager->setUniformViewAndProjectionMatrices(getPanelShader());
			getPanelShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixCursor);
			getPanelShader().setUniform<Color4f>("u_color", getTextColor());
			Renderer::draw(guiManager->getModelSquare());
		}
		// Draw children
		GuiElement::draw(worldPosition);
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::onRegister()
	{
		GuiTextBox::onRegister(std::move(std::make_unique<EditableText>(getText(), getFont(), getFontSize(), inchesToPoints(static_cast<double>(getWidth() - 2 * getBorder()) / Engine::getDPI().y), cursorWidth, getParseMode(), getAlignment())));
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(CallbackType::MOUSE_ENTER);
		signUpEvent(CallbackType::MOUSE_LEAVE);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditBox::onSizeChange()
	{
		GuiTextBox::onSizeChange();
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
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*dynamicText).moveCursorToLeft(!(mods & KEY_MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
			}
			else if (key == KEY_RIGHT) {
				if (mods & KEY_MOD_CONTROL) {
					static_cast<EditableText&>(*dynamicText).moveCursorToRightWordEnd(!(mods & KEY_MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*dynamicText).moveCursorToRight(!(mods & KEY_MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
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
			}
			else if (key == KEY_C && mods & KEY_MOD_CONTROL) {
				// Put selected text as UTF-8 encoded string into the clipboard
				Engine::setClipboardString(static_cast<EditableText&>(*dynamicText).getSelectedText());
			}
			else if (key == KEY_V && mods & KEY_MOD_CONTROL) {
				std::optional<std::string> clipboard = Engine::getClipboardString();
				// Input (hopefully) UTF-8 encoded string into text object
				if (clipboard.has_value()) static_cast<EditableText&>(*dynamicText).inputAtCursor(clipboard.value());
				cursorBlinkingTimer.reset();
				cursorIsVisible = true;
				recomputeText();
				computeModelMatrixCursor();
				computeModelMatricesSelectionBoxes();
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