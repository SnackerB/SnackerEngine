#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{

	void GuiPanel::computeModelMatrix()
	{
		const Vec2i& position = getPosition();
		const Vec2i& size = getSize();
		modelMatrix = Mat4f::TranslateAndScale(
			Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f), 
			Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
	}

	void GuiPanel::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		shader.bind();
		guiManager->setUniformViewAndProjectionMatrices(shader);
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		shader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrix);
		shader.setUniform<Color3f>("u_color", backgroundColor);
		Renderer::draw(guiManager->getModelSquare());
		pushClippingBox(parentPosition);
		GuiElement::draw(parentPosition);
		popClippingBox();
	}

	void GuiPanel::onPositionChange()
	{
		GuiElement::onPositionChange();
		computeModelMatrix();
	}

	void GuiPanel::onSizeChange()
	{
		GuiElement::onSizeChange();
		computeModelMatrix();
	}

	GuiPanel::IsCollidingResult GuiPanel::isColliding(const Vec2i& position)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (position.x > myPosition.x && position.x < myPosition.x + mySize.x
			&& position.y > myPosition.y && position.y < myPosition.y + mySize.y) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}

	const Shader& GuiPanel::getPanelShader() const
	{
		return shader;
	}

	GuiPanel::GuiPanel(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const Color3f& backgroundColor)
		: GuiElement(position, size, resizeMode), backgroundColor(backgroundColor), modelMatrix{}, shader("shaders/gui/simpleColor.shader")
	{
		computeModelMatrix();
	}

	void GuiPanel::setBackgroundColor(const Color3f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}

	Color3f GuiPanel::getBackgroundColor() const
	{
		return backgroundColor;
	}

	GuiPanel::GuiPanel(const GuiPanel& other) noexcept
		: GuiElement(other), backgroundColor(other.backgroundColor), modelMatrix(other.modelMatrix), shader(other.shader) {}

	GuiPanel& GuiPanel::operator=(const GuiPanel& other) noexcept
	{
		GuiElement::operator=(other);
		backgroundColor = other.backgroundColor;
		modelMatrix = other.modelMatrix;
		shader = other.shader;
		return *this;
	}

	GuiPanel::GuiPanel(GuiPanel&& other) noexcept
		: GuiElement(std::move(other)), backgroundColor(other.backgroundColor), modelMatrix(other.modelMatrix), shader(other.shader) {}

	GuiPanel& GuiPanel::operator=(GuiPanel&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		backgroundColor = other.backgroundColor;
		modelMatrix = other.modelMatrix;
		shader = other.shader;
		return *this;
	}

}