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
		scaleFactor = 1.0;
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_SCALE_TEXT_UP:
		{
			double textWidth = pointsToInches((text->getRight() - text->getLeft())) * DPI;
			if (textWidth > size.x) break;
			double textHeight = pointsToInches((text->getTop() - text->getBottom())) * DPI;
			if (textHeight > size.y) break;
			scaleFactor = std::min(size.x / textWidth, size.y / textHeight);
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_RECOMPUTE_SCALE_DOWN:
		{
			double textWidth = pointsToInches((text->getRight() - text->getLeft())) * DPI;
			double textHeight = pointsToInches((text->getTop() - text->getBottom())) * DPI;
			if (textWidth < size.x && textHeight < size.y) break;
			scaleFactor = std::min(size.x / textWidth, size.y / textHeight);
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_SCALE_TEXT:
		{
			double textWidth = pointsToInches((text->getRight() - text->getLeft())) * DPI;
			double textHeight = pointsToInches((text->getTop() - text->getBottom())) * DPI;
			scaleFactor = std::min(size.x / textWidth, size.y / textHeight);
			break;
		}
		default:
		{
			break;
		}
		}
		Vec2f textPosition = computeTextPosition();
		modelMatrixText = Mat4f::TranslateAndScale(Vec3f(textPosition.x, textPosition.y, 0), Vec3f(pointsToInches<float>(static_cast<float>(text->getFontSize())) * static_cast<float>(DPI) * scaleFactor));
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
		recomputeText();
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

	void GuiDynamicTextBox::recomputeText()
	{
		switch (textBoxMode)
		{
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE:
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_SCALE_TEXT_UP:
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_SCALE_TEXT_DOWN:
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_SCALE_TEXT:
		{
			// Just compute the text with the size.x as textWidth:
			if (singleLine) text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), false);
			text->recompute();
			computeModelMatrices();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_RECOMPUTE_SCALE_DOWN:
		{
			if (singleLine) text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), false);
			text->setFontSize(fontSize, false);
			unsigned int DPI = Engine::getDPI().y;
			double maxFontSize = 0.0;
			double minFontSize = 0.0;
			//infoLogger << "recomputing text, starting with font size " << text->getFontSize() << LOGGER::ENDL;
			for (unsigned int i = 0; i < recomputeTries; ++i)
			{
				text->recompute();
				double textHeight = pointsToInches((text->getTop() - text->getBottom())) * DPI;
				// Check text width only in single line parse mode!
				if (textHeight < size.y && (getParseMode() != StaticText::ParseMode::SINGLE_LINE || pointsToInches((text->getRight() - text->getLeft())) * DPI < size.x)) {
					minFontSize = text->getFontSize();
					if (minFontSize == fontSize) break;
					if (maxFontSize == 0) {
						text->setFontSize(std::min(minFontSize * 2.0, fontSize), false);
					}
					else {
						text->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
				else {
					maxFontSize = text->getFontSize();
					if (minFontSize == 0) {
						text->setFontSize(maxFontSize / 2.0, false);
					}
					else {
						text->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
				//infoLogger << "font size:" << text->getFontSize() << ", minFontSize: " << minFontSize << LOGGER::ENDL;
			}
			text->setFontSize(minFontSize, false);
			text->recompute();
			computeModelMatrices();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE_RECOMPUTE_SCALE:
		{
			if (singleLine) text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), false);
			text->setFontSize(fontSize, false);
			unsigned int DPI = Engine::getDPI().y;
			double maxFontSize = 0.0;
			double minFontSize = 0.0;
			//infoLogger << "recomputing text, starting with font size " << text->getFontSize() << LOGGER::ENDL;
			for (unsigned int i = 0; i < recomputeTries; ++i)
			{
				text->recompute();
				double textHeight = pointsToInches((text->getTop() - text->getBottom())) * DPI;
				// Check text width only in single line parse mode!
				if (textHeight < size.y && (getParseMode() != StaticText::ParseMode::SINGLE_LINE || pointsToInches((text->getRight() - text->getLeft())) * DPI < size.x)) {
					minFontSize = text->getFontSize();
					if (maxFontSize == 0) {
						text->setFontSize(minFontSize * 2.0, false);
					}
					else {
						text->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
				else {
					maxFontSize = text->getFontSize();
					if (minFontSize == 0) {
						text->setFontSize(maxFontSize / 2.0, false);
					}
					else {
						text->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
				//infoLogger << "font size:" << text->getFontSize() << ", minFontSize: " << minFontSize << LOGGER::ENDL;
			}
			text->setFontSize(minFontSize, false);
			text->recompute();
			computeModelMatrices();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT:
		{
			// Just compute the text with the size.x as textWidth:
			if (singleLine) text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(getWidth() / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), false);
			text->recompute();
			size.y = static_cast<int>(std::ceil((text->getTop() - text->getBottom()) * Engine::getDPI().y * pointsToInches(1.0)));
			computeModelMatrices();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_WIDTH_TO_FIT:
		{
			// compute the text with zero textWidth
			text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(0.0, false);
			text->recompute();
			size.x = static_cast<int>(std::ceil((text->getRight() - text->getLeft()) * Engine::getDPI().x * pointsToInches(1.0)));
			computeModelMatrices();
			break;
		}
		case SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_TO_FIT:
		{
			// compute the text with zero textWidth
			text->setParseMode(StaticText::ParseMode::SINGLE_LINE, false);
			text->setTextWidth(0.0, false);
			text->recompute();
			size.y = static_cast<int>(std::ceil((text->getTop() - text->getBottom()) * Engine::getDPI().y * pointsToInches(1.0)));
			size.x = static_cast<int>(std::ceil((text->getRight() - text->getLeft()) * Engine::getDPI().x * pointsToInches(1.0)));
			computeModelMatrices();
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
		Vec2f textOffset = Vec2f(0.0f, 0.0f);
		switch (text->getAlignment())
		{
		case StaticText::Alignment::LEFT:
		{
			// Align to the top of the text box
			textOffset.y = -pointsToInches(text->getTop()) * DPI * scaleFactor;
			// Is already aligned to the left!
			break;
		}
		case StaticText::Alignment::CENTER:
		{
			// Align to the center of the text box
			textOffset.y = -pointsToInches((text->getTop() + text->getBottom()) / 2.0) * DPI * scaleFactor - size.y / 2.0;
			textOffset.x = static_cast<float>(size.x) / 2.0f - pointsToInches((text->getRight() - text->getLeft()) / 2.0) * DPI * scaleFactor;
			break;
		}
		case StaticText::Alignment::RIGHT:
		{
			// Align to the top of the text box
			textOffset.y = -pointsToInches(text->getTop()) * DPI * scaleFactor;
			// Align to the right of the text box
			textOffset.x = static_cast<float>(size.x) - pointsToInches((text->getRight() - text->getLeft())) * DPI * scaleFactor;
			break;
		}
		default:
			break;
		}
		Vec2f position = Vec2f(static_cast<float>(getPositionX() + textOffset.x), -static_cast<float>(getPositionY()) + static_cast<float>(textOffset.y));
		return position;
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
		modelMatrixText{}, modelMatrixBackground{}, textBoxMode(textBoxMode), singleLine(singleLine), scaleFactor(1.0),
		fontSize(this->text->getFontSize()), recomputeTries(10)
	{
		recomputeText();
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
		textBoxMode(other.textBoxMode), singleLine(other.singleLine), scaleFactor(other.scaleFactor),
		fontSize(other.fontSize), recomputeTries(other.recomputeTries) {}

	GuiDynamicTextBox::GuiDynamicTextBox(GuiDynamicTextBox&& other) noexcept
		: GuiElement(std::move(other)), text(std::move(other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode), singleLine(other.singleLine), scaleFactor(other.scaleFactor),
		fontSize(other.fontSize), recomputeTries(other.recomputeTries)
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
		scaleFactor = other.scaleFactor;
		fontSize = other.fontSize;
		recomputeTries = other.recomputeTries;
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
		scaleFactor = other.scaleFactor;
		fontSize = other.fontSize;
		recomputeTries = other.recomputeTries;
		// set other to a well defined state
		other.text = nullptr;
		return *this;
	}

	void GuiDynamicTextBox::setText(const std::string& text)
	{
		this->text->setText(text, false);
		recomputeText();
		computeModelMatrices();
	}

	void GuiDynamicTextBox::setFont(const Font& font)
	{
		if (this->text->getFont().fontID != font.fontID) {
			this->text->setFont(font, false);
			material = constructTextMaterial(text->getFont(), textColor, backgroundColor);
			recomputeText();
			computeModelMatrices();
		}
	}

	void GuiDynamicTextBox::setFontSize(const double& fontSize)
	{
		if (this->fontSize != fontSize) {
			this->text->setFontSize(fontSize, false);
			this->fontSize = fontSize;
			recomputeText();
			computeModelMatrices();
		}
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
		if (text->getParseMode() != parseMode) {
			text->setParseMode(parseMode, false);
			recomputeText();
			computeModelMatrices();
		}
	}

	void GuiDynamicTextBox::setAlignment(const StaticText::Alignment& alignment)
	{
		if (this->text->getAlignment() != alignment) {
			this->text->setAlignment(alignment, false);
			recomputeText();
			computeModelMatrices();
		}
	}

	void GuiDynamicTextBox::setTextBoxMode(const TextBoxMode& textBoxMode)
	{
		if (this->textBoxMode != textBoxMode) {
			this->textBoxMode = textBoxMode;
			recomputeText();
			computeModelMatrices();
		}
	}

	void GuiDynamicTextBox::setSingleLine(const bool& singleLine)
	{
		if (this->singleLine != singleLine) {
			this->singleLine = singleLine;
			recomputeText();
			computeModelMatrices();
		}
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
		Vec2f cursorOffset = static_cast<EditableText&>(*text).getCursorPos() * scaleFactor;
		cursorOffset.x = pointsToInches(cursorOffset.x);
		cursorOffset.y = pointsToInches(cursorOffset.y);
		cursorOffset *= static_cast<float>(DPI);
		Vec2f cursorSize = static_cast<EditableText&>(*text).getCursorSize() * scaleFactor;
		cursorSize.x = pointsToInches(cursorSize.x);
		cursorSize.y = pointsToInches(cursorSize.y);
		cursorSize *= static_cast<float>(DPI);
		Vec2f textOffset = computeTextPosition();
		modelMatrixCursor = Mat4f::TranslateAndScale(Vec3f(textOffset.x + cursorOffset.x + cursorSize.x, textOffset.y + cursorOffset.y, 0.0f), cursorSize);
	}

	void GuiEditTextBox::computeModelMatricesSelectionBoxes()
	{
		modelMatricesSelectionBoxes.clear();
		auto result = static_cast<EditableText&>(*text).getSelectionBoxes();
		if (result.empty()) return;
		Vec2f textPosition = computeTextPosition();
		unsigned int DPI = Engine::getDPI().y;
		for (const auto& selectionBox : result) {
			modelMatricesSelectionBoxes.push_back(Mat4f::TranslateAndScale(textPosition + pointsToInches(selectionBox.position * scaleFactor) * static_cast<float>(DPI), pointsToInches(selectionBox.size * scaleFactor) * static_cast<float>(DPI)));
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
				}
				else {
					static_cast<EditableText&>(*text).deleteCharacterBeforeCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				recomputeText();
				computeModelMatrixCursor();
			}
			else if (key == KEY_DELETE) {
				if (mods & MOD_CONTROL) {
					static_cast<EditableText&>(*text).deleteWordAfterCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				else {
					static_cast<EditableText&>(*text).deleteCharacterAfterCursor();
					cursorBlinkingTimer.reset();
					cursorIsVisible = true;
				}
				recomputeText();
				computeModelMatrixCursor();
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
			recomputeText();
			computeModelMatrixCursor();
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
			static_cast<EditableText&>(*text).computeCursorPosFromMousePos(mousePos / scaleFactor);
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
		static_cast<EditableText&>(*text).computeCursorPosFromMousePos(mousePos / scaleFactor, false);
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
			textOffsetY = -pointsToInches((text->getTop() + text->getBottom()) / 2.0) * DPI.y * scaleFactor - static_cast<double>(size.y) / 2.0;
		}
		else {
			// Align to the top of the text box
			textOffsetY = -pointsToInches(text->getTop()) * scaleFactor * DPI.y;
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