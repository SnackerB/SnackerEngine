#include "Gui/GuiElements/GuiPanel2.h"
#include "Gui/GuiManager2.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{

	void GuiPanel2::computeModelMatrix()
	{
		const Vec2i& position = getPosition();
		const Vec2i& size = getSize();
		modelMatrix = Mat4f::TranslateAndScale(
			Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f), 
			Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
	}

	void GuiPanel2::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		shader.bind();
		guiManager->setUniformViewAndProjectionMatrices(shader);
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		shader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrix);
		shader.setUniform<Color3f>("u_color", backgroundColor);
		Renderer::draw(guiManager->getModelSquare());
		GuiElement2::draw(parentPosition);
	}

	void GuiPanel2::onPositionChange()
	{
		GuiElement2::onPositionChange();
		computeModelMatrix();
	}

	void GuiPanel2::onSizeChange()
	{
		GuiElement2::onSizeChange();
		computeModelMatrix();
	}

	GuiPanel2::IsCollidingResult GuiPanel2::isColliding(const Vec2i& position)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (position.x > myPosition.x && position.x < myPosition.x + mySize.x
			&& position.y > myPosition.y && position.y < myPosition.y + mySize.y) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}

	const Shader& GuiPanel2::getPanelShader() const
	{
		return shader;
	}

	GuiPanel2::GuiPanel2(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const Color3f& backgroundColor)
		: GuiElement2(position, size, resizeMode), backgroundColor(backgroundColor), modelMatrix{}, shader("shaders/gui/simpleColor.shader")
	{
		computeModelMatrix();
	}

	void GuiPanel2::setBackgroundColor(const Color3f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}

	Color3f GuiPanel2::getBackgroundColor() const
	{
		return backgroundColor;
	}

	GuiPanel2::GuiPanel2(const GuiPanel2& other) noexcept
		: GuiElement2(other), backgroundColor(other.backgroundColor), modelMatrix(other.modelMatrix), shader(other.shader) {}

	GuiPanel2& GuiPanel2::operator=(const GuiPanel2& other) noexcept
	{
		GuiElement2::operator=(other);
		backgroundColor = other.backgroundColor;
		modelMatrix = other.modelMatrix;
		shader = other.shader;
		return *this;
	}

	GuiPanel2::GuiPanel2(GuiPanel2&& other) noexcept
		: GuiElement2(std::move(other)), backgroundColor(other.backgroundColor), modelMatrix(other.modelMatrix), shader(other.shader) {}

	GuiPanel2& GuiPanel2::operator=(GuiPanel2&& other) noexcept
	{
		GuiElement2::operator=(std::move(other));
		backgroundColor = other.backgroundColor;
		modelMatrix = other.modelMatrix;
		shader = other.shader;
		return *this;
	}

}