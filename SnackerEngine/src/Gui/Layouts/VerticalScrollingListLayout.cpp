#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{

	void VerticalScrollingListLayout::enforceLayout()
	{
		if (!guiManager) return;
		float currentYOffset = border;
		float width = static_cast<float>(getSize().x);
		width -= 2 * border;
		const auto& children = getChildren();
		for (unsigned int i = 0; i < children.size(); ++i) {
			int height = getPreferredSize(children[i]).y;
			if (height == -1) height = getSize(children[i]).y;
			height = std::max(std::min(height, getMaxSize(children[i]).y), getMinSize(children[i]).y);
			// Set position
			setPositionWithoutEnforcingLayouts(children[i], Vec2i(static_cast<int>(std::floor(border)), static_cast<int>(std::floor(currentYOffset))));
			// Resize
			int width = getPreferredSize(children[i]).x;
			if (width == -1) width = getSize(children[i]).x;
			width = std::max(std::min(width, getMaxSize(children[i]).x), getMinSize(children[i]).x);
			// Set position and size
			setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(static_cast<int>(std::floor(border)), static_cast<int>(std::floor(currentYOffset))), Vec2i(width, height));
			// Advance to next element
			currentYOffset += height + border;
			// Enforce layout of child element
			enforceLayoutOnElement(children[i]);
		}
		currentVerticalSize = currentYOffset;
		computeScrollBar();
	}
	
	void VerticalScrollingListLayout::computeScrollBar()
	{
		const auto& children = getChildren();
		if (currentVerticalSize == 0.0f) {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = static_cast<unsigned int>(children.size()) - 1;
			if (guiManager) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			return;
		}
		Vec2i size = getSize();
		if (currentVerticalOffset > 0.0f) currentVerticalOffset = 0.0f;
		else if (currentVerticalOffset < -currentVerticalSize + size.y) currentVerticalOffset = -currentVerticalSize + size.y;
		visiblePercentage = size.y / currentVerticalSize;
		if (visiblePercentage >= 1.0f) {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = static_cast<unsigned int>(children.size()) - 1;
			if (guiManager) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			currentVerticalOffset = 0.0f;
			return;
		}
		// Check if the scroll bar would be visible at all
		float scrollBarBackgroundHeight = size.y - 2 * border - scrollBarOffsetBottom - scrollBarOffsetTop;
		if (scrollBarBackgroundHeight > 0.0f) {
			drawScrollBar = true;
			// Compute first and last visible element
			firstVisibleElement = 0;
			for (unsigned int i = 0; i < children.size(); ++i) {
				if (getPosition(children[i]).y > -currentVerticalOffset) {
					firstVisibleElement = i > 0 ? i - 1 : 0;
					break;
				}
			}
			lastVisibleElement = static_cast<unsigned int>(children.size()) - 1;
			for (unsigned int i = firstVisibleElement + 1; i < children.size(); ++i) {
				if (getPosition(children[i]).y > size.y - currentVerticalOffset) {
					lastVisibleElement = i;
					break;
				}
			}
			if (guiManager) signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			offsetPercentage = currentVerticalOffset / currentVerticalSize;
			// Compute model matrices
			float scrollBarX = size.x - scrollBarOffsetRight - scrollBarWidth;
			modelMatrixScrollBarBackground = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -size.y + border + scrollBarOffsetBottom, 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight, 0.0f));
			modelMatrixScrollBar = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -static_cast<float>(border) - scrollBarOffsetTop + scrollBarBackgroundHeight * (offsetPercentage - visiblePercentage), 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight * visiblePercentage, 0.0f));
		}
		else {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = static_cast<unsigned int>(children.size()) - 1;
			if (guiManager) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		}
	}
	
	void VerticalScrollingListLayout::onRegister()
	{
		GuiLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_SCROLL_ON_ELEMENT);
	}
	
	void VerticalScrollingListLayout::callbackMouseScrollOnElement(const Vec2d& offset)
	{
		currentVerticalOffset += static_cast<float>(offset.y) * scrollSpeed;
		computeScrollBar();
	}
	
	void VerticalScrollingListLayout::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		pushClippingBox(parentPosition);
		// Draw children
		const auto& children = getChildren();
		Vec2i newParentPosition = parentPosition + getPosition();
		for (unsigned int i = firstVisibleElement; i <= lastVisibleElement; ++i) {
			drawElement(children[i], newParentPosition + Vec2i(0, static_cast<int>(std::floor(currentVerticalOffset))));
		}
		// Draw scrollBar
		if (drawScrollBar) {
			shaderScrollBar.bind();
			guiManager->setUniformViewAndProjectionMatrices(shaderScrollBar);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x + getPositionX()), -static_cast<float>(parentPosition.y + getPositionY()), 0.0f));
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBarBackground);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarBackgroundColor);
			Renderer::draw(guiManager->getModelSquare());
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBar);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		popClippingBox();
	}

	VerticalScrollingListLayout::IsCollidingResult VerticalScrollingListLayout::isColliding(const Vec2i& position)
	{
		if (position.x >= getPositionX() && position.x <= getPositionX() + getWidth() &&
			position.y >= getPositionY() && position.y <= getPositionY() + getHeight()) {
			if (drawScrollBar) {
				// Check if we are colliding with the scroll bar
				Vec2i size = getSize();
				float scrollBarBackgroundHeight = size.y - 2 * border - scrollBarOffsetBottom - scrollBarOffsetTop;
				if (position.x >= getPositionX() + size.x - border - scrollBarWidth &&
					position.x <= getPositionX() + size.x - border &&
					position.y >= getPositionY() + border + scrollBarOffsetTop &&
					position.y <= getPositionY() + border + scrollBarOffsetTop + scrollBarBackgroundHeight) {
					return IsCollidingResult::COLLIDE_STRONG;
				}
			}
			return IsCollidingResult::COLLIDE_CHILD;
		}
		return IsCollidingResult::NOT_COLLIDING;
	}

	void VerticalScrollingListLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i mouseOffset = getMouseOffset(getGuiID());
			Vec2i size = getSize();
			float scrollBarBackgroundHeight = size.y - 2 * border - scrollBarOffsetBottom - scrollBarOffsetTop;
			if (mouseOffset.y >= border + scrollBarOffsetTop - scrollBarBackgroundHeight * offsetPercentage &&
				mouseOffset.y <= border + scrollBarOffsetTop - scrollBarBackgroundHeight * (offsetPercentage - visiblePercentage)) {
				mouseOffsetFromScrollBar = mouseOffset.y - (border + scrollBarOffsetTop - scrollBarBackgroundHeight * offsetPercentage);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
		}
	}

	void VerticalScrollingListLayout::callbackMouseMotion(const Vec2d& position)
	{
		Vec2i mouseOffset = getMouseOffset(getGuiID());
		Vec2i size = getSize();
		float scrollBarBackgroundHeight = size.y - 2 * border - scrollBarOffsetBottom - scrollBarOffsetTop;
		if (scrollBarBackgroundHeight == 0.0f) return;
		float newOffsetPercentage = (mouseOffsetFromScrollBar - mouseOffset.y + border + scrollBarOffsetTop) / scrollBarBackgroundHeight;
		currentVerticalOffset = currentVerticalSize * newOffsetPercentage;
		computeScrollBar();
	}

	void VerticalScrollingListLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	VerticalScrollingListLayout::GuiID VerticalScrollingListLayout::getCollidingChild(const Vec2i& position)
	{
		return GuiLayout::getCollidingChild(Vec2i(position.x, position.y - static_cast<int>(std::floor(currentVerticalOffset))));
	}

	Vec2i VerticalScrollingListLayout::getChildOffset(const GuiID& childID)
	{
		Vec2i result = getPosition(childID);
		result.y += static_cast<int>(std::floor(currentVerticalOffset));
		return result;
	}

	VerticalScrollingListLayout::VerticalScrollingListLayout(const unsigned& border, AlignmentHorizontal alignmentHorizontal, AlignmentVertical alignmentVertical, const float& scrollSpeed, const Color3f& scrollBarBackgroundColor, const Color3f& scrollBarColor, const float& scrollBarWidth, const float& scrollBarOffsetTop, const float& scrollBarOffsetBottom, const float& scrollBarOffsetRight)
		: VerticalListLayout(border, false, alignmentHorizontal, alignmentVertical), currentVerticalSize(0.0f), currentVerticalOffset(0.0f),
		scrollSpeed(scrollSpeed), drawScrollBar(false), visiblePercentage(0.0f), offsetPercentage(0.0f), modelMatrixScrollBarBackground{},
		modelMatrixScrollBar{}, shaderScrollBar("shaders/gui/simpleColor.shader"), scrollBarBackgroundColor(scrollBarBackgroundColor),
		scrollBarColor(scrollBarColor), scrollBarWidth(scrollBarWidth), scrollBarOffsetTop(scrollBarOffsetTop),
		scrollBarOffsetBottom(scrollBarOffsetBottom), scrollBarOffsetRight(scrollBarOffsetRight),
		mouseOffsetFromScrollBar(0.0f), firstVisibleElement(0), lastVisibleElement(0) {}

	VerticalScrollingListLayout::VerticalScrollingListLayout(const GuiStyle& style)
		: VerticalScrollingListLayout(style.listLayoutVerticalOffset, AlignmentHorizontal::LEFT,
			AlignmentVertical::TOP, style.listLayoutScrollSpeed,
			style.listLayoutScrollBarBackgroundColor, style.listLayoutScrollBarColor, style.listLayoutScrollBarWidth,
			style.listLayoutScrollBarOffsetTop, style.listLayoutScrollBarOffsetBottom, style.listLayoutScrollBarOffsetRight) {}

}