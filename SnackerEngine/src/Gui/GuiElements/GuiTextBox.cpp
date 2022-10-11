#include "Gui/GuiElements/GuiTextBox.h"

#include "Gui/Text/TextMaterial.h"
#include "AssetManager/MaterialManager.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Math/Utility.h"
#include "Core/Engine.h"
#include "Gui/GuiStyle.h"

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
		text->setTextWidth(static_cast<float>(size.x) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f));
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
	GuiDynamicTextBox::GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode)
		: GuiElement(position, size), text(std::make_unique<DynamicText>(text, font, fontSize, static_cast<float>(size.x) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), parseMode, alignment)), 
		material(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"), font, textColor, backgroundColor.alpha == 1.0 ? backgroundColor : Color4f(textColor.r, textColor.g, textColor.b, 0.0f)))),
		backgroundColor(backgroundColor), textColor(textColor), backgroundShader("shaders/gui/simpleColor.shader"), modelMatrixText{}, modelMatrixBackground{}, textBoxMode(textBoxMode)
	{
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const std::string& text, const GuiStyle& style)
		: GuiDynamicTextBox(Vec2i(), style.guiTextBoxSize, text, style.defaultFont, style.fontSizeNormal, 
			style.guiTextBoxTextColor, style.guiTextBoxBackgroundColor,  style.guiTextBoxParseMode, 
			style.guiTextBoxAlignment, style.guiTextBoxMode) {}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiDynamicTextBox(Vec2i(), style.guiTextBoxSize, text, style.defaultFont, fontSize,
			style.guiTextBoxTextColor, style.guiTextBoxBackgroundColor, style.guiTextBoxParseMode,
			style.guiTextBoxAlignment, style.guiTextBoxMode) {}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(const GuiDynamicTextBox& other) noexcept
		: GuiElement(other), text(std::make_unique<DynamicText>(*other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode) {}
	//--------------------------------------------------------------------------------------------------
	GuiDynamicTextBox::GuiDynamicTextBox(GuiDynamicTextBox&& other) noexcept
		: GuiElement(other), text(std::move(other.text)), material(other.material),
		backgroundColor(other.backgroundColor), textColor(other.textColor), backgroundShader(other.backgroundShader),
		modelMatrixText(other.modelMatrixText), modelMatrixBackground(other.modelMatrixBackground),
		textBoxMode(other.textBoxMode)
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
	/*
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(const std::string& text, const GuiStyle& style)
		: GuiDynamicTextBox(text, style) {}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(const std::string& text, const double& fontSize, const GuiStyle& style)
		: GuiDynamicTextBox(text, fontSize, style) {}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(const GuiDynamicTextBox& other) noexcept
	{

	}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox::GuiEditTextBox(GuiDynamicTextBox&& other) noexcept
	{

	}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox& GuiEditTextBox::operator=(const GuiEditTextBox& other) noexcept
	{

	}
	//--------------------------------------------------------------------------------------------------
	GuiEditTextBox& GuiEditTextBox::operator=(GuiEditTextBox&& other) noexcept
	{

	}
	//--------------------------------------------------------------------------------------------------
	*/
}
