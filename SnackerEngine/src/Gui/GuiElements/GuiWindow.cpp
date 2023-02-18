#include "Gui/GuiElements/GuiWindow.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::computeResizeButtonModelMatrix()
	{
		resizeButtonModelMatrix = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(getPositionX() + getWidth() - resizeButtonSize), static_cast<float>(-getPositionY() - getHeight()), 0.0f), Vec3f(static_cast<float>(resizeButtonSize), static_cast<float>(resizeButtonSize), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::draw(const Vec2i& parentPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		/// Draw the panel first. Also draws all children.
		GuiPanel::draw(parentPosition);
		if (!guiManager) return;
		getPanelShader().bind();
		guiManager->setUniformViewAndProjectionMatrices(getPanelShader());
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		getPanelShader().setUniform<Mat4f>("u_model", resizeButtonModelMatrix * translationMatrix);
		getPanelShader().setUniform<Color4f>("u_color", resizeButtonColor);
		Renderer::draw(guiManager->getModelTriangle());
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::onPositionChange()
	{
		GuiPanel::onPositionChange();
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::onSizeChange()
	{
		GuiPanel::onSizeChange();
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiWindow::isCollidingWithResizeButton(const Vec2i& offset)
	{
		return offset.x >= getWidth() - resizeButtonSize && offset.y >= getHeight() - offset.x + getWidth() - resizeButtonSize;
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::IsCollidingResult GuiWindow::isColliding(const Vec2i& position)
	{
		if (GuiPanel::isColliding(position) != IsCollidingResult::NOT_COLLIDING) {
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
	void GuiWindow::onRegister()
	{
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			isMoving = false;
			isResizing = false;
			signOffEvent(CallbackType::MOUSE_MOTION);
			signOffEvent(CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			// Figure out if the user clicked on the resize button or on the window
			mouseOffset = getMouseOffset(getGuiID());
			if (isCollidingWithResizeButton(mouseOffset)) {
				isResizing = true;
				// When were resizing the mouse offset is relative to the top left corner of the resize button
				mouseOffset = mouseOffset - getSize() + Vec2f(static_cast<float>(resizeButtonSize));
			}
			else {
				isMoving = true;
			}
			signUpEvent(CallbackType::MOUSE_MOTION);
			signUpEvent(CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::callbackMouseMotion(const Vec2d& position)
	{
		if (isMoving) {
			Vec2f newPosition = getMouseOffset(getParentID()) - mouseOffset;
			setPosition(newPosition);
		}
		else if (isResizing) {
			Vec2f newSize = getMouseOffset(getGuiID()) + Vec2f(static_cast<float>(resizeButtonSize)) - mouseOffset;
			// Clip size
			if (newSize.x < resizeButtonSize) newSize.x = static_cast<float>(resizeButtonSize);
			if (newSize.y < resizeButtonSize) newSize.y = static_cast<float>(resizeButtonSize);
			setSize(newSize);
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::GuiWindow(const Vec2i& position, const Vec2i& size, const Color3f& backgroundColor, const double& resizeButtonSize, const Color4f& resizeButtonColor)
		: GuiPanel(position, size, ResizeMode::DO_NOT_RESIZE, backgroundColor), resizeButtonSize(resizeButtonSize), resizeButtonColor(resizeButtonColor),
		resizeButtonModelMatrix{}, isMoving(false), isResizing(false), mouseOffset(Vec2f())
	{
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::GuiWindow(const GuiStyle& style)
		: GuiWindow(Vec2i(), style.guiWindowSize, style.guiWindowBackgroundColor, style.guiWindowResizeButtonSize, style.guiWindowResizeButtonColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::GuiWindow(const GuiWindow& other) noexcept
		: GuiPanel(other), resizeButtonSize(other.resizeButtonSize), resizeButtonColor(other.resizeButtonColor),
		resizeButtonModelMatrix(other.resizeButtonModelMatrix), isMoving(false), isResizing(false), mouseOffset(Vec2f()) {}
	//--------------------------------------------------------------------------------------------------
	GuiWindow& GuiWindow::operator=(const GuiWindow& other) noexcept
	{
		GuiPanel::operator=(other);
		resizeButtonSize = other.resizeButtonSize;
		resizeButtonColor = other.resizeButtonColor;
		resizeButtonModelMatrix = other.resizeButtonModelMatrix;
		isMoving = false;
		isResizing = false;
		mouseOffset = Vec2f();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::GuiWindow(GuiWindow&& other) noexcept
		: GuiPanel(std::move(other)), resizeButtonSize(other.resizeButtonSize), resizeButtonColor(other.resizeButtonColor),
		resizeButtonModelMatrix(other.resizeButtonModelMatrix), isMoving(false), isResizing(false), mouseOffset(Vec2f()) {}
	//--------------------------------------------------------------------------------------------------
	GuiWindow& GuiWindow::operator=(GuiWindow&& other) noexcept
	{
		GuiPanel::operator=(std::move(other));
		resizeButtonSize = other.resizeButtonSize;
		resizeButtonColor = other.resizeButtonColor;
		resizeButtonModelMatrix = other.resizeButtonModelMatrix;
		isMoving = false;
		isResizing = false;
		mouseOffset = Vec2f();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::~GuiWindow()
	{
	}
	//--------------------------------------------------------------------------------------------------
	const double& GuiWindow::getResizeButtonSize() const
	{
		return resizeButtonSize;
	}
	//--------------------------------------------------------------------------------------------------
	const Color4f& GuiWindow::getResizeButtonColor() const
	{
		return resizeButtonColor;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::setResizeButtonSize(const double& resizeButtonSize)
	{
		this->resizeButtonSize = resizeButtonSize;
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::setResizeButtonColor(const Color4f& resizeButtonColor)
	{
		this->resizeButtonColor = resizeButtonColor;
	}
	//--------------------------------------------------------------------------------------------------
}