#include "Gui/Layouts/VerticalLayout.h"
#include "Core/Keys.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{

	void VerticalLayout::removeChild(GuiElement& guiElement)
	{
		const auto& children = getChildren();
		auto result = std::find(children.begin(), children.end(), guiElement.getGuiID());
		if (result != children.end()) {
			GuiLayout::removeChild(guiElement);
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

	void VerticalLayout::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i mySize = getSize();
		unsigned int remainder = mySize.y;
		const auto& children = getChildren();
		for (unsigned int i = 0; i < children.size(); ++i) {
			Vec2i position = Vec2i(0, mySize.y - remainder);
			Vec2i size = (i == children.size() - 1) ? Vec2i(mySize.x, remainder)
				: Vec2i(mySize.x, static_cast<int>(static_cast<double>(mySize.y) * percentages[i]));
			remainder -= size.y;
			setPositionAndSizeWithoutEnforcingLayouts(children[i], position, size);
			enforceLayoutOnElement(children[i]);
		}
	}

	std::optional<std::pair<unsigned int, int>> VerticalLayout::getCollidingBorderAndOffset(const Vec2i& position)
	{
		int height = getSize().y;
		// Start from the left
		int currentY = 0;
		for (unsigned int i = 0; i < percentages.size() - 1; ++i) {
			currentY += static_cast<int>(static_cast<float>(height) * percentages[i]);
			if (((currentY - static_cast<int>(resizeAreaHeight)) <= position.y) && (position.y <= (currentY + static_cast<int>(resizeAreaHeight)))) {
				return std::make_pair(i, position.y - currentY);
			}
		}
		return {};
	}

	VerticalLayout::IsCollidingResult VerticalLayout::isColliding(const Vec2i& position)
	{
		if (position.x < getPositionX() || position.x > getPositionX() + getWidth()) return IsCollidingResult::NOT_COLLIDING;
		return getCollidingBorderAndOffset(position) ? IsCollidingResult::COLLIDE_STRONG : IsCollidingResult::COLLIDE_CHILD;
	}

	void VerticalLayout::onRegister()
	{
		GuiLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
	}

	void VerticalLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	void VerticalLayout::callbackMouseMotion(const Vec2d& position)
	{
		int borderPos = getMouseOffset(getGuiID()).y - mouseOffset;
		const auto& children = getChildren();
		if (borderPos < getPosition(children[resizeBorder]).y) {
			borderPos = getPosition(children[resizeBorder]).y;
		}
		else if (resizeBorder + 2 < children.size() && borderPos > getPosition(children[resizeBorder + 2]).y) {
			borderPos = getPosition(children[resizeBorder + 2]).y;
		}
		else if (borderPos > getSize().y) {
			borderPos = getSize().y;
		}
		// Compute new percentages. We only need to change the percentage of the element left and right of the resize border
		int totalWidth = resizeBorder >= children.size() - 2 ? getSize().y
			: getPosition(children[resizeBorder + 2]).y;
		totalWidth -= getPosition(children[resizeBorder]).y;
		percentages[resizeBorder] = static_cast<double>(borderPos - getPosition(children[resizeBorder]).y) / static_cast<double>(getSize(getParentID()).y);
		if (resizeBorder + 2 < children.size()) {
			percentages[resizeBorder + 1] = static_cast<double>(getPosition(children[resizeBorder + 2]).y - borderPos) / static_cast<double>(getSize(getParentID()).y);
		}
		else {
			percentages[resizeBorder + 1] = static_cast<double>(getSize(getParentID()).y - getPosition(children[resizeBorder + 1]).y - borderPos) / static_cast<double>(getSize(getParentID()).y);
		}
		enforceLayout();
	}

	void VerticalLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
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

	void VerticalLayout::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::VRESIZE);
	}

	void VerticalLayout::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}

	VerticalLayout::VerticalLayout(const bool& forceWidth)
		: GuiLayout(), totalWeight(0.0), forceWidth(forceWidth), resizeAreaHeight(5), mouseOffset(0),
		resizeBorder(0), weights(0.0), percentages(0.0) {}

	bool VerticalLayout::registerChild(GuiElement& guiElement, const double& weight)
	{
		if (GuiLayout::registerChildWithoutEnforcingLayouts(guiElement)) {
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

	bool VerticalLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, 0.0);
	}

	VerticalLayout::VerticalLayout(const VerticalLayout& other) noexcept
		: GuiLayout(other), totalWeight(0.0), forceWidth(other.forceWidth), resizeAreaHeight(other.resizeAreaHeight), mouseOffset(0),
		resizeBorder(0), weights{}, percentages{} {}

	VerticalLayout& VerticalLayout::operator=(const VerticalLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		totalWeight = 0.0;
		forceWidth = other.forceWidth;
		resizeAreaHeight = other.resizeAreaHeight;
		mouseOffset = 0;
		resizeBorder = 0;
		weights.clear();
		percentages.clear();
		return *this;
	}

	VerticalLayout::VerticalLayout(VerticalLayout&& other) noexcept
		: GuiLayout(std::move(other)), totalWeight(other.totalWeight), forceWidth(other.forceWidth), resizeAreaHeight(other.resizeAreaHeight),
		mouseOffset(other.mouseOffset), resizeBorder(other.resizeBorder), weights(other.weights), percentages(other.percentages) {}

	VerticalLayout& VerticalLayout::operator=(VerticalLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		totalWeight = other.totalWeight;
		forceWidth = other.forceWidth;
		resizeAreaHeight = other.resizeAreaHeight;
		mouseOffset = other.mouseOffset;
		resizeBorder = other.resizeBorder;
		weights = other.weights;
		percentages = other.percentages;
		return *this;
	}

}