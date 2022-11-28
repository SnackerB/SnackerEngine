#include "Gui/Layouts/HorizontalLayout.h"
#include "Core/Keys.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{

	void HorizontalLayout::removeChild(GuiElement& guiElement)
	{
		const auto& children = getChildren();
		auto result = std::find(children.begin(), children.end(), guiElement.getGuiID());
		if (result != children.end()) {
			GuiLayout::removeChild(guiElement);
			// Compute weights from percentages for all children
			computeWeightsFromPercentages();
			// Delete child
			totalWeight -= weights[result - children.begin()];
			weights.erase(weights.begin() + (result - children.begin()));
			percentages.erase(weights.begin() + (result - children.begin()));
			// Compute percentages from weights for all children
			computePercentagesFromWeights();
			enforceLayout();
		}
	}
	
	void HorizontalLayout::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i mySize = getSize();
		const auto& children = getChildren();
		for (unsigned int i = 0; i < children.size(); ++i) {
			int positionX = i > 0 ? static_cast<int>(static_cast<double>(mySize.x) * percentages[i - 1])
				: 0;
			int width = i == 0 ? static_cast<int>(static_cast<double>(mySize.x) * percentages[0])
				: static_cast<int>(static_cast<double>(mySize.x) * (percentages[i] - percentages[i - 1]));
			if (forceHeight) {
				setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(positionX, 0), Vec2i(width, mySize.y));
			}
			else {
				setPositionXWithoutEnforcingLayouts(children[i], positionX);
				setWidthWithoutEnforcingLayouts(children[i], width);
			}
			enforceLayoutOnElement(children[i]);
		}
	}
	
	std::optional<std::pair<unsigned int, int>> HorizontalLayout::getCollidingBorderAndOffset(const Vec2i& position)
	{
		int width = getSize().x;
		// Start from the left
		for (unsigned int i = 0; i < percentages.size() - 1; ++i) {
			int currentX = static_cast<int>(static_cast<double>(width) * percentages[i]);
			if (((currentX - static_cast<int>(resizeAreaWidth)) <= position.x) && (position.x <= (currentX + static_cast<int>(resizeAreaWidth)))) {
				return std::make_pair(i, position.x - currentX);
			}
		}
		return {};
	}
	
	HorizontalLayout::IsCollidingResult HorizontalLayout::isColliding(const Vec2i& position)
	{
		if (position.y < getPositionY() || position.y > getPositionY() + getHeight()) return IsCollidingResult::NOT_COLLIDING;
		return getCollidingBorderAndOffset(position) ? IsCollidingResult::COLLIDE_STRONG : IsCollidingResult::COLLIDE_CHILD;
	}

	void HorizontalLayout::onRegister()
	{
		GuiLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
	}

	void HorizontalLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	void HorizontalLayout::callbackMouseMotion(const Vec2d& position)
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
		// Compute new percentages. We only need to change the percentage of the element in front of the resize border
		percentages[resizeBorder] = getSize().x == 0 ? 0.0 : static_cast<double>(borderPos) / static_cast<double>(getSize().x);
		enforceLayout();
	}

	void HorizontalLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i offset = getMouseOffset(getGuiID());
			auto result = getCollidingBorderAndOffset(offset);
			if (result) {
				resizeBorder = result->first;
				mouseOffset = result->second;
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
		}
	}

	void HorizontalLayout::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::HRESIZE);
	}

	void HorizontalLayout::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}

	void HorizontalLayout::computePercentagesFromWeights()
	{
		double cumulativePercentage = 0.0;
		for (unsigned int i = 0; i < weights.size(); ++i) {
			cumulativePercentage += weights[0] / totalWeight;
			percentages[i] = cumulativePercentage;
		}
	}

	void HorizontalLayout::computeWeightsFromPercentages()
	{
		if (!weights.empty()) weights[0] = percentages[0] * totalWeight;
		for (unsigned int i = 1; i < weights.size(); ++i) {
			weights[i] = (percentages[i] - percentages[i - 1]) * totalWeight;
		}
	}

	HorizontalLayout::HorizontalLayout(const bool& forceHeight)
		: GuiLayout(), totalWeight(0.0), forceHeight(forceHeight), resizeAreaWidth(5), mouseOffset(0),
		resizeBorder(0), weights(0.0), percentages(0.0) {}

	bool HorizontalLayout::registerChild(GuiElement& guiElement, const double& weight)
	{
		if (GuiLayout::registerChildWithoutEnforcingLayouts(guiElement)) {
			// Compute weights from percentages for all children
			computeWeightsFromPercentages();
			// push back new weight
			weights.push_back(weight);
			percentages.push_back(0.0);
			totalWeight += weight;
			// Compute percentages from weights for all children
			computePercentagesFromWeights();
			enforceLayout();
			return true;
		}
		return false;
	}

	bool HorizontalLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, 0.0);
	}

	HorizontalLayout::HorizontalLayout(const HorizontalLayout& other) noexcept
		: GuiLayout(other), totalWeight(0.0), forceHeight(other.forceHeight), resizeAreaWidth(other.resizeAreaWidth), mouseOffset(0),
		resizeBorder(0), weights{}, percentages{} {}

	HorizontalLayout& HorizontalLayout::operator=(const HorizontalLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		totalWeight = 0.0;
		forceHeight = other.forceHeight;
		resizeAreaWidth = other.resizeAreaWidth;
		mouseOffset = 0;
		resizeBorder = 0;  
		weights.clear();
		percentages.clear();
		return *this;
	}

	HorizontalLayout::HorizontalLayout(HorizontalLayout&& other) noexcept
		: GuiLayout(std::move(other)), totalWeight(other.totalWeight), forceHeight(other.forceHeight), resizeAreaWidth(other.resizeAreaWidth), 
		mouseOffset(other.mouseOffset), resizeBorder(other.resizeBorder), weights(other.weights), percentages(other.percentages) {}

	HorizontalLayout& HorizontalLayout::operator=(HorizontalLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
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