#include "Gui/GuiElements/GuiTextBox.h"

#include "Gui/Text/TextMaterial.h"
#include "AssetManager/MaterialManager.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Math/Utility.h"
#include "Core/Engine.h"
#include "Gui/GuiStyle.h"
#include "Core/Keys.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::computeModelMatrices()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f), Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
		unsigned int DPI = Engine::getDPI().y;
		// Compute offset due to centering
		double textOffsetY;
		if (text->getAlignment() == StaticText::Alignment::CENTER) {
			// Align to the center of the text box
			// TODO: Why is this *1.0 necessary? Does not work if i remove it. Compiler bug?
			textOffsetY = -pointsToInches((text->getTop()*1.0 + text->getBottom()) / 2.0) * DPI - size.y / 2.0;
		}
		else {
			// Align to the top of the text box
			textOffsetY = -pointsToInches(text->getTop()) * DPI;
		}
		modelMatrixText = Mat4f::TranslateAndScale(Vec3f(position.x, - position.y + textOffsetY, 0), pointsToInches(text->getFontSize()) * DPI);
	}
	//--------------------------------------------------------------------------------------------------
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
		// Draw text
		material.bind();
		guiManager->setUniformViewAndProjectionMatrices(material.getShader());
		material.getShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixText);
		material.getShader().setUniform<float>("u_pxRange", text->getFont().getPixelRange());
		SnackerEngine::Renderer::draw(text->getModel(), material);
		// Draw children
		drawChildren(parentPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::onPositionChange()
	{
		GuiElement::onPositionChange();
		computeModelMatrices();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::onSizeChange()
	{
		text->setTextWidth(singleLine ? 0.0 : static_cast<float>(size.x) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f));
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE: 
		{
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			size.y = (text->getTop()*1.0 - text->getBottom()) * static_cast<double>(Engine::getDPI().y)*1.0 * pointsToInches(1.0);
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			size.x = (text->getRight() - text->getLeft()) * static_cast<double>(Engine::getDPI().y) * pointsToInches(1.0);
			break;
		}
		case TextBoxMode::SHRINK_TO_FIT:
		{
			size.x = (text->getRight() - text->getLeft()) * static_cast<double>(Engine::getDPI().y) * pointsToInches(1.0);
			size.y = (text->getTop() * 1.0 - text->getBottom()) * static_cast<double>(Engine::getDPI().y) * 1.0 * pointsToInches(1.0);
			break;
		}
		default:
			break;
		}
		GuiElement::onSizeChange();
		computeModelMatrices();
	}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::IsCollidingResult GuiDynamicTextBox::isColliding(const Vec2i& position)
	{
		return (position.x > this->position.x && position.x < this->position.x + size.x
			&& position.y > this->position.y && position.y < this->position.y + size.y) ?
			IsCollidingResult::WEAK_COLLIDING : IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::onRegister()
	{
		GuiElement::onRegister();
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, std::unique_ptr<DynamicText>&& text, const Font& font, const Color4f& textColor, const Color4f& backgroundColor, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiElement(position, size), text(std::move(text)), 
		material(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"), font, textColor, backgroundColor.alpha == 1.0 ? backgroundColor : Color4f(textColor.r, textColor.g, textColor.b, 0.0f)))),
		backgroundColor(backgroundColor),
		textColor(textColor), backgroundShader("shaders/gui/simpleColor.shader"), modelMatrixText{}, modelMatrixBackground{}, 
		textBoxMode(textBoxMode), singleLine(singleLine)
	{
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiDynamicTextBox(position, size, std::make_unique<DynamicText>(text, font, fontSize, singleLine ? 0.0 : static_cast<float>(size.x) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), parseMode, alignment),
			font, textColor, backgroundColor, textBoxMode, singleLine) {}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const std::string& text, const GuiStyle& style)
		: GuiDynamicTextBox(Vec2i(), style.guiTextBoxSize, text, style.defaultFont, style.fontSizeNormal, 
			style.guiTextBoxTextColor, style.guiTextBoxBackgroundColor,  style.guiTextBoxParseMode, 
			style.guiTextBoxAlignment, style.guiTextBoxMode, false) {}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiDynamicTextBox(Vec2i(), style.guiTextBoxSize, text, style.defaultFont, fontSize,
			style.guiTextBoxTextColor, style.guiTextBoxBackgroundColor, style.guiTextBoxParseMode,
			style.guiTextBoxAlignment, style.guiTextBoxMode, false) {}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const GuiDynamicTextBox& other) noexcept
		: GuiElement(other), text(std::make_unique<DynamicText>(*other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode), singleLine(other.singleLine) {}
	// TODO: This does not work for editable text!
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(GuiDynamicTextBox&& other) noexcept
		: GuiElement(other), text(std::move(other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode), singleLine(other.singleLine)
	{
		// set other to a well defined state
		other.text = nullptr;
		other.material = Material();
		other.backgroundShader = Shader();
	}
	//--------------------------------------------------------------------------------------------------
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
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox& GuiDynamicTextBox::operator=(GuiDynamicTextBox&& other) noexcept
	{
		GuiElement::operator=(other);
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
		other.material = Material();
		other.backgroundShader = Shader();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setText(const std::string& text)
	{
		this->text->setText(text);
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setFont(const Font& font)
	{
		text->setFont(font);
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setFontSize(const double& fontSize)
	{
		text->setFontSize(fontSize);
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setTextColor(const Color4f& textColor)
	{
		this->textColor = textColor;
		material = MaterialManager::createMaterial(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"), text->getFont(), textColor, backgroundColor.alpha == 1.0 ? backgroundColor : Color4f(0.0f, 0.0f, 0.0f, 0.0f))));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
		if (backgroundColor.alpha == 1.0f) material = MaterialManager::createMaterial(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"), text->getFont(), textColor, backgroundColor)));
		if (backgroundColor.alpha == 0.0f) material = MaterialManager::createMaterial(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"), text->getFont(), textColor, Color4f(textColor.r, textColor.g, textColor.b, 0.0f))));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setTextParseMode(const StaticText::ParseMode& parseMode)
	{
		text->setParseMode(parseMode);
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setAlignment(const StaticText::Alignment& alignment)
	{
		text->setAlignment(alignment);
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiDynamicTextBox::setTextBoxMode(const TextBoxMode& textBoxMode)
	{
		this->textBoxMode = textBoxMode;
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::computeModelMatrixCursor()
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
	//--------------------------------------------------------------------------------------------------
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
		// Draw text
		material.bind();
		guiManager->setUniformViewAndProjectionMatrices(material.getShader());
		material.getShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixText);
		material.getShader().setUniform<float>("u_pxRange", text->getFont().getPixelRange());
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
		drawChildren(parentPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::onPositionChange()
	{
		GuiDynamicTextBox::onPositionChange();
		computeModelMatrixCursor();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::onSizeChange()
	{
		GuiDynamicTextBox::onSizeChange();
		computeModelMatrixCursor();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::onRegister()
	{
		GuiDynamicTextBox::onRegister();
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (active && button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			if (isColliding(getParentMouseOffset()) == IsCollidingResult::NOT_COLLIDING) {
				active = false;
				cursorIsVisible = false;
				signOffEvent(CallbackType::CHARACTER_INPUT);
				signOffEvent(CallbackType::KEYBOARD);
				signOffEvent(CallbackType::UPDATE);
				signOffEvent(CallbackType::MOUSE_BUTTON);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
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
					if (eventHandleTextWasEdited) activate(*eventHandleTextWasEdited);
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
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::callbackCharacterInput(const unsigned int& codepoint)
	{
		if (active)
		{
			static_cast<EditableText&>(*text).inputAtCursor(codepoint);
			cursorBlinkingTimer.reset();
			cursorIsVisible = true;
			setSize(size);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
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
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::update(const double& dt)
	{
		if (active) {
			if (cursorBlinkingTimer.tick(dt).first) {
				cursorIsVisible = !cursorIsVisible;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::onHandleDestruction(GuiHandle& guiHandle)
	{
		eventHandleTextWasEdited = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		eventHandleTextWasEdited = static_cast<GuiEventHandle*>(&guiHandle);
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font, const double& fontSize, const double& cursorWidth, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine, const double& cursorBlinkTime)
		: GuiDynamicTextBox(position, size, std::move(std::make_unique<EditableText>(text, font, fontSize, static_cast<double>(size.x) / static_cast<double>(Engine::getDPI().y) / pointsToInches(1.0), cursorWidth, parseMode, alignment)),
			font, textColor, backgroundColor, textBoxMode, singleLine), active(false), cursorIsVisible(false), cursorBlinkingTimer(cursorBlinkTime), modelMatrixCursor{} {}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(const std::string& text, const GuiStyle& style)
		: GuiEditTextBox(Vec2i(), style.guiTextBoxSize, text, style.defaultFont, style.fontSizeNormal,
			style.guiEditTextBoxCursorWidth, style.guiTextBoxTextColor, style.guiTextBoxBackgroundColor, 
			style.guiTextBoxParseMode, style.guiTextBoxAlignment, style.guiTextBoxMode, 
			false, style.guiEditTextBoxBlinkTime) {}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiEditTextBox(Vec2i(), style.guiTextBoxSize, text, style.defaultFont, fontSize,
			style.guiEditTextBoxCursorWidth, style.guiTextBoxTextColor, style.guiTextBoxBackgroundColor,
			style.guiTextBoxParseMode, style.guiTextBoxAlignment, style.guiTextBoxMode, 
			false, style.guiEditTextBoxBlinkTime) {}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(const GuiEditTextBox& other) noexcept
		: GuiDynamicTextBox(other), active(false), cursorIsVisible(false), cursorBlinkingTimer(other.cursorBlinkingTimer), eventHandleTextWasEdited(nullptr) {}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(GuiEditTextBox&& other) noexcept
		: GuiDynamicTextBox(std::move(other)), active(false), cursorIsVisible(false), cursorBlinkingTimer(std::move(other.cursorBlinkingTimer)), eventHandleTextWasEdited(std::move(other.eventHandleTextWasEdited)) 
	{
		other.eventHandleTextWasEdited = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox& GuiEditTextBox::operator=(const GuiEditTextBox& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		active = false;
		cursorIsVisible = false;
		cursorBlinkingTimer = other.cursorBlinkingTimer;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox& GuiEditTextBox::operator=(GuiEditTextBox&& other) noexcept
	{
		GuiDynamicTextBox::operator=(std::move(other));
		active = false;
		cursorIsVisible = false;
		cursorBlinkingTimer = std::move(other.cursorBlinkingTimer);
		eventHandleTextWasEdited = std::move(other.eventHandleTextWasEdited);
		other.eventHandleTextWasEdited = nullptr;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiEditTextBox::isActive() const
	{
		return active;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEditTextBox::setEventHandleTextWasEdited(GuiEventHandle& eventHandle)
	{
		if (!eventHandleTextWasEdited) {
			eventHandleTextWasEdited = &eventHandle;
			signUpHandle(eventHandle, 0);
		}
	}
	//--------------------------------------------------------------------------------------------------
}
