#include "Gui/Layouts/ListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Color.h"
#include "Graphics/Renderer.h"
#include "Core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{

	void ListLayout::addChild(const GuiID& guiID, const LayoutOptions& options)
	{
		GuiLayout::addChild(guiID, {});
		layoutOptions.push_back(options);
		enforceLayout();
	}

	std::size_t ListLayout::removeChild(GuiElement& guiElement)
	{
		std::size_t index = GuiLayout::removeChild(guiElement);
		layoutOptions.erase(layoutOptions.begin() + index);
		enforceLayout();
		return index;
	}

	void ListLayout::onRegister()
	{
		GuiLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_SCROLL_ON_ELEMENT);
	}

	void ListLayout::callbackMouseScrollOnElement(const Vec2d& offset)
	{
		currentVerticalOffset += offset.y * scrollSpeed;
		computeScrollBar();
	}

	void ListLayout::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		// Draw children
		Vec2i newParentPosition = Vec2i(parentPosition.x, parentPosition.y + currentVerticalOffset);
		for (unsigned int i = firstVisibleElement; i <= lastVisibleElement; ++i) {
			drawElement(children[i], newParentPosition);
		}
		// Draw scrollBar
		if (drawScrollBar) {
			shaderScrollBar.bind();
			guiManager->setUniformViewAndProjectionMatrices(shaderScrollBar);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBarBackground);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarBackgroundColor);
			Renderer::draw(guiManager->getModelSquare());
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBar);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarColor);
			Renderer::draw(guiManager->getModelSquare());
		}
	}

	ListLayout::IsCollidingResult ListLayout::isColliding(const Vec2i& position)
	{
		if (drawScrollBar) {
			// Check if we are colliding with the scroll bar
			Vec2i parentSize = getSize(parentID);
			float scrollBarBackgroundHeight = parentSize.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
			if (position.x >= parentSize.x - leftBorder - scrollBarWidth &&
				position.x <= parentSize.x - leftBorder &&
				position.y >= verticalOffset + scrollBarOffsetTop &&
				position.y <= verticalOffset + scrollBarOffsetTop + scrollBarBackgroundHeight) {
				return IsCollidingResult::STRONG_COLLIDING;
			}
		}
		return GuiLayout::isColliding(position);
	}

	void ListLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i mouseOffset = getMouseOffset(parentID);
			Vec2i parentSize = getSize(parentID);
			float scrollBarBackgroundHeight = parentSize.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
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
		Vec2i mouseOffset = getMouseOffset(parentID);
		Vec2i parentSize = getSize(parentID);
		float scrollBarBackgroundHeight = parentSize.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
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

	std::optional<std::pair<std::pair<ListLayout::GuiID, ListLayout::IsCollidingResult>, Vec2i>> ListLayout::getFirstCollidingChild(const Vec2i& position)
	{
		return GuiLayout::getFirstCollidingChild(Vec2i(position.x, position.y - currentVerticalOffset));
	}

	ListLayout::ListLayout(const float& verticalOffset, const float& leftBorder, const float& scrollSpeed, const Color3f& scrollBarBackgroundColor, const Color3f& scrollBarColor, const float& scrollBarWidth, const float& scrollBarOffsetTop, const float& scrollBarOffsetBottom, const float& scrollBarOffsetRight)
		: GuiLayout(), verticalOffset(verticalOffset), leftBorder(leftBorder), currentVerticalSize(0.0f), currentVerticalOffset(0.0f),
		scrollSpeed(scrollSpeed), drawScrollBar(false), visiblePercentage(0.0f), offsetPercentage(0.0f), modelMatrixScrollBarBackground{},
		modelMatrixScrollBar{}, shaderScrollBar("shaders/gui/simpleColor.shader"), scrollBarBackgroundColor(scrollBarBackgroundColor),
		scrollBarColor(scrollBarColor), scrollBarWidth(scrollBarWidth), scrollBarOffsetTop(scrollBarOffsetTop), 
		scrollBarOffsetBottom(scrollBarOffsetBottom), scrollBarOffsetRight(scrollBarOffsetRight),
		mouseOffsetFromScrollBar(0.0f), firstVisibleElement(0), lastVisibleElement(0), layoutOptions{}
	{
	}

	ListLayout::ListLayout(const GuiStyle& style)
		: ListLayout(style.listLayoutVerticalOffset, style.listLayoutLeftBorder, style.listLayoutScrollSpeed, style.listLayoutScrollBarBackgroundColor, style.listLayoutScrollBarColor, style.listLayoutScrollBarWidth, style.listLayoutScrollBarOffsetTop, style.listLayoutScrollBarOffsetBottom, style.listLayoutScrollBarOffsetRight)
	{
	}

	void ListLayout::enforceLayout() 
	{
		if (!guiManager) return;
		float currentYOffset = verticalOffset;
		double parentWidth = getSize(parentID).x;
		parentWidth -= 2 * leftBorder;
		for (unsigned int i = 0; i < children.size(); ++i) {
			int height = getSize(children[i]).y;
			// Set position
			setPosition(children[i], Vec2i(leftBorder, currentYOffset));
			notifyPositionChange(children[i]);
			// Resize
			switch (resizeMode)
			{
			case SnackerEngine::ListLayout::ListLayoutResizeMode::DONT_RESIZE:
			{
				break;
			}
			case SnackerEngine::ListLayout::ListLayoutResizeMode::RESIZE_WIDTH_IF_POSSIBLE:
			{
				if (parentWidth >= getPreferredMinSize(children[i]).x &&
					parentWidth <= getPreferredMaxSize(children[i]).x) {
					setWidth(children[i], static_cast<int>(parentWidth));
				}
				break;
			}
			case SnackerEngine::ListLayout::ListLayoutResizeMode::RESIZE_WIDTH:
			{
				setWidth(children[i], static_cast<int>(parentWidth));
			}
			default:
				break;
			}
			// Advance to next element
			currentYOffset += height + verticalOffset;
		}
		currentVerticalSize = currentYOffset;
		computeScrollBar();
	}

	void ListLayout::computeScrollBar()
	{
		if (currentVerticalSize == 0.0f) {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = children.size() - 1;
			if (guiManager) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			return;
		}
		Vec2i parentSize = getSize(parentID);
		if (currentVerticalOffset > 0.0f) currentVerticalOffset = 0.0f;
		else if (currentVerticalOffset < -currentVerticalSize + parentSize.y) currentVerticalOffset = -currentVerticalSize + parentSize.y;
		visiblePercentage = parentSize.y / currentVerticalSize;
		if (visiblePercentage >= 1.0f) {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = children.size() - 1;
			if (guiManager) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			currentVerticalOffset = 0.0f;
			return;
		}
		// Check if the scroll bar would be visible at all
		float scrollBarBackgroundHeight = parentSize.y - 2 * verticalOffset - scrollBarOffsetBottom - scrollBarOffsetTop;
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
			lastVisibleElement = children.size() - 1;
			for (unsigned int i = firstVisibleElement + 1; i < children.size(); ++i) {
				if (getPosition(children[i]).y > parentSize.y - currentVerticalOffset) {
					lastVisibleElement = i;
					break;
				}
			}
			if (guiManager) signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			offsetPercentage = currentVerticalOffset / currentVerticalSize;
			// Compute model matrices
			float scrollBarX = parentSize.x - scrollBarOffsetRight - scrollBarWidth;
			modelMatrixScrollBarBackground = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -parentSize.y + verticalOffset + scrollBarOffsetBottom, 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight, 0.0f));
			modelMatrixScrollBar = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -verticalOffset - scrollBarOffsetTop + scrollBarBackgroundHeight * (offsetPercentage - visiblePercentage), 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight * visiblePercentage, 0.0f));
		}
		else {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = children.size() - 1;
			if (guiManager) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		}
	}

}