#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Core/Keys.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{

	void VerticalScrollingListLayout::enforceLayout()
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		float currentYOffset = border;
		float width = static_cast<float>(getSize().x);
		width -= 2 * border;
		const auto& children = getChildren();
		for (unsigned int i = 0; i < children.size(); ++i) {
			int childHeight = getPreferredSize(children[i]).y;
			if (childHeight == -1) childHeight = getSize(children[i]).y;
			childHeight = guiManager->clipHeightToMinMaxHeight(childHeight, children[i]);
			// Set position
			setPositionOfChild(children[i], Vec2i(static_cast<int>(std::floor(border)), static_cast<int>(std::floor(currentYOffset))));
			// Resize
			// int childWidth = getPreferredSize(children[i]).x;
			int childWidth = width;
			if (childWidth == -1) childWidth = getSize(children[i]).x;
			childWidth = guiManager->clipWidthToMinMaxWidth(childWidth, children[i]);
			// Set position and size
			setPositionAndSizeOfChild(children[i], Vec2i(static_cast<int>(std::floor(border)), static_cast<int>(std::floor(currentYOffset))), Vec2i(childWidth, childHeight));
			// Advance to next element
			currentYOffset += childHeight + border;
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
			lastVisibleElement = children.empty() ? 0 : static_cast<unsigned int>(children.size()) - 1;
			if (getGuiManager()) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			return;
		}
		Vec2i size = getSize();
		if (currentVerticalOffset > 0.0f) currentVerticalOffset = 0.0f;
		else if (currentVerticalOffset < -currentVerticalSize + size.y) currentVerticalOffset = -currentVerticalSize + size.y;
		visiblePercentage = size.y / currentVerticalSize;
		if (visiblePercentage >= 1.0f) {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = children.empty() ? 0 : static_cast<unsigned int>(children.size()) - 1;
			if (getGuiManager()) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
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
			lastVisibleElement = children.empty() ? 0 : static_cast<unsigned int>(children.size()) - 1;
			for (unsigned int i = firstVisibleElement + 1; i < children.size(); ++i) {
				if (getPosition(children[i]).y > size.y - currentVerticalOffset) {
					lastVisibleElement = i;
					break;
				}
			}
			if (getGuiManager()) signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			offsetPercentage = currentVerticalOffset / currentVerticalSize;
			// Compute model matrices
			float scrollBarX = size.x - scrollBarOffsetRight - scrollBarWidth;
			modelMatrixScrollBarBackground = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -size.y + static_cast<int>(border) + static_cast<int>(scrollBarOffsetBottom), 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight, 0.0f));
			modelMatrixScrollBar = Mat4f::TranslateAndScale(Vec3f(scrollBarX, -static_cast<float>(border) - scrollBarOffsetTop + scrollBarBackgroundHeight * (offsetPercentage - visiblePercentage), 0.0f), Vec3f(scrollBarWidth, scrollBarBackgroundHeight * visiblePercentage, 0.0f));
		}
		else {
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = children.empty() ? 0 : static_cast<unsigned int>(children.size()) - 1;
			if (getGuiManager()) signOffEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
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
	
	void VerticalScrollingListLayout::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		if (backgroundColor.alpha != 0.0f)
		{
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
			backgroundShader.setUniform<Color3f>("u_color", Color3f(backgroundColor.r, backgroundColor.g, backgroundColor.b));
			Renderer::draw(guiManager->getModelSquare());
		}
		pushClippingBox(worldPosition);
		// Draw children
		const auto& children = getChildren();
		for (unsigned int i = firstVisibleElement; i < std::min(static_cast<std::size_t>(lastVisibleElement + 1), children.size()); ++i) {
			drawElement(children[i], worldPosition + getPosition(children[i]) + Vec2i(0, static_cast<int>(std::floor(currentVerticalOffset))));
		}
		// Draw scrollBar
		if (drawScrollBar) {
			shaderScrollBar.bind();
			guiManager->setUniformViewAndProjectionMatrices(shaderScrollBar);
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBarBackground);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarBackgroundColor);
			Renderer::draw(guiManager->getModelSquare());
			shaderScrollBar.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollBar);
			shaderScrollBar.setUniform<Color3f>("u_color", scrollBarColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		popClippingBox();
	}

	VerticalScrollingListLayout::IsCollidingResult VerticalScrollingListLayout::isColliding(const Vec2i& offset)
	{
		if (offset.x >=0 && offset.x <= getWidth() &&
			offset.y >=0 && offset.y <= getHeight()) {
			if (drawScrollBar) {
				// Check if we are colliding with the scroll bar
				Vec2i size = getSize();
				float scrollBarBackgroundHeight = size.y - 2 * border - scrollBarOffsetBottom - scrollBarOffsetTop;
				if (offset.x >= size.x - scrollBarOffsetRight - scrollBarWidth &&
					offset.x <= size.x - scrollBarOffsetRight &&
					offset.y >= scrollBarOffsetTop &&
					offset.y <= scrollBarOffsetTop + scrollBarBackgroundHeight) {
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

	VerticalScrollingListLayout::GuiID VerticalScrollingListLayout::getCollidingChild(const Vec2i& offset)
	{
		return GuiLayout::getCollidingChild(Vec2i(offset.x, offset.y - static_cast<int>(std::floor(currentVerticalOffset))));
	}

	Vec2i VerticalScrollingListLayout::getChildOffset(const GuiID& childID)
	{
		Vec2i result = getPosition(childID);
		result.y += static_cast<int>(std::floor(currentVerticalOffset));
		return result;
	}

	VerticalScrollingListLayout::VerticalScrollingListLayout(const unsigned& border, AlignmentHorizontal alignmentHorizontal, AlignmentVertical alignmentVertical, const float& scrollSpeed, const Color3f& scrollBarBackgroundColor, const Color3f& scrollBarColor, const float& scrollBarWidth, const float& scrollBarOffsetTop, const float& scrollBarOffsetBottom, const float& scrollBarOffsetRight)
		: VerticalListLayout(border, false, false, false, false, alignmentHorizontal, alignmentVertical), currentVerticalSize(0.0f), currentVerticalOffset(0.0f),
		scrollSpeed(scrollSpeed), drawScrollBar(false), visiblePercentage(0.0f), offsetPercentage(0.0f), modelMatrixScrollBarBackground{},
		modelMatrixScrollBar{}, shaderScrollBar("shaders/gui/simpleColor.shader"), scrollBarBackgroundColor(scrollBarBackgroundColor),
		scrollBarColor(scrollBarColor), scrollBarWidth(scrollBarWidth), scrollBarOffsetTop(scrollBarOffsetTop),
		scrollBarOffsetBottom(scrollBarOffsetBottom), scrollBarOffsetRight(scrollBarOffsetRight),
		mouseOffsetFromScrollBar(0.0f), firstVisibleElement(0), lastVisibleElement(0) {}

	VerticalScrollingListLayout::VerticalScrollingListLayout(const GuiStyle& style)
		: VerticalScrollingListLayout(style.verticalScrollingListLayoutVerticalOffset, AlignmentHorizontal::LEFT,
			AlignmentVertical::TOP, style.verticalScrollingListLayoutScrollSpeed,
			style.verticalScrollingListLayoutScrollBarBackgroundColor, style.verticalScrollingListLayoutScrollBarColor, style.verticalScrollingListLayoutScrollBarWidth,
			style.verticalScrollingListLayoutScrollBarOffsetTop, style.verticalScrollingListLayoutScrollBarOffsetBottom, style.verticalScrollingListLayoutScrollBarOffsetRight) {}

	VerticalScrollingListLayout::VerticalScrollingListLayout(const VerticalScrollingListLayout& other) noexcept
		: VerticalListLayout(other), currentVerticalSize(0.0f), currentVerticalOffset(0.0f), scrollSpeed(other.scrollSpeed),
		drawScrollBar(false), visiblePercentage(0.0f), offsetPercentage(0.0f), modelMatrixScrollBarBackground{},
		modelMatrixScrollBar{}, shaderScrollBar(other.shaderScrollBar), scrollBarBackgroundColor(other.scrollBarBackgroundColor), 
		scrollBarColor(other.scrollBarColor), scrollBarWidth(other.scrollBarWidth), scrollBarOffsetTop(other.scrollBarOffsetTop), 
		scrollBarOffsetBottom(other.scrollBarOffsetBottom), scrollBarOffsetRight(other.scrollBarOffsetRight), 
		mouseOffsetFromScrollBar(0.0f), firstVisibleElement(0), lastVisibleElement(0) {}

	VerticalScrollingListLayout& VerticalScrollingListLayout::operator=(const VerticalScrollingListLayout& other) noexcept
	{
		VerticalListLayout::operator=(other);
		currentVerticalSize = 0.0f;
		currentVerticalOffset = 0.0f;
		scrollSpeed = other.scrollSpeed;
		drawScrollBar = false;
		visiblePercentage = 0.0f;
		offsetPercentage = 0.0f;
		modelMatrixScrollBarBackground = Mat4f();
		modelMatrixScrollBar = Mat4f();
		shaderScrollBar = other.shaderScrollBar;
		scrollBarBackgroundColor = other.scrollBarBackgroundColor;
		scrollBarColor = other.scrollBarColor;
		scrollBarWidth = other.scrollBarWidth;
		scrollBarOffsetTop = other.scrollBarOffsetTop;
		scrollBarOffsetBottom = other.scrollBarOffsetBottom;
		scrollBarOffsetRight = other.scrollBarOffsetRight;
		mouseOffsetFromScrollBar = 0.0f;
		firstVisibleElement = 0;
		lastVisibleElement = 0;
		return *this;
	}

	VerticalScrollingListLayout::VerticalScrollingListLayout(VerticalScrollingListLayout&& other) noexcept
		: VerticalListLayout(std::move(other)), currentVerticalSize(other.currentVerticalSize), currentVerticalOffset(other.currentVerticalOffset),
		scrollSpeed(other.scrollSpeed), drawScrollBar(other.drawScrollBar), visiblePercentage(other.visiblePercentage),
		offsetPercentage(other.offsetPercentage), modelMatrixScrollBarBackground(other.modelMatrixScrollBarBackground),
		modelMatrixScrollBar(other.modelMatrixScrollBar), shaderScrollBar(other.shaderScrollBar),
		scrollBarBackgroundColor(other.scrollBarBackgroundColor), scrollBarColor(other.scrollBarColor),
		scrollBarWidth(other.scrollBarWidth), scrollBarOffsetTop(other.scrollBarOffsetTop), scrollBarOffsetBottom(other.scrollBarOffsetBottom),
		scrollBarOffsetRight(other.scrollBarOffsetRight), mouseOffsetFromScrollBar(other.mouseOffsetFromScrollBar),
		firstVisibleElement(other.firstVisibleElement), lastVisibleElement(other.lastVisibleElement)
	{
	}

	VerticalScrollingListLayout& VerticalScrollingListLayout::operator=(VerticalScrollingListLayout&& other) noexcept
	{
		VerticalListLayout::operator=(std::move(other));
		currentVerticalSize = other.currentVerticalSize;
		currentVerticalOffset = other.currentVerticalOffset;
		scrollSpeed = other.scrollSpeed;
		drawScrollBar = other.drawScrollBar;
		visiblePercentage = other.visiblePercentage;
		offsetPercentage = other.offsetPercentage;
		modelMatrixScrollBarBackground = other.modelMatrixScrollBarBackground;
		modelMatrixScrollBar = other.modelMatrixScrollBar;
		shaderScrollBar = other.shaderScrollBar;
		scrollBarBackgroundColor = other.scrollBarBackgroundColor;
		scrollBarColor = other.scrollBarColor;
		scrollBarWidth = other.scrollBarWidth;
		scrollBarOffsetTop = other.scrollBarOffsetTop;
		scrollBarOffsetBottom = other.scrollBarOffsetBottom;
		scrollBarOffsetRight = other.scrollBarOffsetRight;
		mouseOffsetFromScrollBar = other.mouseOffsetFromScrollBar;
		firstVisibleElement = other.firstVisibleElement;
		lastVisibleElement = other.lastVisibleElement;
		return *this;
	}

}