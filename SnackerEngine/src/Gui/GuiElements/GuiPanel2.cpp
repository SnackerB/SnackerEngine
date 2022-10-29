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
	}

	void GuiPanel2::onSizeChange()
	{
	}

	GuiPanel2::IsCollidingResult GuiPanel2::isColliding(const Vec2i& position)
	{
		return IsCollidingResult();
	}

	const Shader& GuiPanel2::getPanelShader() const
	{
		// TODO: insert return statement here
	}

	GuiPanel2::GuiPanel2(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const Color3f& backgroundColor)
	{
	}

	void GuiPanel2::setBackgroundColor(const Color3f& backgroundColor)
	{
	}

	Color3f GuiPanel2::getBackgroundColor() const
	{
		return Color3f();
	}

	GuiPanel2::GuiPanel2(const GuiPanel2& other) noexcept
	{
	}

	GuiPanel2& GuiPanel2::operator=(const GuiPanel2& other) noexcept
	{
		// TODO: insert return statement here
	}

	GuiPanel2::GuiPanel2(GuiPanel2&& other) noexcept
	{
	}

	GuiPanel2& GuiPanel2::operator=(GuiPanel2&& other) noexcept
	{
		// TODO: insert return statement here
	}

}