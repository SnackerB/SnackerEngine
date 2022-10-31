#include "Gui/GuiElements/GuiWindow2.h"
#include "Gui/GuiManager2.h"
#include "Graphics/Renderer.h"
#include "core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::computeResizeButtonModelMatrix()
	{
		resizeButtonModelMatrix = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(getPositionX() + getWidth() - resizeButtonSize), static_cast<float>(-getPositionY() - getHeight()), 0.0f), Vec3f(static_cast<float>(resizeButtonSize), static_cast<float>(resizeButtonSize), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::draw(const Vec2i& parentPosition)
	{
		/// Draw the panel first. Also draws all children.
		GuiPanel2::draw(parentPosition);
		if (!guiManager) return;
		getPanelShader().bind();
		guiManager->setUniformViewAndProjectionMatrices(getPanelShader());
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		getPanelShader().setUniform<Mat4f>("u_model", resizeButtonModelMatrix * translationMatrix);
		getPanelShader().setUniform<Color3f>("u_color", resizeButtonColor);
		Renderer::draw(guiManager->getModelTriangle());
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::onPositionChange()
	{
		GuiPanel2::onPositionChange();
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::onSizeChange()
	{
		GuiPanel2::onSizeChange();
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiWindow2::isCollidingWithResizeButton(const Vec2i& offset)
	{
		return offset.x >= getWidth() - resizeButtonSize && offset.y >= getHeight() - offset.x + getWidth() - resizeButtonSize;
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2::IsCollidingResult GuiWindow2::isColliding(const Vec2i& position)
	{
		if (GuiPanel2::isColliding(position) != IsCollidingResult::NOT_COLLIDING) {
			// Check if the resize button was hit
			if (isCollidingWithResizeButton(position - getPosition())) {
				return IsCollidingResult::COLLIDE_STRONG;
			}
			else {
				return IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT;
			}
		}
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::onRegister()
	{
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			isMoving = false;
			isResizing = false;
			signOffEvent(CallbackType::MOUSE_MOTION);
			signOffEvent(CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			// Figure out if the user clicked on the resize button or on the window
			mouseOffset = getMouseOffset(getGuiID());
			if (isCollidingWithResizeButton(mouseOffset)) {
				isResizing = true;
				// When were resizing the mouse offset is relative to the top left corner of the resize button
				mouseOffset = mouseOffset - getSize() + Vec2f(resizeButtonSize);
			}
			else {
				isMoving = true;
			}
			signUpEvent(CallbackType::MOUSE_MOTION);
			signUpEvent(CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::callbackMouseMotion(const Vec2d& position)
	{
		if (isMoving) {
			Vec2f newPosition = getMouseOffset(getParentID()) - mouseOffset;
			setPosition(newPosition);
		}
		else if (isResizing) {
			Vec2f newSize = getMouseOffset(getGuiID()) + Vec2f(resizeButtonSize, resizeButtonSize) - mouseOffset;
			// Clip size
			if (newSize.x < resizeButtonSize) newSize.x = resizeButtonSize;
			if (newSize.y < resizeButtonSize) newSize.y = resizeButtonSize;
			setSize(newSize);
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2::GuiWindow2(const Vec2i& position, const Vec2i& size, const Color3f& backgroundColor, const double& resizeButtonSize, const Color3f& resizeButtonColor)
		: GuiPanel2(position, size, ResizeMode::DO_NOT_RESIZE, backgroundColor), resizeButtonSize(resizeButtonSize), resizeButtonColor(resizeButtonColor),
		resizeButtonModelMatrix{}, isMoving(false), isResizing(false), mouseOffset(Vec2f())
	{
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2::GuiWindow2(const GuiStyle& style)
		: GuiWindow2(Vec2i(), style.guiWindowSize, style.guiWindowBackgroundColor, style.guiWindowResizeButtonSize, style.guiWindowResizeButtonColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2::GuiWindow2(const GuiWindow2& other) noexcept
		: GuiPanel2(other), resizeButtonSize(other.resizeButtonSize), resizeButtonColor(other.resizeButtonColor),
		resizeButtonModelMatrix(other.resizeButtonModelMatrix), isMoving(false), isResizing(false), mouseOffset(Vec2f()) {}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2& GuiWindow2::operator=(const GuiWindow2& other) noexcept
	{
		GuiPanel2::operator=(other);
		resizeButtonSize = other.resizeButtonSize;
		resizeButtonColor = other.resizeButtonColor;
		resizeButtonModelMatrix = other.resizeButtonModelMatrix;
		isMoving = false;
		isResizing = false;
		mouseOffset = Vec2f();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2::GuiWindow2(GuiWindow2&& other) noexcept
		: GuiPanel2(std::move(other)), resizeButtonSize(other.resizeButtonSize), resizeButtonColor(other.resizeButtonColor),
		resizeButtonModelMatrix(other.resizeButtonModelMatrix), isMoving(false), isResizing(false), mouseOffset(Vec2f()) {}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2& GuiWindow2::operator=(GuiWindow2&& other) noexcept
	{
		GuiPanel2::operator=(std::move(other));
		resizeButtonSize = other.resizeButtonSize;
		resizeButtonColor = other.resizeButtonColor;
		resizeButtonModelMatrix = other.resizeButtonModelMatrix;
		isMoving = false;
		isResizing = false;
		mouseOffset = Vec2f();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow2::~GuiWindow2()
	{
	}
	//--------------------------------------------------------------------------------------------------
	const double& GuiWindow2::getResizeButtonSize() const
	{
		return resizeButtonSize;
	}
	//--------------------------------------------------------------------------------------------------
	const Color3f& GuiWindow2::getResizeButtonColor() const
	{
		return resizeButtonColor;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::setResizeButtonSize(const double& resizeButtonSize)
	{
		this->resizeButtonSize = resizeButtonSize;
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow2::setResizeButtonColor(const Color3f& resizeButtonColor)
	{
		this->resizeButtonColor = resizeButtonColor;
	}
	//--------------------------------------------------------------------------------------------------
}