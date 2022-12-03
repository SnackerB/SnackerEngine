#include "Gui/GuiElements/GuiTextBox.h"

#include "Core/Engine.h"
#include "Math/Utility.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Gui/Text/TextMaterial.h"
#include "Gui/GuiStyle.h"
#include "core/Keys.h"

#include <memory>

namespace SnackerEngine
{

	void GuiDynamicTextBox::computeModelMatrices()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f), Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
		unsigned int DPI = Engine::getDPI().y;
		Vec2f textPosition = computeTextPosition();
		modelMatrixText = Mat4f::TranslateAndScale(Vec3f(textPosition.x, textPosition.y, 0), pointsToInches<float>(static_cast<float>(text->getFontSize())) * static_cast<float>(DPI));
	}

	void GuiDynamicTextBox::draw(const Vec2i& parentPosition)
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
		pushClippingBox(parentPosition);
		// Draw text
		material.bind();
		guiManager->setUniformViewAndProjectionMatrices(material.getShader());
		material.getShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixText);
		material.getShader().setUniform<float>("u_pxRange", static_cast<float>(text->getFont().getPixelRange()));
		SnackerEngine::Renderer::draw(text->getModel(), material);
		// Draw children
		GuiElement::draw(parentPosition);
		popClippingBox();
	}

	void GuiDynamicTextBox::onPositionChange()
	{
		GuiElement::onPositionChange();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::onSizeChange()
	{
		GuiElement::onSizeChange();
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	GuiDynamicTextBox::IsCollidingResult GuiDynamicTextBox::isColliding(const Vec2i& position)
	{
		return (position.x > this->position.x && position.x < this->position.x + size.x
			&& position.y > this->position.y && position.y < this->position.y + size.y) ?
			IsCollidingResult::COLLIDE_CHILD : IsCollidingResult::NOT_COLLIDING;
	}

	void GuiDynamicTextBox::onRegister()
	{
		GuiElement::onRegister();
	}

	void GuiDynamicTextBox::resizeToText()
	{
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE:
		{
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			size.y = static_cast<int>(std::ceil((text->getTop() - text->getBottom()) * Engine::getDPI().y * pointsToInches(1.0)));
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			size.x = static_cast<int>(std::ceil((text->getRight() - text->getLeft()) * Engine::getDPI().x * pointsToInches(1.0)));
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_TO_FIT:
		{
			size.y = static_cast<int>(std::ceil((text->getTop() - text->getBottom()) * Engine::getDPI().y * pointsToInches(1.0)));
			size.x = static_cast<int>(std::ceil((text->getRight() - text->getLeft()) * Engine::getDPI().x * pointsToInches(1.0)));
			break;
		}
		default:
			break;
		}
	}

	void GuiDynamicTextBox::resizeAndRecomputeText()
	{
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE: 
		{
			// Just compute the text with the size.x as textWidth:
			if (singleLine) text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f));
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			// compute the text with size.x as textWidth
			if (singleLine) text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f));
			resizeToText();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			// compute the text with zero textWidth
			text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(0.0);
			resizeToText();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_TO_FIT:
		{
			// compute the text with zero textWidth
			text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(0.0);
			resizeToText();
			break;
		}
		default:
			break;
		}
	}

	Vec2f GuiDynamicTextBox::computeTextPosition()
	{
		unsigned int DPI = Engine::getDPI().y;
		// Compute offset due to centering
		double textOffsetY;
		if (text->getAlignment() == StaticText::Alignment::CENTER) {
			// Align to the center of the text box
			textOffsetY = -pointsToInches((text->getTop() + text->getBottom()) / 2.0) * DPI - size.y / 2.0;
		}
		else {
			// Align to the top of the text box
			textOffsetY = -pointsToInches(text->getTop()) * DPI;
		}
		return Vec2f(static_cast<float>(position.x), -static_cast<float>(position.y) + static_cast<float>(textOffsetY));
	}

	/// Helper function for constructing the text material
	static Material constructTextMaterial(const Font& font, const Color4f& textColor, const Color4f& backgroundColor)
	{
		return Material(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"),
			font, textColor,
			backgroundColor.alpha == 1.0 ? backgroundColor : Color4f(textColor.r, textColor.g, textColor.b, 0.0f))));
	}

	GuiDynamicTextBox::GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, std::unique_ptr<DynamicText>&& text, const Color4f& textColor, const Color4f& backgroundColor, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiElement(position, size, resizeMode), text(std::move(text)),
		material(constructTextMaterial(this->text->getFont(), textColor, Color4f(0.0f, 0.0f))),
		backgroundColor(backgroundColor), textColor(textColor), backgroundShader("shaders/gui/simpleColor.shader"),
		modelMatrixText{}, modelMatrixBackground{}, textBoxMode(textBoxMode), singleLine(singleLine)
	{
		// Check if the size needs to be adjusted
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_TO_FIT:
		{
			resizeToText(); break;
		}
		default:
			break;
		}
		computeModelMatrices();
	}

	/// Helper function that computes which textWidth is necessary for the given resizeMode and size vector
	static double computeTextwidth(const GuiDynamicTextBox::TextBoxMode textBoxMode, const Vec2i& size, const bool& singleLine) {
		if (singleLine) return 0.0;
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE:
		{
			return size.x;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			return size.x;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			return 0.0;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_TO_FIT:
		{
			return 0.0;
		}
		default:
			break;
		}
		return 0.0;
	}

	GuiDynamicTextBox::GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& text, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiDynamicTextBox(position, size, resizeMode, std::make_unique<DynamicText>(text, font, fontSize, computeTextwidth(textBoxMode, size, singleLine), parseMode, alignment),
			textColor, backgroundColor, textBoxMode, singleLine) {}

	GuiDynamicTextBox::GuiDynamicTextBox(const std::string& text, const GuiStyle& style)
		: GuiDynamicTextBox(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text, 
			style.defaultFont, style.fontSizeNormal, style.guiTextBoxTextColor, 
			style.guiTextBoxBackgroundColor, style.guiTextBoxParseMode, 
			style.guiTextBoxAlignment, style.guiTextBoxMode2, style.guiTextBoxSingleLine) {}

	GuiDynamicTextBox::GuiDynamicTextBox(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiDynamicTextBox(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text,
			style.defaultFont, fontSize, style.guiTextBoxTextColor,
			style.guiTextBoxBackgroundColor, style.guiTextBoxParseMode,
			style.guiTextBoxAlignment, style.guiTextBoxMode2, style.guiTextBoxSingleLine) {}

	GuiDynamicTextBox::GuiDynamicTextBox(const GuiDynamicTextBox& other) noexcept
		: GuiElement(other), text(std::make_unique<DynamicText>(*other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode), singleLine(other.singleLine) {}

	GuiDynamicTextBox::GuiDynamicTextBox(GuiDynamicTextBox&& other) noexcept
		: GuiElement(std::move(other)), text(std::move(other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode), singleLine(other.singleLine)
	{
		// set other to a well defined state
		other.text = nullptr;
	}

	GuiDynamicTextBox& GuiDynamicTextBox::operator=(const GuiDynamicTextBox& other) noexcept
	{
		GuiElement::operator=(other);
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

	GuiDynamicTextBox& GuiDynamicTextBox::operator=(GuiDynamicTextBox&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
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

	void GuiDynamicTextBox::setText(const std::string& text)
	{
		this->text->setText(text);
		resizeToText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::setFont(const Font& font)
	{
		this->text->setFont(font);
		material = constructTextMaterial(text->getFont(), textColor, backgroundColor);
		resizeToText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::setFontSize(const double& fontSize)
	{
		this->text->setFontSize(fontSize);
		resizeToText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::setTextColor(const Color4f& textColor)
	{
		this->textColor = textColor;
		material = constructTextMaterial(text->getFont(), textColor, backgroundColor);
	}

	void GuiDynamicTextBox::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
		material = constructTextMaterial(text->getFont(), textColor, backgroundColor);
	}

	void GuiDynamicTextBox::setTextParseMode(const StaticText::ParseMode& parseMode)
	{
		text->setParseMode(parseMode);
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::setAlignment(const StaticText::Alignment& alignment)
	{
		this->text->setAlignment(alignment);
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::setTextBoxMode(const TextBoxMode& textBoxMode)
	{
		this->textBoxMode = textBoxMode;
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::setSingleLine(const bool& singleLine)
	{
		this->singleLine = singleLine;
		resizeAndRecomputeText();
		computeModelMatrices();
	}

	const Vec2i GuiDynamicTextBox::getTextSize() const
	{
		if (!text) return Vec2i();
		return Vec2i( 
			static_cast<int>(std::ceil((text->getRight() * 1.0 - text->getLeft()) * Engine::getDPI().x * pointsToInches(1.0))),
			static_cast<int>(std::ceil((text->getTop() * 1.0 - text->getBottom()) * Engine::getDPI().y * pointsToInches(1.0)))
		);
	}

	void GuiEditTextBox::computeModelMatrixCursor()
	{
		unsigned int DPI = Engine::getDPI().y;
		Vec2f cursorOffset = static_cast<EditableText&>(*text).getCursorPos();
		cursorOffset.x = pointsToInches(cursorOffset.x);
		cursorOffset.y = pointsToInches(cursorOffset.y);
		cursorOffset *= static_cast<float>(DPI);
		Vec2f cursorSize = static_cast<EditableText&>(*text).getCursorSize();
		cursorSize.x = pointsToInches(cursorSize.x);
		cursorSize.y = pointsToInches(cursorSize.y);
		cursorSize *= static_cast<float>(DPI);
		float additionalOffset = pointsToInches<float>(static_cast<float>(text->getFont().getAscender()) * static_cast<float>(DPI) * static_cast<float>(text->getFontSize()));
		modelMatrixCursor = Mat4f::TranslateAndScale(Vec3f(position.x + cursorOffset.x + cursorSize.x, -position.y + cursorOffset.y - additionalOffset, 0.0f), cursorSize);
	}

	void GuiEditTextBox::computeModelMatricesSelectionBoxes()
	{
		// TODO
		modelMatricesSelectionBoxes.clear();
		auto result = static_cast<EditableText&>(*text).getSelectionBoxes();
		if (result.empty()) return;
		Vec2f textPosition = computeTextPosition();
		unsigned int DPI = Engine::getDPI().y;
		for (const auto& selectionBox : result) {
			modelMatricesSelectionBoxes.push_back(Mat4f::TranslateAndScale(textPosition + pointsToInches(selectionBox.position) * static_cast<float>(DPI), pointsToInches(selectionBox.size) * static_cast<float>(DPI)));
		}
	}

	void GuiEditTextBox::draw(const Vec2i& parentPosition)
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
		// Draw selection boxes
		pushClippingBox(parentPosition);
		for (const auto& modelMatrixSelectionBox : modelMatricesSelectionBoxes) {
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixSelectionBox);
			backgroundShader.setUniform<Color3f>("u_color", Color3f(0.0f, 0.0f, 1.0f)); // TODO: Transparent background TODO: Correct color
			Renderer::draw(guiManager->getModelSquare());
		}
		// Draw text
		material.bind();
		guiManager->setUniformViewAndProjectionMatrices(material.getShader());
		material.getShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixText);
		material.getShader().setUniform<float>("u_pxRange", static_cast<float>(text->getFont().getPixelRange()));
		SnackerEngine::Renderer::draw(text->getModel(), material);
		// Draw cursor
		if (cursorIsVisible) {
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixCursor);
			backgroundShader.setUniform<Color3f>("u_color", Color3f(textColor.r, textColor.g, textColor.b)); // TODO: Transparent background
			Renderer::draw(guiManager->getModelSquare());
		}
		// Draw children
		GuiElement::draw(parentPosition);
		popClippingBox();
	}

	void GuiEditTextBox::onPositionChange()
	{
		GuiDynamicTextBox::onPositionChange();
		computeModelMatrixCursor();
	}

	void GuiEditTextBox::onSizeChange()
	{
		GuiDynamicTextBox::onSizeChange();
		computeModelMatrixCursor();
		computeModelMatricesSelectionBoxes();
	}

	GuiEditTextBox::IsCollidingResult GuiEditTextBox::isColliding(const Vec2i& position)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (position.x > myPosition.x && position.x < myPosition.x + mySize.x
			&& position.y > myPosition.y && position.y < myPosition.y + mySize.y) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}

	void GuiEditTextBox::onRegister()
	{
		GuiDynamicTextBox::onRegister();
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(CallbackType::MOUSE_ENTER);
		signUpEvent(CallbackType::MOUSE_LEAVE);
	}

	void GuiEditTextBox::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (active && button == MOUSE_BUTTON_LEFT) {
			if (action == ACTION_PRESS) {
				if (isColliding(getMouseOffset(getParentID())) == IsCollidingResult::NOT_COLLIDING) {
					active = false;
					cursorIsVisible = false;
					signOffEvent(CallbackType::CHARACTER_INPUT);
					signOffEvent(CallbackType::KEYBOARD);
					signOffEvent(CallbackType::UPDATE);
					signOffEvent(CallbackType::MOUSE_BUTTON);
					signOffEvent(CallbackType::MOUSE_MOTION);
					if (eventHandleTextWasEdited) activate(*eventHandleTextWasEdited);
					static_cast<EditableText&>(*text).setSelectionIndexToCursor();
					computeModelMatricesSelectionBoxes();
				}
			}
			else if (action == ACTION_RELEASE) {
				signOffEvent(CallbackType::MOUSE_MOTION);
			}
		}
	}

	void GuiEditTextBox::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		if (active && (action == ACTION_PRESS || action == ACTION_REPEAT))
		{
			if (key == KEY_LEFT) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).moveCursorToLeftWordBeginning(!(mods & MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*text).moveCursorToLeft(!(mods & MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
			}
			else if (key == KEY_RIGHT) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).moveCursorToRightWordEnd(!(mods & MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*text).moveCursorToRight(!(mods & MOD_SHIFT));
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
			}
			else if (key == KEY_BACKSPACE) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).deleteWordBeforeCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*text).deleteCharacterBeforeCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
			}
			else if (key == KEY_DELETE) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).deleteWordAfterCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
				else {
					static_cast<EditableText&>(*text).deleteCharacterAfterCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
			}
			else if (key == KEY_ENTER) {
				if (singleLine) {
					// Special single line-mode
					active = false;
					cursorIsVisible = false;
					signOffEvent(CallbackType::CHARACTER_INPUT);
					signOffEvent(CallbackType::KEYBOARD);
					signOffEvent(CallbackType::UPDATE);
					signOffEvent(CallbackType::MOUSE_BUTTON);
					signOffEvent(CallbackType::MOUSE_MOTION);
					if (eventHandleTextWasEdited) activate(*eventHandleTextWasEdited);
					// Kill selection
					static_cast<EditableText&>(*text).setSelectionIndexToCursor();
				}
				else {
					static_cast<EditableText&>(*text).inputNewlineAtCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
					computeModelMatrixCursor();
				}
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
				static_cast<EditableText&>(*text).setSelectionIndexToCursor();
			}
			// Compute selection boxes
			computeModelMatricesSelectionBoxes();
		}
	}

	void GuiEditTextBox::callbackCharacterInput(const unsigned int& codepoint)
	{
		if (active)
		{
			static_cast<EditableText&>(*text).inputAtCursor(codepoint);
			cursorBlinkingTimer.reset();
			cursorIsVisible = true;
			setSize(size);
			// Compute selection boxes
			computeModelMatricesSelectionBoxes();
		}
	}

	void GuiEditTextBox::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			active = true;
			signUpEvent(CallbackType::CHARACTER_INPUT);
			signUpEvent(CallbackType::KEYBOARD);
			signUpEvent(CallbackType::UPDATE);
			signUpEvent(CallbackType::MOUSE_BUTTON);
			signUpEvent(CallbackType::MOUSE_MOTION);
			Vec2d mousePos = getMouseOffsetToText();
			static_cast<EditableText&>(*text).computeCursorPosFromMousePos(mousePos);
			computeModelMatrixCursor();
			computeModelMatricesSelectionBoxes();
			cursorBlinkingTimer.reset();
			cursorIsVisible = true;
		}
	}

	void GuiEditTextBox::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::IBEAM);
	}

	void GuiEditTextBox::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}

	void GuiEditTextBox::callbackMouseMotion(const Vec2d& position)
	{
		Vec2d mousePos = getMouseOffsetToText();
		static_cast<EditableText&>(*text).computeCursorPosFromMousePos(mousePos, false);
		computeModelMatrixCursor();
		computeModelMatricesSelectionBoxes();
		cursorBlinkingTimer.reset();
		cursorIsVisible = true;
	}

	void GuiEditTextBox::update(const double& dt)
	{
		if (active) {
			if (cursorBlinkingTimer.tick(dt).first) {
				cursorIsVisible = !cursorIsVisible;
			}
		}
	}

	void GuiEditTextBox::onHandleDestruction(GuiHandle& guiHandle)
	{
		eventHandleTextWasEdited = nullptr;
	}

	void GuiEditTextBox::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		eventHandleTextWasEdited = static_cast<GuiEventHandle*>(&guiHandle);
	}

	Vec2d GuiEditTextBox::getMouseOffsetToText()
	{
		Vec2d DPI = Engine::getDPI();
		// Compute offset due to centering
		double textOffsetY;
		if (text->getAlignment() == StaticText::Alignment::CENTER) {
			// Align to the center of the text box
			// TODO: Why is this *1.0 necessary? Does not work if i remove it. Compiler bug?
			textOffsetY = -pointsToInches((text->getTop() * 1.0 + text->getBottom()) / 2.0) * DPI.y - static_cast<double>(size.y) / 2.0;
		}
		else {
			// Align to the top of the text box
			textOffsetY = -pointsToInches(text->getTop()) * DPI.y;
		}
		// Get mouse offset to upper left corner of element
		Vec2d mousePos = getMouseOffset(getGuiID());
		mousePos.y = - textOffsetY - mousePos.y;
		mousePos.x /= DPI.x;
		mousePos.y /= DPI.y;
		mousePos /= pointsToInches(1.0);
		return mousePos;
	}

	GuiEditTextBox::GuiEditTextBox(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const std::string& text, const Font& font, const double& fontSize, const double& cursorWidth, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine, const double& cursorBlinkTime)
		: GuiDynamicTextBox(position, size, resizeMode, 
			std::move(std::make_unique<EditableText>(text, font, fontSize, computeTextwidth(textBoxMode, size, singleLine), cursorWidth, parseMode, alignment)),
			textColor, backgroundColor, textBoxMode, singleLine),
		active(false), cursorIsVisible(false), cursorBlinkingTimer(cursorBlinkTime), modelMatrixCursor{}, modelMatricesSelectionBoxes{}, eventHandleTextWasEdited(nullptr) {}

	GuiEditTextBox::GuiEditTextBox(const std::string& text, const GuiStyle& style)
		: GuiEditTextBox(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text, style.defaultFont, style.fontSizeNormal,
			style.guiEditTextBoxCursorWidth, style.guiTextBoxTextColor, style.guiEditTextBoxBackgroundColor,
			style.guiTextBoxParseMode, style.guiTextBoxAlignment, style.guiTextBoxMode2,
			style.guiEditTextBoxSingleLine, style.guiEditTextBoxBlinkTime) {}

	GuiEditTextBox::GuiEditTextBox(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiEditTextBox(Vec2i(), style.guiTextBoxSize, style.guiTextBoxResizeMode, text, style.defaultFont, fontSize,
			style.guiEditTextBoxCursorWidth, style.guiTextBoxTextColor, style.guiEditTextBoxBackgroundColor,
			style.guiTextBoxParseMode, style.guiTextBoxAlignment, style.guiTextBoxMode2,
			style.guiEditTextBoxSingleLine, style.guiEditTextBoxBlinkTime) {}

	GuiEditTextBox::GuiEditTextBox(const GuiEditTextBox& other) noexcept
		: GuiDynamicTextBox(other), active(false), cursorIsVisible(false), 
		cursorBlinkingTimer(other.cursorBlinkingTimer), modelMatrixCursor(other.modelMatrixCursor),
		modelMatricesSelectionBoxes(other.modelMatricesSelectionBoxes), eventHandleTextWasEdited(nullptr) {}

	GuiEditTextBox::GuiEditTextBox(GuiEditTextBox&& other) noexcept
		: GuiDynamicTextBox(std::move(other)), active(false), cursorIsVisible(false),
		cursorBlinkingTimer(std::move(other.cursorBlinkingTimer)), modelMatrixCursor(other.modelMatrixCursor),
		modelMatricesSelectionBoxes(other.modelMatricesSelectionBoxes), eventHandleTextWasEdited(std::move(other.eventHandleTextWasEdited)) 
	{
		other.eventHandleTextWasEdited = nullptr;
		if (eventHandleTextWasEdited) notifyHandleOnGuiElementMove(&other, *eventHandleTextWasEdited);
	}

	GuiEditTextBox& GuiEditTextBox::operator=(const GuiEditTextBox& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		active = false;
		cursorIsVisible = false;
		cursorBlinkingTimer = other.cursorBlinkingTimer;
		modelMatrixCursor = other.modelMatrixCursor;
		modelMatricesSelectionBoxes = other.modelMatricesSelectionBoxes;
		eventHandleTextWasEdited = nullptr;
		return *this;
	}

	GuiEditTextBox& GuiEditTextBox::operator=(GuiEditTextBox&& other) noexcept
	{
		GuiDynamicTextBox::operator=(std::move(other));
		active = false;
		cursorIsVisible = false;
		cursorBlinkingTimer = std::move(other.cursorBlinkingTimer);
		eventHandleTextWasEdited = std::move(other.eventHandleTextWasEdited);
		modelMatrixCursor = other.modelMatrixCursor;
		modelMatricesSelectionBoxes = other.modelMatricesSelectionBoxes;
		other.eventHandleTextWasEdited = nullptr;
		if (eventHandleTextWasEdited) notifyHandleOnGuiElementMove(&other, *eventHandleTextWasEdited);
		return *this;
	}

	void GuiEditTextBox::setEventHandleTextWasEdited(GuiEventHandle& eventHandle)
	{
		if (!eventHandleTextWasEdited) {
			this->eventHandleTextWasEdited = &eventHandle;
			signUpHandle(eventHandle, 0);
		}
	}

	void GuiEditTextBox::setCursorWidth(const float& cursorWidth)
	{
		static_cast<EditableText&>(*text).setCursorWidth(cursorWidth);
	}

	void GuiEditTextBox::setCursorBlinkTime(const double& cursorBlinkTime)
	{
		cursorBlinkingTimer.setTimeStep(cursorBlinkTime);
	}

	void GuiEditTextBox::setText(const std::string& text)
	{
		GuiDynamicTextBox::setText(text);
		computeModelMatrixCursor();
	}

	void GuiEditTextBox::setFont(const Font& font)
	{
		GuiDynamicTextBox::setFont(font);
		computeModelMatrixCursor();
	}

	void GuiEditTextBox::setFontSize(const double& fontSize)
	{
		GuiDynamicTextBox::setFontSize(fontSize);
		computeModelMatrixCursor();
	}

	void GuiEditTextBox::setTextParseMode(const StaticText::ParseMode& parseMode)
	{
		GuiDynamicTextBox::setTextParseMode(parseMode);
		computeModelMatrixCursor();
	}

	void GuiEditTextBox::setAlignment(const StaticText::Alignment& alignment)
	{
		GuiDynamicTextBox::setAlignment(alignment);
		computeModelMatrixCursor();
	}

	void GuiEditTextBox::setTextBoxMode(const TextBoxMode& textBoxMode)
	{
		GuiDynamicTextBox::setTextBoxMode(textBoxMode);
		computeModelMatrixCursor();
	}

	void GuiEditTextBox::setSingleLine(const bool& singleLine)
	{
		GuiDynamicTextBox::setSingleLine(singleLine);
		computeModelMatrixCursor();
	}

}