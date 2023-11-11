#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	Color4f GuiPanel::defaultBackgroundColor = Color4f(0.0f, 1.0f);
	Shader GuiPanel::defaultBackgroundShader{};
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const Color4f& backgroundColor)
		: GuiElement(position, size, resizeMode), backgroundColor(backgroundColor), modelMatrix{},
		shader("shaders/gui/simpleAlphaColor.shader")
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiElement(json, data, parameterNames)
	{
		parseJsonOrReadFromData(backgroundColor, "backgroundColor", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(const GuiPanel& other) noexcept
		: GuiElement(other), backgroundColor(other.backgroundColor), modelMatrix(other.modelMatrix), shader(other.shader)
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel& GuiPanel::operator=(const GuiPanel& other) noexcept
	{
		GuiElement::operator=(other);
		backgroundColor = other.backgroundColor;
		modelMatrix = other.modelMatrix;
		shader = other.shader;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(GuiPanel&& other) noexcept
		: GuiElement(std::move(other)), backgroundColor(other.backgroundColor), modelMatrix(other.modelMatrix), shader(other.shader)
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel& GuiPanel::operator=(GuiPanel&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		backgroundColor = other.backgroundColor;
		modelMatrix = other.modelMatrix;
		shader = other.shader;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}
	//--------------------------------------------------------------------------------------------------
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
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::onRegister()
	{
		computeModelMatrix();
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
	GuiElement::IsCollidingResult GuiPanel::isColliding(const Vec2i& offset) const
	{
		IsCollidingResult result = GuiElement::isColliding(offset);
		if (result == IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT && getBackgroundColor().alpha == 0) return IsCollidingResult::COLLIDE_CHILD;
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::computeModelMatrix()
	{
		const Vec2i& position = getPosition();
		const Vec2i& size = getSize();
		modelMatrix = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-size.y), 0.0f),
			Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
}