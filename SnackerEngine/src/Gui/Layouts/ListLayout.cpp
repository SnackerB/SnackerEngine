#include "Gui/Layouts/ListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{

	void ListLayout::enforceLayout()
	{
		if (!guiManager) return;
		float currentYOffset = verticalOffset;
		float width = static_cast<float>(getSize().x);
		width -= 2 * leftBorder;
		const auto& children = getChildren();
		for (unsigned int i = 0; i < children.size(); ++i) {
			int height = getSize(children[i]).y;
			// Set position
			setPositionWithoutEnforcingLayouts(children[i], Vec2i(static_cast<int>(std::floor(leftBorder)), static_cast<int>(std::floor(currentYOffset))));
			// Resize
			switch (listLayoutResizeMode)
			{
			case SnackerEngine::ListLayout::ListLayoutResizeMode::DONT_RESIZE:
			{
				break;
			}
			case SnackerEngine::ListLayout::ListLayoutResizeMode::RESIZE_WIDTH_IF_POSSIBLE:
			{
				switch (getResizeMode(children[i]))
				{
				case ResizeMode::DO_NOT_RESIZE:
				{
					break;
				}
				case ResizeMode::SAME_AS_PARENT:
				{
					setWidthWithoutEnforcingLayouts(children[i], static_cast<int>(width));
					break;
				}
				case ResizeMode::RESIZE_RANGE:
				{
					int newWidth = static_cast<int>(std::roundf(width));
					if (width <= getMinSize(children[i]).x) newWidth = getMinSize(children[i]).x;
					else if (getMaxSize(children[i]).x != -1 && width >= getMaxSize(children[i]).x) newWidth = getMaxSize(children[i]).x;
					setWidthWithoutEnforcingLayouts(children[i], static_cast<int>(newWidth));
					break;
				}
				default:
					break;
				}
				break;
			}
			case SnackerEngine::ListLayout::ListLayoutResizeMode::RESIZE_WIDTH:
			{
				setSizeWithoutEnforcingLayouts(children[i], static_cast<int>(width));
			}
			default:
				break;
			}
			// Advance to next element
			currentYOffset += height + verticalOffset;
			// Enforce layout of child element
			enforceLayoutOnElement(children[i]);
		}
		currentVerticalSize = currentYOffset;
		computeScrollBar();
	}
	
	void ListLayout::computeScrollBar()
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
		float scrollBarBackgroundHeight = size.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
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
			modelMatrixScrollBarBackground = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -size.y + verticalOffset + scrollBarOffsetBottom, 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight, 0.0f));
			modelMatrixScrollBar = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -verticalOffset - scrollBarOffsetTop + scrollBarBackgroundHeight * (offsetPercentage - visiblePercentage), 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight * visiblePercentage, 0.0f));
		}
		else {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = static_cast<unsigned int>(children.size()) - 1;
			if (guiManager) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		}
	}
	
	void ListLayout::onRegister()
	{
		GuiLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_SCROLL_ON_ELEMENT);
	}
	
	void ListLayout::callbackMouseScrollOnElement(const Vec2d& offset)
	{
		currentVerticalOffset += static_cast<float>(offset.y) * scrollSpeed;
		computeScrollBar();
	}
	
	void ListLayout::draw(const Vec2i& parentPosition)
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
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x + getPositionX()), -static_cast<float>(parentPosition.y - getPositionY()), 0.0f));
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBarBackground);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarBackgroundColor);
			Renderer::draw(guiManager->getModelSquare());
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBar);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		popClippingBox();
	}
	
	ListLayout::IsCollidingResult ListLayout::isColliding(const Vec2i& position)
	{
		if (position.x >= getPositionX() && position.x <= getPositionX() + getWidth() &&
			position.y >= getPositionY() && position.y <= getPositionY() + getHeight()) {
			if (drawScrollBar) {
				// Check if we are colliding with the scroll bar
				Vec2i size = getSize();
				float scrollBarBackgroundHeight = size.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
				if (position.x >= getPositionX() + size.x - leftBorder - scrollBarWidth &&
					position.x <= getPositionX() + size.x - leftBorder &&
					position.y >= getPositionY() + verticalOffset + scrollBarOffsetTop &&
					position.y <= getPositionY() + verticalOffset + scrollBarOffsetTop + scrollBarBackgroundHeight) {
					return IsCollidingResult::COLLIDE_STRONG;
				}
			}
			return IsCollidingResult::COLLIDE_CHILD;
		}
		return IsCollidingResult::NOT_COLLIDING;
	}

	void ListLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i mouseOffset = getMouseOffset(getGuiID());
			Vec2i size = getSize();
			float scrollBarBackgroundHeight = size.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
			if (mouseOffset.y >= verticalOffset + scrollBarOffsetTop - scrollBarBackgroundHeight * offsetPercentage &&
				mouseOffset.y <= verticalOffset + scrollBarOffsetTop - scrollBarBackgroundHeight * (offsetPercentage - visiblePercentage)) {
				mouseOffsetFromScrollBar = mouseOffset.y - (verticalOffset + scrollBarOffsetTop - scrollBarBackgroundHeight * offsetPercentage);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
		}
	}

	void ListLayout::callbackMouseMotion(const Vec2d& position)
	{
		Vec2i mouseOffset = getMouseOffset(getGuiID());
		Vec2i size = getSize();
		float scrollBarBackgroundHeight = size.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
		if (scrollBarBackgroundHeight == 0.0f) return;
		float newOffsetPercentage = (mouseOffsetFromScrollBar - mouseOffset.y + verticalOffset + scrollBarOffsetTop) / scrollBarBackgroundHeight;
		currentVerticalOffset = currentVerticalSize * newOffsetPercentage;
		computeScrollBar();
	}

	void ListLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	ListLayout::GuiID ListLayout::getCollidingChild(const Vec2i& position)
	{
		return GuiLayout::getCollidingChild(Vec2i(position.x, position.y - static_cast<int>(std::floor(currentVerticalOffset))));
	}

	Vec2i ListLayout::getChildOffset(const GuiID& childID)
	{
		Vec2i result = getPosition(childID);
		result.y += static_cast<int>(std::floor(currentVerticalOffset));
		return result;
	}

	ListLayout::ListLayout(const float& verticalOffset, const float& leftBorder, const float& scrollSpeed, const Color3f& scrollBarBackgroundColor, const Color3f& scrollBarColor, const float& scrollBarWidth, const float& scrollBarOffsetTop, const float& scrollBarOffsetBottom, const float& scrollBarOffsetRight, const ListLayoutResizeMode& resizeMode)
		: GuiLayout(), verticalOffset(verticalOffset), leftBorder(leftBorder), currentVerticalSize(0.0f), currentVerticalOffset(0.0f),
		scrollSpeed(scrollSpeed), drawScrollBar(false), visiblePercentage(0.0f), offsetPercentage(0.0f), modelMatrixScrollBarBackground{},
		modelMatrixScrollBar{}, shaderScrollBar("shaders/gui/simpleColor.shader"), scrollBarBackgroundColor(scrollBarBackgroundColor),
		scrollBarColor(scrollBarColor), scrollBarWidth(scrollBarWidth), scrollBarOffsetTop(scrollBarOffsetTop),
		scrollBarOffsetBottom(scrollBarOffsetBottom), scrollBarOffsetRight(scrollBarOffsetRight),
		mouseOffsetFromScrollBar(0.0f), firstVisibleElement(0), lastVisibleElement(0), listLayoutResizeMode(resizeMode) {}

	ListLayout::ListLayout(const GuiStyle& style)
		: ListLayout(style.listLayoutVerticalOffset, style.listLayoutLeftBorder, style.listLayoutScrollSpeed, 
			style.listLayoutScrollBarBackgroundColor, style.listLayoutScrollBarColor, style.listLayoutScrollBarWidth, 
			style.listLayoutScrollBarOffsetTop, style.listLayoutScrollBarOffsetBottom, style.listLayoutScrollBarOffsetRight,
			style.listLayoutResizeMode) {}

}