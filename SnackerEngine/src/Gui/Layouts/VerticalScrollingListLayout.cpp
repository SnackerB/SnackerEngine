#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Core/Keys.h"

namespace SnackerEngine
{

	Color4f GuiVerticalScrollingListLayout::defaultBackgroundColor = Color4f(0.4f, 1.0f);
	unsigned GuiVerticalScrollingListLayout::defaultScrollBarWidth = 20;
	Color4f GuiVerticalScrollingListLayout::defaultScrollbarColor = Color4f(0.0f, 0.0f, 1.0f, 1.0f);
	Color4f GuiVerticalScrollingListLayout::defaultScrollbarHoverColor = Color4f(0.0f, 0.0f, 0.8f, 1.0f);
	Color4f GuiVerticalScrollingListLayout::defaultScrollbarPressedColor = Color4f(0.0f, 0.0f, 0.6f, 1.0f);
	Color4f GuiVerticalScrollingListLayout::defaultScrollbarBackgroundColor = Color4f(0.0f, 1.0f);
	unsigned GuiVerticalScrollingListLayout::defaultScrollbarBorder = 10;

	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(Color4f backgroundColor, Color4f scrollbarColor, Color4f scrollbarHoverColor, Color4f scrollbarPressedColor, Color4f scrollbarBackgroundColor)
		: GuiVerticalListLayout(backgroundColor), scrollbarColor(scrollbarColor), scrollbarHoverColor(scrollbarHoverColor), scrollbarPressedColor(scrollbarPressedColor) {}

	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVerticalListLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(scrollSpeed, "scrollSpeed", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarBackgroundColor, "scrollbarBackgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarColor, "scrollbarColor", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarHoverColor, "scrollbarHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarPressedColor, "scrollbarPressedColor", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarWidth, "scrollbarWidth", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarBorder, "scrollbarBorder", json, data, parameterNames);
	}

	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(const GuiVerticalScrollingListLayout& other) noexcept
		: GuiVerticalListLayout(other), currentVerticalHeight(other.currentVerticalHeight), currentVerticalOffset(other.currentVerticalOffset),
		scrollSpeed(other.scrollSpeed), drawScrollBar(other.drawScrollBar), visiblePercentage(other.visiblePercentage), 
		offsetPercentage(other.offsetPercentage), scrollbarBackgroundColor(other.scrollbarBackgroundColor), 
		scrollbarColor(other.scrollbarColor), scrollbarHoverColor(other.scrollbarHoverColor),
		scrollbarPressedColor(other.scrollbarPressedColor), modelMatrixScrollbar{}, modelMatrixScrollbarBackground{},
		currentScrollbarColor(other.currentScrollbarColor), scrollbarWidth(other.scrollbarWidth), 
		scrollbarBorder(other.scrollbarBorder), mouseOffsetFromScrollBar(0.0f), 
		firstVisibleElement(other.firstVisibleElement), lastVisibleElement(other.lastVisibleElement) {}

	GuiVerticalScrollingListLayout& GuiVerticalScrollingListLayout::operator=(const GuiVerticalScrollingListLayout& other) noexcept
	{
		GuiVerticalListLayout::operator=(other);
		currentVerticalHeight = other.currentVerticalHeight;
		currentVerticalOffset = other.currentVerticalOffset;
		scrollSpeed = other.scrollSpeed;
		drawScrollBar = other.drawScrollBar;
		visiblePercentage = other.visiblePercentage;
		offsetPercentage = other.offsetPercentage; 
		scrollbarBackgroundColor = other.scrollbarBackgroundColor;
		scrollbarColor = other.scrollbarColor; 
		scrollbarHoverColor = other.scrollbarHoverColor;
		scrollbarPressedColor = other.scrollbarPressedColor; 
		modelMatrixScrollbar = Mat4f();
		modelMatrixScrollbarBackground = Mat4f();
		currentScrollbarColor = other.currentScrollbarColor;
		scrollbarWidth = other.scrollbarWidth;
		scrollbarBorder = other.scrollbarBorder;
		mouseOffsetFromScrollBar = 0.0f; 
		firstVisibleElement = other.firstVisibleElement;
		lastVisibleElement = other.lastVisibleElement;
		return *this;
	}

	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(GuiVerticalScrollingListLayout&& other) noexcept
		: GuiVerticalListLayout(std::move(other)), currentVerticalHeight(other.currentVerticalHeight), currentVerticalOffset(other.currentVerticalOffset),
		scrollSpeed(other.scrollSpeed), drawScrollBar(other.drawScrollBar), visiblePercentage(other.visiblePercentage),
		offsetPercentage(other.offsetPercentage), scrollbarBackgroundColor(other.scrollbarBackgroundColor),
		scrollbarColor(other.scrollbarColor), scrollbarHoverColor(other.scrollbarHoverColor),
		scrollbarPressedColor(other.scrollbarPressedColor), modelMatrixScrollbar(other.modelMatrixScrollbar), 
		modelMatrixScrollbarBackground(other.modelMatrixScrollbarBackground),
		currentScrollbarColor(other.currentScrollbarColor),scrollbarWidth(other.scrollbarWidth), 
		scrollbarBorder(other.scrollbarBorder), mouseOffsetFromScrollBar(0.0f),
		firstVisibleElement(other.firstVisibleElement), lastVisibleElement(other.lastVisibleElement) {}

	GuiVerticalScrollingListLayout& GuiVerticalScrollingListLayout::operator=(GuiVerticalScrollingListLayout&& other) noexcept
	{
		GuiVerticalListLayout::operator=(std::move(other));
		currentVerticalHeight = other.currentVerticalHeight;
		currentVerticalOffset = other.currentVerticalOffset;
		scrollSpeed = other.scrollSpeed;
		drawScrollBar = other.drawScrollBar;
		visiblePercentage = other.visiblePercentage;
		offsetPercentage = other.offsetPercentage;
		scrollbarBackgroundColor = other.scrollbarBackgroundColor;
		scrollbarColor = other.scrollbarColor;
		scrollbarHoverColor = other.scrollbarHoverColor;
		scrollbarPressedColor = other.scrollbarPressedColor;
		modelMatrixScrollbar = other.modelMatrixScrollbar;
		modelMatrixScrollbarBackground = other.modelMatrixScrollbarBackground;
		currentScrollbarColor = other.currentScrollbarColor;
		scrollbarWidth = other.scrollbarWidth;
		scrollbarBorder = other.scrollbarBorder;
		mouseOffsetFromScrollBar = 0.0f;
		firstVisibleElement = other.firstVisibleElement;
		lastVisibleElement = other.lastVisibleElement;
		return *this;
	}

	void GuiVerticalScrollingListLayout::computeScrollBar()
	{
		if (static_cast<int>(currentVerticalHeight) <= getHeight()) {
			visiblePercentage = 1.0f;
			offsetPercentage = 0.0f;
			currentVerticalOffset = 0;
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = static_cast<int>(getChildren().size()) - 1;
		}
		else {
			visiblePercentage = float(getHeight()) / float(currentVerticalHeight);
			offsetPercentage = std::max(0.0f, std::min(offsetPercentage, float(currentVerticalHeight - getHeight()) / float(currentVerticalHeight)));
			currentVerticalOffset = static_cast<int>(std::roundf(offsetPercentage * static_cast<float>(currentVerticalHeight)));
			drawScrollBar = true;
			computeScrollbarModelMatrices();
			computeFirstAndLastVisibleElements();
		}
	}

	void GuiVerticalScrollingListLayout::computeScrollbarModelMatrices()
	{
		float scrollbarPositionX = static_cast<float>(getWidth() - scrollbarBorder - scrollbarWidth);
		float scrollbarBackgroundPositionY = -static_cast<float>(getHeight()) + static_cast<float>(scrollbarBorder);
		float scrollbarBackgroundHeight = std::max(0.0f, static_cast<float>(getHeight() - 2 * static_cast<int>(scrollbarBorder)));
		modelMatrixScrollbarBackground = Mat4f::TranslateAndScale(Vec2f(scrollbarPositionX, scrollbarBackgroundPositionY), Vec2f(static_cast<float>(scrollbarWidth), scrollbarBackgroundHeight));
		modelMatrixScrollbar = Mat4f::TranslateAndScale(Vec2f(scrollbarPositionX, scrollbarBackgroundPositionY + (1.0f - offsetPercentage) * scrollbarBackgroundHeight - visiblePercentage * scrollbarBackgroundHeight), Vec2f(static_cast<float>(scrollbarWidth), visiblePercentage * scrollbarBackgroundHeight));
	}

	void GuiVerticalScrollingListLayout::computeFirstAndLastVisibleElements()
	{
		/// Compute first visible element
		const auto& children = getChildren();
		firstVisibleElement = static_cast<int>(children.size() - 1);
		for (unsigned i = 0; i < children.size(); ++i) {
			GuiElement* element = getElement(children[i]);
			if (!element) continue;
			if (element->getPositionY() + element->getHeight() >= currentVerticalOffset) {
				firstVisibleElement = i;
				break;
			}
		}
		/// Compute last visible element
		lastVisibleElement = static_cast<int>(children.size() - 1);
		for (unsigned i = firstVisibleElement + 1; i < children.size(); ++i) {
			GuiElement* element = getElement(children[i]);
			if (!element) continue;
			if (element->getPositionY() > currentVerticalOffset + getHeight()) {
				lastVisibleElement = i - 1;
				break;
			}
		}
	}

	void GuiVerticalScrollingListLayout::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		drawBackground(worldPosition);
		pushClippingBox(worldPosition);
		// Draw children
		const auto& children = getChildren();
		for (unsigned int i = firstVisibleElement; i < std::min(static_cast<std::size_t>(lastVisibleElement + 1), children.size()); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) drawElement(children[i], worldPosition + child->getPosition() - Vec2i(0, static_cast<int>(currentVerticalOffset)));
		}
		// Draw scrollBar
		if (drawScrollBar) {
			Shader& scrollbarShader = getBackgroundShader();
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			scrollbarShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(scrollbarShader);
			scrollbarShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollbarBackground);
			scrollbarShader.setUniform<Color4f>("u_color", scrollbarBackgroundColor);
			Renderer::draw(guiManager->getModelSquare());
			scrollbarShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollbar);
			scrollbarShader.setUniform<Color4f>("u_color", currentScrollbarColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		popClippingBox();
	}

	void GuiVerticalScrollingListLayout::onRegister()
	{
		GuiVerticalListLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_SCROLL_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
	}

	void GuiVerticalScrollingListLayout::enforceLayout()
	{
		GuiVerticalLayout::enforceLayout();
		// Go through all children, positioning them and resizing them to their preferredHeights/minHeights!
		int currentYOffset = getVerticalBorder();
		const auto& children = getChildren();
		for (GuiID childID : children) {
			GuiElement* child = getElement(childID);
			if (!child) continue;			
			int childHeight = child->getPreferredHeight();
			if (childHeight == -1) childHeight = child->getMinHeight();
			if (childHeight == -1) childHeight = 0;
			// Set position
			setPositionYOfChild(childID, currentYOffset);
			// Resize
			setHeightOfChild(childID, childHeight);
			// Advance to next element
			currentYOffset += childHeight + getVerticalBorder();
		}
		currentVerticalHeight = currentYOffset;
		computeScrollBar();
	}

	Vec2i GuiVerticalScrollingListLayout::getChildOffset(const GuiID& childID) const
	{
		GuiElement* child = getElement(childID);
		if (!child) return Vec2i{};
		Vec2i result = child->getPosition();
		result.y += static_cast<int>(std::floor(currentVerticalOffset));
		return result;
	}

	GuiElement::IsCollidingResult GuiVerticalScrollingListLayout::isColliding(const Vec2i& offset) const
	{
		if (offset.x >= 0 && offset.x <= getWidth() &&
			offset.y >= 0 && offset.y <= getHeight()) {
			if (drawScrollBar) {
				// Check if we are colliding with the scroll bar
				Vec2i size = getSize();
				int scrollBarBackgroundHeight = size.y - 2 * static_cast<int>(scrollbarBorder);
				if (offset.x >= size.x - static_cast<int>(scrollbarBorder) - static_cast<int>(scrollbarWidth) &&
					offset.x <= size.x - static_cast<int>(scrollbarBorder) &&
					offset.y >= static_cast<int>(scrollbarBorder) &&
					offset.y <= static_cast<int>(scrollbarBorder) + scrollBarBackgroundHeight) {
					return IsCollidingResult::COLLIDE_STRONG;
				}
			}
			return IsCollidingResult::COLLIDE_CHILD;
		}
		return IsCollidingResult::NOT_COLLIDING;
	}

	GuiElement::GuiID GuiVerticalScrollingListLayout::getCollidingChild(const Vec2i& offset) const
	{
		return GuiLayout::getCollidingChild(Vec2i(offset.x, offset.y + static_cast<int>(std::floor(currentVerticalOffset))));
	}

	void GuiVerticalScrollingListLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	void GuiVerticalScrollingListLayout::callbackMouseMotion(const Vec2d& position)
	{
		Vec2i mouseOffset = getMouseOffset();
		Vec2i size = getSize();
		int scrollBarBackgroundHeight = size.y - 2 * static_cast<int>(scrollbarBorder);
		if (scrollBarBackgroundHeight == 0.0f) return;
		offsetPercentage = (mouseOffset.y - mouseOffsetFromScrollBar - scrollbarBorder) / scrollBarBackgroundHeight;
		currentVerticalOffset = static_cast<int>(currentVerticalHeight * offsetPercentage);
		computeScrollBar();
	}

	void GuiVerticalScrollingListLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i mouseOffset = getMouseOffset();
			Vec2i size = getSize();
			int scrollBarBackgroundHeight = size.y - 2 * static_cast<int>(scrollbarBorder);
			if (mouseOffset.y >= scrollbarBorder + scrollBarBackgroundHeight * offsetPercentage &&
				mouseOffset.y <= scrollbarBorder + scrollBarBackgroundHeight * (offsetPercentage + visiblePercentage)) {

				mouseOffsetFromScrollBar = mouseOffset.y - scrollbarBorder - scrollBarBackgroundHeight * offsetPercentage;
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
			else {
				offsetPercentage = (mouseOffset.y - scrollbarBorder) / scrollBarBackgroundHeight - visiblePercentage / 2.0f;
				computeScrollBar();
				mouseOffsetFromScrollBar = mouseOffset.y - scrollbarBorder - scrollBarBackgroundHeight * offsetPercentage;
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
		}
	}

	void GuiVerticalScrollingListLayout::callbackMouseScrollOnElement(const Vec2d& offset)
	{
		currentVerticalOffset -= static_cast<int>(offset.y * static_cast<double>(scrollSpeed));
		if (currentVerticalHeight == 0.0f) offsetPercentage = 0.0f;
		else offsetPercentage = currentVerticalOffset / static_cast<float>(currentVerticalHeight);
		computeScrollBar();
	}

	void GuiVerticalScrollingListLayout::callbackMouseEnter(const Vec2d& position)
	{
		infoLogger << LOGGER::BEGIN << "Entered!" << LOGGER::ENDL;
	}

	void GuiVerticalScrollingListLayout::callbackMouseLeave(const Vec2d& position)
	{
		infoLogger << LOGGER::BEGIN << "Left!" << LOGGER::ENDL;
	}
}