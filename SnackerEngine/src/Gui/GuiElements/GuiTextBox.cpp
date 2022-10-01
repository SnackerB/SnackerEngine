#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/Text/TextMaterial.h"
#include "AssetManager/MaterialManager.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Math/Utility.h"
#include "Core/Engine.h"

namespace SnackerEngine
{

	void GuiStaticTextBox::computeModelMatrices()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f), Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
		unsigned int DPI = Engine::getDPI().y;
		// Compute offset due to centering
		double textOffsetY;
		if (text.getAlignment() == StaticText2::Alignment::CENTER) {
			// Align to the center of the text box
			// TODO: Why is this *1.0 necessary? Does not work if i remove it. Compiler bug?
			textOffsetY = -pointsToInches((text.getTop()*1.0 + text.getBottom()) / 2.0) * DPI - size.y / 2.0;
		}
		else {
			// Align to the top of the text box
			textOffsetY = -pointsToInches(text.getTop()) * DPI;
		}
		modelMatrixText = Mat4f::TranslateAndScale(Vec3f(position.x, - position.y + textOffsetY, 0), pointsToInches(text.getFontSize()) * DPI);
	}

	void GuiStaticTextBox::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		// Draw background
		backgroundShader.bind();
		guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
		backgroundShader.setUniform<Color3f>("u_color", Color3f(backgroundColor.r, backgroundColor.g, backgroundColor.b)); // TODO: Transparent background
		Renderer::draw(guiManager->getModelSquare());
		// Draw text
		material.bind();
		guiManager->setUniformViewAndProjectionMatrices(material.getShader());
		material.getShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixText);
		material.getShader().setUniform<float>("u_pxRange", text.getFont().getPixelRange());
		SnackerEngine::Renderer::draw(text.getModel(), material);
		// Draw children
		drawChildren(parentPosition);
	}

	void GuiStaticTextBox::onPositionChange()
	{
		GuiElement::onPositionChange();
		computeModelMatrices();
	}

	void GuiStaticTextBox::onSizeChange()
	{
		GuiElement::onSizeChange();
		text.setTextWidth(static_cast<float>(size.x) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f));
		computeModelMatrices();
	}

	GuiStaticTextBox::IsCollidingResult GuiStaticTextBox::isColliding(const Vec2i& position)
	{
		return (position.x > this->position.x && position.x < this->position.x + size.x
			&& position.y > this->position.y && position.y < this->position.y + size.y) ?
			IsCollidingResult::WEAK_COLLIDING : IsCollidingResult::NOT_COLLIDING;
	}

	void GuiStaticTextBox::onRegister()
	{
		GuiElement::onRegister();
		computeModelMatrices();
	}

	GuiStaticTextBox::GuiStaticTextBox(const Vec2i& position, const Vec2i& size, const std::u8string& text, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText2::ParseMode& parseMode, const StaticText2::Alignment& alignment)
		: GuiElement(position, size), text(text, font, fontSize, static_cast<float>(size.x) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), parseMode, alignment), material(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"), font, textColor, Color4f(0.0f, 0.0f, 0.0f, 0.0f)))),
		backgroundColor(backgroundColor), backgroundShader("shaders/gui/simpleColor.shader"), modelMatrixText{}, modelMatrixBackground{}
	{
		computeModelMatrices();
	}

}
