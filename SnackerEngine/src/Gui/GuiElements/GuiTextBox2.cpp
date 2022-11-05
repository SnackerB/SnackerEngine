#include "Gui/GuiElements/GuiTextBox2.h"

#include "Core/Engine.h"
#include "Math/Utility.h"
#include "Gui/GuiManager2.h"
#include "Graphics/Renderer.h"
#include "Gui/Text/TextMaterial.h"
#include "Gui/GuiStyle.h"
#include "core/Keys.h"

#include <memory>

namespace SnackerEngine
{

	void GuiDynamicTextBox2::computeModelMatrices()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f), Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
		unsigned int DPI = Engine::getDPI().y;
		// Compute offset due to centering
		double textOffsetY;
		if (text->getAlignment() == StaticText::Alignment::CENTER) {
			// Align to the center of the text box
			// TODO: Why is this *1.0 necessary? Does not work if i remove it. Compiler bug?
			textOffsetY = -pointsToInches((text->getTop() * 1.0 + text->getBottom()) / 2.0) * DPI - size.y / 2.0;
		}
		else {
			// Align to the top of the text box
			textOffsetY = -pointsToInches(text->getTop()) * DPI;
		}
		modelMatrixText = Mat4f::TranslateAndScale(Vec3f(position.x, -position.y + textOffsetY, 0), pointsToInches(text->getFontSize()) * DPI);
	}

	void GuiDynamicTextBox2::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		// Draw background
		if (backgroundColor.alpha != 0.0f) {
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
			backgroundShader.setUniform<Color3f>("u_color", Color3f(backgroundColor.r, backgroundColor.g, backgroundColor.b)); // TODO: Transparent background
			Renderer::draw(guiManager->getModelSquare());
		}
		// Draw text
		material.bind();
		guiManager->setUniformViewAndProjectionMatrices(material.getShader());
		material.getShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixText);
		material.getShader().setUniform<float>("u_pxRange", text->getFont().getPixelRange());
		SnackerEngine::Renderer::draw(text->getModel(), material);
		// Draw children
		GuiElement2::draw(parentPosition);
	}

	void GuiDynamicTextBox2::onPositionChange()
	{
		GuiElement2::onPositionChange();
		computeModelMatrices();
	}

	void GuiDynamicTextBox2::onSizeChange()
	{
		GuiElement2::onSizeChange();
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	GuiDynamicTextBox2::IsCollidingResult GuiDynamicTextBox2::isColliding(const Vec2i& position)
	{
		return (position.x > this->position.x && position.x < this->position.x + size.x
			&& position.y > this->position.y && position.y < this->position.y + size.y) ?
			IsCollidingResult::COLLIDE_CHILD : IsCollidingResult::NOT_COLLIDING;
	}

	void GuiDynamicTextBox2::onRegister()
	{
		GuiElement2::onRegister();
	}

	void GuiDynamicTextBox2::resizeToText()
	{
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::FORCE_SIZE:
		{
			break;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			size.y = static_cast<int>((text->getTop() * 1.0 - text->getBottom()) * Engine::getDPI().y * pointsToInches(1.0));
			break;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			size.x = static_cast<int>((text->getRight() * 1.0 - text->getLeft()) * Engine::getDPI().x * pointsToInches(1.0));
			break;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_TO_FIT:
		{
			size.y = static_cast<int>((text->getTop()*1.0 - text->getBottom()) * Engine::getDPI().y * pointsToInches(1.0));
			size.x = static_cast<int>((text->getRight()*1.0 - text->getLeft()) * Engine::getDPI().x * pointsToInches(1.0));
			break;
		}
		default:
			break;
		}
	}

	void GuiDynamicTextBox2::resizeAndRecomputeText()
	{
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::FORCE_SIZE: 
		{
			// Just compute the text with the size.x as textWidth:
			if (singleLine) text->setTextWidth(0.0);
			else text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f));
			break;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			// compute the text with size.x as textWidth
			if (singleLine) text->setTextWidth(0.0);
			else text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f));
			resizeToText();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			// compute the text with zero textWidth
			text->setTextWidth(0.0);
			resizeToText();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_TO_FIT:
		{
			// compute the text with zero textWidth
			text->setTextWidth(0.0);
			resizeToText();
			break;
		}
		default:
			break;
		}
	}

	/// Helper function for constructing the text material
	static Material constructTextMaterial(const Font& font, const Color4f& textColor, const Color4f& backgroundColor)
	{
		return Material(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"),
			font, textColor,
			backgroundColor.alpha == 1.0 ? backgroundColor : Color4f(textColor.r, textColor.g, textColor.b, 0.0f))));
	}

	GuiDynamicTextBox2::GuiDynamicTextBox2(const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, std::unique_ptr<DynamicText>&& text, const Color4f& textColor, const Color4f& backgroundColor, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiElement2(position, size, resizeMode), text(std::move(text)),
		material(constructTextMaterial(this->text->getFont(), textColor, backgroundColor)),
		backgroundColor(backgroundColor), textColor(textColor), backgroundShader("shaders/gui/simpleColor.shader"),
		modelMatrixText{}, modelMatrixBackground{}, textBoxMode(textBoxMode), singleLine(singleLine)
	{
		computeModelMatrices();
	}

	/// Helper function that computes which textWidth is necessary for the given resizeMode and size vector
	static double computeTextwidth(const GuiDynamicTextBox2::TextBoxMode textBoxMode, const Vec2i& size, const bool& singleLine) {
		if (singleLine) return 0.0;
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::FORCE_SIZE:
		{
			return size.x;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			return size.x;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			return 0.0;
		}
		case SnackerEngine::GuiDynamicTextBox2::TextBoxMode::SHRINK_TO_FIT:
		{
			return 0.0;
		}
		default:
			break;
		}
		return 0.0;
	}

	GuiDynamicTextBox2::GuiDynamicTextBox2(const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, const std::string& text, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiDynamicTextBox2(position, size, resizeMode, std::make_unique<DynamicText>(text, font, fontSize, computeTextwidth(textBoxMode, size, singleLine), parseMode, alignment),
			textColor, backgroundColor, textBoxMode, singleLine) {}

	GuiDynamicTextBox2::GuiDynamicTextBox2(const std::string& text, const GuiStyle& style)
		: GuiDynamicTextBox2(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text, 
			style.defaultFont, style.fontSizeNormal, style.guiTextBoxTextColor, 
			style.guiTextBoxBackgroundColor, style.guiTextBoxParseMode, 
			style.guiTextBoxAlignment, style.guiTextBoxMode2, style.guiTextBoxSingleLine) {}

	GuiDynamicTextBox2::GuiDynamicTextBox2(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiDynamicTextBox2(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text,
			style.defaultFont, fontSize, style.guiTextBoxTextColor,
			style.guiTextBoxBackgroundColor, style.guiTextBoxParseMode,
			style.guiTextBoxAlignment, style.guiTextBoxMode2, style.guiTextBoxSingleLine) {}

	GuiDynamicTextBox2::GuiDynamicTextBox2(const GuiDynamicTextBox2& other) noexcept
		: GuiElement2(other), text(std::make_unique<DynamicText>(*other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode), singleLine(other.singleLine) {}

	GuiDynamicTextBox2::GuiDynamicTextBox2(GuiDynamicTextBox2&& other) noexcept
		: GuiElement2(std::move(other)), text(std::move(other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode), singleLine(other.singleLine)
	{
		// set other to a well defined state
		other.text = nullptr;
	}

	GuiDynamicTextBox2& GuiDynamicTextBox2::operator=(const GuiDynamicTextBox2& other) noexcept
	{
		GuiElement2::operator=(other);
		text = std::make_unique<DynamicText>(*other.text);
		material = other.material;
		backgroundColor = other.backgroundColor;
		textColor = other.textColor;
		backgroundShader = other.backgroundShader;
		modelMatrixText = other.modelMatrixText;
		modelMatrixBackground = other.modelMatrixBackground;
		textBoxMode = other.textBoxMode;
		singleLine = other.singleLine;
		return *this;
	}

	GuiDynamicTextBox2& GuiDynamicTextBox2::operator=(GuiDynamicTextBox2&& other) noexcept
	{
		GuiElement2::operator=(std::move(other));
		text = std::move(other.text);
		material = other.material;
		backgroundColor = other.backgroundColor;
		textColor = other.textColor;
		backgroundShader = other.backgroundShader;
		modelMatrixText = other.modelMatrixText;
		modelMatrixBackground = other.modelMatrixBackground;
		textBoxMode = other.textBoxMode;
		singleLine = other.singleLine;
		// set other to a well defined state
		other.text = nullptr;
		return *this;
	}

	void GuiDynamicTextBox2::setText(const std::string& text)
	{
		this->text->setText(text);
		resizeToText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox2::setFont(const Font& font)
	{
		this->text->setFont(font);
		material = constructTextMaterial(text->getFont(), textColor, backgroundColor);
		resizeToText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox2::setFontSize(const double& fontSize)
	{
		this->text->setFontSize(fontSize);
		resizeToText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox2::setTextColor(const Color4f& textColor)
	{
		this->textColor = textColor;
		material = constructTextMaterial(text->getFont(), textColor, backgroundColor);
	}

	void GuiDynamicTextBox2::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
		material = constructTextMaterial(text->getFont(), textColor, backgroundColor);
	}

	void GuiDynamicTextBox2::setTextParseMode(const StaticText::ParseMode& parseMode)
	{
		text->setParseMode(parseMode);
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox2::setAlignment(const StaticText::Alignment& alignment)
	{
		this->text->setAlignment(alignment);
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox2::setTextBoxMode(const TextBoxMode& textBoxMode)
	{
		this->textBoxMode = textBoxMode;
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox2::setSingleLine(const bool& singleLine)
	{
		this->singleLine = singleLine;
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	void GuiEditTextBox2::computeModelMatrixCursor()
	{
		unsigned int DPI = Engine::getDPI().y;
		Vec2f cursorOffset = static_cast<EditableText&>(*text).getCursorPos();
		cursorOffset.x = pointsToInches(cursorOffset.x);
		cursorOffset.y = pointsToInches(cursorOffset.y);
		cursorOffset *= DPI;
		Vec2f cursorSize = static_cast<EditableText&>(*text).getCursorSize();
		cursorSize.x = pointsToInches(cursorSize.x);
		cursorSize.y = pointsToInches(cursorSize.y);
		cursorSize *= DPI;
		float additionalOffset = pointsToInches(text->getFont().getAscender()) * DPI * text->getFontSize();
		modelMatrixCursor = Mat4f::TranslateAndScale(Vec3f(position.x + cursorOffset.x + cursorSize.x, -position.y + cursorOffset.y - additionalOffset, 0.0f), cursorSize);
	}

	void GuiEditTextBox2::draw(const Vec2i& parentPosition)
	{
		GuiDynamicTextBox2::draw(parentPosition);
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		// Draw cursor
		if (cursorIsVisible) {
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixCursor);
			backgroundShader.setUniform<Color3f>("u_color", Color3f(textColor.r, textColor.g, textColor.b)); // TODO: Transparent background
			Renderer::draw(guiManager->getModelSquare());
		}
	}

	void GuiEditTextBox2::onPositionChange()
	{
		GuiDynamicTextBox2::onPositionChange();
		computeModelMatrixCursor();
	}

	void GuiEditTextBox2::onSizeChange()
	{
		GuiDynamicTextBox2::onSizeChange();
		computeModelMatrixCursor();
	}

	GuiEditTextBox2::IsCollidingResult GuiEditTextBox2::isColliding(const Vec2i& position)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (position.x > myPosition.x && position.x < myPosition.x + mySize.x
			&& position.y > myPosition.y && position.y < myPosition.y + mySize.y) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}

	void GuiEditTextBox2::onRegister()
	{
		GuiDynamicTextBox2::onRegister();
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}

	void GuiEditTextBox2::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (active && button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			if (isColliding(getMouseOffset(getParentID())) == IsCollidingResult::NOT_COLLIDING) {
				active = false;
				cursorIsVisible = false;
				signOffEvent(CallbackType::CHARACTER_INPUT);
				signOffEvent(CallbackType::KEYBOARD);
				signOffEvent(CallbackType::UPDATE);
				signOffEvent(CallbackType::MOUSE_BUTTON);
			}
		}
	}

	void GuiEditTextBox2::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		if (active && (action == ACTION_PRESS || action == ACTION_REPEAT))
		{
			if (key == KEY_LEFT) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).moveCursorToLeftWordBeginning();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					setSize(size);
				}
				else {
					static_cast<EditableText&>(*text).moveCursorToLeft();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					setSize(size);
				}
			}
			else if (key == KEY_RIGHT) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).moveCursorToRightWordEnd();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					setSize(size);
				}
				else {
					static_cast<EditableText&>(*text).moveCursorToRight();
					setSize(size);
				}
			}
			else if (key == KEY_BACKSPACE) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).deleteWordBeforeCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					setSize(size);
				}
				else {
					static_cast<EditableText&>(*text).deleteCharacterBeforeCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					setSize(size);
				}
			}
			else if (key == KEY_ENTER) {
				if (text->getTextWidth() == 0.0) {
					// Special single line-mode
					active = false;
					cursorIsVisible = false;
					signOffEvent(CallbackType::CHARACTER_INPUT);
					signOffEvent(CallbackType::KEYBOARD);
					signOffEvent(CallbackType::UPDATE);
					signOffEvent(CallbackType::MOUSE_BUTTON);
					// if (eventHandleTextWasEdited) activate(*eventHandleTextWasEdited); TODO: Uncomment
				}
				else {
					static_cast<EditableText&>(*text).inputNewlineAtCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					setSize(size);
				}
			}
			else if (key == KEY_ESCAPE) {
				active = false;
				cursorIsVisible = false;
				signOffEvent(CallbackType::CHARACTER_INPUT);
				signOffEvent(CallbackType::KEYBOARD);
				signOffEvent(CallbackType::UPDATE);
				signOffEvent(CallbackType::MOUSE_BUTTON);
			}
		}
	}

	void GuiEditTextBox2::callbackCharacterInput(const unsigned int& codepoint)
	{
		if (active)
		{
			static_cast<EditableText&>(*text).inputAtCursor(codepoint);
			cursorBlinkingTimer.reset();
			cursorIsVisible = true;
			setSize(size);
		}
	}

	void GuiEditTextBox2::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			active = true;
			signUpEvent(CallbackType::CHARACTER_INPUT);
			signUpEvent(CallbackType::KEYBOARD);
			signUpEvent(CallbackType::UPDATE);
			signUpEvent(CallbackType::MOUSE_BUTTON);
			cursorBlinkingTimer.reset();
			cursorIsVisible = true;
		}
	}

	void GuiEditTextBox2::update(const double& dt)
	{
		if (active) {
			if (cursorBlinkingTimer.tick(dt).first) {
				cursorIsVisible = !cursorIsVisible;
			}
		}
	}

	GuiEditTextBox2::GuiEditTextBox2(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const std::string& text, const Font& font, const double& fontSize, const double& cursorWidth, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine, const double& cursorBlinkTime)
		: GuiDynamicTextBox2(position, size, resizeMode, 
			std::move(std::make_unique<EditableText>(text, font, fontSize, computeTextwidth(textBoxMode, size, singleLine), cursorWidth)),
			textColor, backgroundColor, textBoxMode, singleLine),
		active(false), cursorIsVisible(false), cursorBlinkingTimer(cursorBlinkTime), modelMatrixCursor{} {}

	GuiEditTextBox2::GuiEditTextBox2(const std::string& text, const GuiStyle& style)
		: GuiEditTextBox2(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text, style.defaultFont, style.fontSizeNormal,
			style.guiEditTextBoxCursorWidth, style.guiTextBoxTextColor, style.guiEditTextBoxBackgroundColor,
			style.guiTextBoxParseMode, style.guiTextBoxAlignment, style.guiTextBoxMode2,
			style.guiEditTextBoxSingleLine, style.guiEditTextBoxBlinkTime) {}

	GuiEditTextBox2::GuiEditTextBox2(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiEditTextBox2(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text, style.defaultFont, fontSize,
			style.guiEditTextBoxCursorWidth, style.guiTextBoxTextColor, style.guiEditTextBoxBackgroundColor,
			style.guiTextBoxParseMode, style.guiTextBoxAlignment, style.guiTextBoxMode2,
			style.guiEditTextBoxSingleLine, style.guiEditTextBoxBlinkTime) {}

	// TODO: Edit copy/move constructor/assignment operators when adding support for handles!
	GuiEditTextBox2::GuiEditTextBox2(const GuiEditTextBox2& other) noexcept
		: GuiDynamicTextBox2(other), active(false), cursorIsVisible(false), 
		cursorBlinkingTimer(other.cursorBlinkingTimer) {}

	GuiEditTextBox2::GuiEditTextBox2(GuiEditTextBox2&& other) noexcept
		: GuiDynamicTextBox2(std::move(other)), active(false), cursorIsVisible(false),
		cursorBlinkingTimer(std::move(other.cursorBlinkingTimer)) {}

	GuiEditTextBox2& GuiEditTextBox2::operator=(const GuiEditTextBox2& other) noexcept
	{
		GuiDynamicTextBox2::operator=(other);
		active = false;
		cursorIsVisible = false;
		cursorBlinkingTimer = other.cursorBlinkingTimer;
		return *this;
	}

	GuiEditTextBox2& GuiEditTextBox2::operator=(GuiEditTextBox2&& other) noexcept
	{
		GuiDynamicTextBox2::operator=(std::move(other));
		active = false;
		cursorIsVisible = false;
		cursorBlinkingTimer = std::move(other.cursorBlinkingTimer);
		//eventHandleTextWasEdited = std::move(other.eventHandleTextWasEdited);
		//other.eventHandleTextWasEdited = nullptr;
		return *this;
	}

	void GuiEditTextBox2::setCursorWidth(const double& cursorWidth)
	{
		static_cast<EditableText&>(*text).setCursorWidth(cursorWidth);
	}

	void GuiEditTextBox2::setCursorBlinkTime(const double& cursorBlinkTime)
	{
		cursorBlinkingTimer.setTimeStep(cursorBlinkTime);
	}

}