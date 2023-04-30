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
			Vec3f(0.0f, static_cast<float>(-size.y), 0.0f), 
			Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
	}

	void GuiPanel::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (backgroundColor.alpha != 0.0f)
		{
			shader.bind();
			guiManager->setUniformViewAndProjectionMatrices(shader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			shader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrix);
			shader.setUniform<Color4f>("u_color", backgroundColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		pushClippingBox(worldPosition);
		GuiElement::draw(worldPosition);
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

	GuiPanel::IsCollidingResult GuiPanel::isColliding(const Vec2i& offset)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (offset.x > 0 && offset.x < mySize.x
			&& offset.y > 0 && offset.y < mySize.y) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}

	const Shader& GuiPanel::getPanelShader() const
	{
		return shader;
	}

	GuiPanel::GuiPanel(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const Color4f& backgroundColor)
		: GuiElement(position, size, resizeMode), backgroundColor(backgroundColor), modelMatrix{}, shader("shaders/gui/simpleAlphaColor.shader")
	{
		computeModelMatrix();
	}

	void GuiPanel::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}

	Color4f GuiPanel::getBackgroundColor() const
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