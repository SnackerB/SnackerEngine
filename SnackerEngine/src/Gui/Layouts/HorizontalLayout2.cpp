#include "Gui/Layouts/HorizontalLayout2.h"
#include "Core/Keys.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{

	void HorizontalLayout2::removeChild(GuiElement2& guiElement)
	{
		const auto& children = getChildren();
		auto result = std::find(children.begin(), children.end(), guiElement.getGuiID());
		if (result != children.end()) {
			GuiLayout2::removeChild(guiElement);
			// Compute weights from percentages for all children
			for (unsigned int i = 0; i < weights.size(); ++i) {
				weights[i] = percentages[i] * totalWeight;
			}
			totalWeight -= weights[result - children.begin()];
			weights.erase(weights.begin() + (result - children.begin()));
			percentages.erase(weights.begin() + (result - children.begin()));
			// Compute percentages from weights for all children
			for (unsigned int i = 0; i < weights.size(); ++i) {
				percentages[i] = weights[i] / totalWeight;
			}
			enforceLayout();
		}
	}
	
	void HorizontalLayout2::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i mySize = getSize();
		unsigned int remainder = mySize.x;
		const auto& children = getChildren();
		for (unsigned int i = 0; i < children.size(); ++i) {
			Vec2i position = Vec2i(mySize.x - remainder, 0);
			Vec2i size = (i == children.size() - 1) ? Vec2i(remainder, mySize.y)
				: Vec2i(static_cast<int>(static_cast<double>(mySize.x) * percentages[i]), mySize.y);
			remainder -= size.x;
			setPositionAndSizeWithoutEnforcingLayouts(children[i], position, size);
			enforceLayoutOnElement(children[i]);
		}
	}
	
	std::optional<std::pair<unsigned int, int>> HorizontalLayout2::getCollidingBorderAndOffset(const Vec2i& position)
	{
		int width = getSize().x;
		// Start from the left
		int currentX = 0;
		for (unsigned int i = 0; i < percentages.size() - 1; ++i) {
			currentX += static_cast<int>(static_cast<float>(width) * percentages[i]);
			if (((currentX - static_cast<int>(resizeAreaWidth)) <= position.x) && (position.x <= (currentX + static_cast<int>(resizeAreaWidth)))) {
				return std::make_pair(i, position.x - currentX);
			}
		}
		return {};
	}
	
	HorizontalLayout2::IsCollidingResult HorizontalLayout2::isColliding(const Vec2i& position)
	{
		if (position.y < getPositionY() || position.y > getPositionY() + getHeight()) return IsCollidingResult::NOT_COLLIDING;
		return getCollidingBorderAndOffset(position) ? IsCollidingResult::COLLIDE_STRONG : IsCollidingResult::COLLIDE_CHILD;
	}

	void HorizontalLayout2::onRegister()
	{
		GuiLayout2::onRegister();
		signUpEvent(SnackerEngine::GuiElement2::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement2::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement2::CallbackType::MOUSE_LEAVE);
	}

	void HorizontalLayout2::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement2::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement2::CallbackType::MOUSE_BUTTON);
		}
	}

	void HorizontalLayout2::callbackMouseMotion(const Vec2d& position)
	{
		int borderPos = getMouseOffset(getGuiID()).x - mouseOffset;
		const auto& children = getChildren();
		if (borderPos < getPosition(children[resizeBorder]).x) {
			borderPos = getPosition(children[resizeBorder]).x;
		}
		else if (resizeBorder + 2 < children.size() && borderPos > getPosition(children[resizeBorder + 2]).x) {
			borderPos = getPosition(children[resizeBorder + 2]).x;
		}
		else if (borderPos > getSize().x) {
			borderPos = getSize().x;
		}
		// Compute new percentages. We only need to change the percentage of the element left and right of the resize border
		int totalWidth = resizeBorder >= children.size() - 2 ? getSize().x
			: getPosition(children[resizeBorder + 2]).x;
		totalWidth -= getPosition(children[resizeBorder]).x;
		percentages[resizeBorder] = static_cast<double>(borderPos - getPosition(children[resizeBorder]).x) / static_cast<double>(getSize(getParentID()).x);
		if (resizeBorder + 2 < children.size()) {
			percentages[resizeBorder + 1] = static_cast<double>(getPosition(children[resizeBorder + 2]).x - borderPos) / static_cast<double>(getSize(getParentID()).x);
		}
		else {
			percentages[resizeBorder + 1] = static_cast<double>(getSize(getParentID()).x - getPosition(children[resizeBorder + 1]).x - borderPos) / static_cast<double>(getSize(getParentID()).x);
		}
		enforceLayout();
	}

	void HorizontalLayout2::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i offset = getMouseOffset(getGuiID());
			auto result = getCollidingBorderAndOffset(offset);
			if (result) {
				resizeBorder = result->first;
				mouseOffset = result->second;
				signUpEvent(SnackerEngine::GuiElement2::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement2::CallbackType::MOUSE_BUTTON);
			}
		}
	}

	void HorizontalLayout2::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::HRESIZE);
	}

	void HorizontalLayout2::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}

	HorizontalLayout2::HorizontalLayout2(const bool& forceHeight)
		: GuiLayout2(), totalWeight(0.0), forceHeight(forceHeight), resizeAreaWidth(20), mouseOffset(0),
		resizeBorder(0), weights(0.0), percentages(0.0) {}

	bool HorizontalLayout2::registerChild(GuiElement2& guiElement, const double& weight)
	{
		if (GuiLayout2::registerChildWithoutEnforcingLayouts(guiElement)) {
			// Compute weights from percentages for all children
			for (unsigned int i = 0; i < weights.size(); ++i) {
				weights[i] = percentages[i] * totalWeight;
			}
			weights.push_back(weight);
			percentages.push_back(0.0);
			totalWeight += weight;
			// Compute percentages from weights for all children
			for (unsigned int i = 0; i < weights.size(); ++i) {
				percentages[i] = weights[i] / totalWeight;
			}
			enforceLayout();
			return true;
		}
		return false;
	}

	bool HorizontalLayout2::registerChild(GuiElement2& guiElement)
	{
		return registerChild(guiElement, 0.0);
	}

	HorizontalLayout2::HorizontalLayout2(const HorizontalLayout2& other) noexcept
		: GuiLayout2(other), totalWeight(0.0), forceHeight(other.forceHeight), resizeAreaWidth(other.resizeAreaWidth), mouseOffset(0),
		resizeBorder(0), weights{}, percentages{} {}

	HorizontalLayout2& HorizontalLayout2::operator=(const HorizontalLayout2& other) noexcept
	{
		GuiLayout2::operator=(other);
		totalWeight = 0.0;
		forceHeight = other.forceHeight;
		resizeAreaWidth = other.resizeAreaWidth;
		mouseOffset = 0;
		resizeBorder = 0;  
		weights.clear();
		percentages.clear();
		return *this;
	}

	HorizontalLayout2::HorizontalLayout2(HorizontalLayout2&& other) noexcept
		: GuiLayout2(std::move(other)), totalWeight(other.totalWeight), forceHeight(other.forceHeight), resizeAreaWidth(other.resizeAreaWidth), 
		mouseOffset(other.mouseOffset), resizeBorder(other.resizeBorder), weights(other.weights), percentages(other.percentages) {}

	HorizontalLayout2& HorizontalLayout2::operator=(HorizontalLayout2&& other) noexcept
	{
		GuiLayout2::operator=(std::move(other));
		totalWeight = other.totalWeight;
		forceHeight = other.forceHeight;
		resizeAreaWidth = other.resizeAreaWidth;
		mouseOffset = other.mouseOffset;
		resizeBorder = other.resizeBorder;
		weights = other.weights;
		percentages = other.percentages;
		return *this;
	}

}