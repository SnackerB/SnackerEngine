#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::computeModelMatrix()
	{
		modelMatrix = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(position.x), static_cast<float>(- position.y - size.y), 0.0f), Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		shader.bind();
		guiManager->setUniformViewAndProjectionMatrices(shader);
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		shader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrix);
		shader.setUniform<Color3f>("u_color", backgroundColor);
		Renderer::draw(guiManager->getModelSquare());
		drawChildren(parentPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::onPositionChange()
	{
		GuiElement::onPositionChange();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::onSizeChange()
	{
		GuiElement::onSizeChange();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::IsCollidingResult GuiPanel::isColliding(const Vec2i& position)
	{
		return (position.x > this->position.x && position.x < this->position.x + size.x
			&& position.y > this->position.y && position.y < this->position.y + size.y) ? 
			IsCollidingResult::WEAK_COLLIDING : IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	const Shader& GuiPanel::getPanelShader() const
	{
		return shader;
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(const Vec2i& position, const Vec2i& size, const Color3f& backgroundColor)
		: GuiElement(position, size), backgroundColor(backgroundColor), modelMatrix{}, shader("shaders/gui/simpleColor.shader")
	{
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setBackgroundColor(const Color3f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}
	//--------------------------------------------------------------------------------------------------
	Color3f GuiPanel::getBackgroundColor() const
	{
		return backgroundColor;
	}
	//--------------------------------------------------------------------------------------------------
}