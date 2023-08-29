#include "Gui/Layouts/VerticalWeightedLayout.h"
#include "Graphics/Renderer.h"
#include "Core/Keys.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::stopResizing()
	{
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::computePercentagesFromWeights()
	{
		if (totalWeight != 0) {
			for (unsigned int i = 0; i < weights.size(); ++i) {
				percentages[i] = weights[i] / totalWeight;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::computeWeightsFromPercentages()
	{
		for (unsigned int i = 1; i < weights.size(); ++i) {
			weights[i] = percentages[i] * totalWeight;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::onRegisterChild(double weight)
	{
		// Compute weights from percentages for all children
		computeWeightsFromPercentages();
		// push back new weight
		weights.push_back(weight);
		percentages.push_back(0.0);
		totalWeight += weight;
		// Compute percentages from weights for all children
		computePercentagesFromWeights();
		registerEnforceLayoutDown();
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalWeightedLayout::GuiVerticalWeightedLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVerticalLayout(json, data, parameterNames)
	{
		if (!json.contains("verticalLayoutMode")) setVerticalLayoutMode(VerticalLayoutMode::FORCE_CHILD_WIDTH);
		parseJsonOrReadFromData(verticalBorder, "verticalBorder", json, data);
		parseJsonOrReadFromData(alignmentVertical, "alignmentVertical", json, data, parameterNames);
		parseJsonOrReadFromData(resizeAreaHeight, "resizeAreaHeight", json, data, parameterNames);
		parseJsonOrReadFromData(allowMoveBorders, "allowMoveBorders", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalWeightedLayout::registerChild(GuiElement& guiElement, double weight)
	{
		if (GuiVerticalLayout::registerChild(guiElement)) {
			onRegisterChild(weight);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalWeightedLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, 1.0);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalWeightedLayout::registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		if (GuiVerticalLayout::registerChild(guiElement, json, data, parameterNames)) {
			double weight = 1.0;
			parseJsonOrReadFromData(weight, "weight", json, data, parameterNames);
			onRegisterChild(weight);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<double> GuiVerticalWeightedLayout::getWeight(GuiID childID) const
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		return index.has_value() ? weights[index.value()] : std::optional<double>();
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<double> GuiVerticalWeightedLayout::getPercentage(GuiID childID) const
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		return index.has_value() ? percentages[index.value()] : std::optional<double>();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::setWeight(GuiID childID, double weight)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		if (index.has_value() && weights[index.value()] != weight) {
			weights[index.value()] = weight;
			computePercentagesFromWeights();
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::setPercentage(GuiID childID, double percentage)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		if (index.has_value() && percentages[index.value()] != percentage) {
			percentages[index.value()] = percentage;
			computeWeightsFromPercentages();
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::setVerticalBorder(int verticalBorder)
	{
		if (this->verticalBorder != verticalBorder) {
			this->verticalBorder = verticalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::setAlignmentVertical(AlignmentVertical alignmentVertical)
	{
		if (this->alignmentVertical != alignmentVertical) {
			this->alignmentVertical = alignmentVertical;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::setAllowMoveBorders(bool allowMoveBorders)
	{
		if (isResizing) stopResizing();
		if (!allowMoveBorders) Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
		this->allowMoveBorders = allowMoveBorders;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::onRegister()
	{
		GuiLayout::onRegister();
		if (allowMoveBorders) {
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<unsigned> GuiVerticalWeightedLayout::removeChild(GuiID guiElement)
	{
		std::optional<unsigned> index = GuiVerticalLayout::removeChild(guiElement);
		if (index.has_value()) {
			// Compute weights from percentages for all children
			computeWeightsFromPercentages();
			// Delete child
			totalWeight -= weights[index.value()];
			weights.erase(weights.begin() + index.value());
			percentages.erase(percentages.begin() + index.value());
			// Compute percentages from weights for all children
			computePercentagesFromWeights();
			// Stop resizing
			if (isResizing) stopResizing();
		}
		return index;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::enforceLayout()
	{
		GuiVerticalLayout::enforceLayout();
		int totalRemainingHeight = getHeight() - static_cast<int>(weights.size() + 1) * verticalBorder;
		double remainingPercentage = 1.0f;
		const auto& children = getChildren();
		std::vector<unsigned> heights(children.size());
		std::vector<unsigned> indicesOfChildrenLeftToResize;
		if (alignmentVertical == AlignmentVertical::TOP || alignmentVertical == AlignmentVertical::CENTER) {
			for (unsigned i = 0; i < children.size(); ++i) indicesOfChildrenLeftToResize.push_back(i);
		}
		else {
			for (int i = static_cast<int>(children.size() - 1); i >= 0; --i) indicesOfChildrenLeftToResize.push_back(i);
		}
		// Repeat the resizing process until all children have been resized or no problems occured during the process
		bool problemOccured = true;
		while (!indicesOfChildrenLeftToResize.empty() && problemOccured) {
			problemOccured = false;
			for (auto it = indicesOfChildrenLeftToResize.begin(); it != indicesOfChildrenLeftToResize.end(); ++it) {
				GuiElement* child = getElement(children[*it]);
				if (child) {
					int height = static_cast<int>(percentages[*it] / remainingPercentage * totalRemainingHeight);
					if (child->getMaxHeight() != -1 && height > child->getMaxHeight()) {
						// If the percentage height is too large, we just set the child to its maximum height
						heights[*it] = child->getMaxHeight();
						totalRemainingHeight -= heights[*it];
						remainingPercentage -= percentages[*it];
						indicesOfChildrenLeftToResize.erase(it);
						problemOccured = true;
						break;
					}
					else if (height < child->getMinHeight()) {
						// If the percentage height is too small, we just set the child to its minimum height
						heights[*it] = child->getMinHeight();
						totalRemainingHeight -= heights[*it];
						remainingPercentage -= percentages[*it];
						indicesOfChildrenLeftToResize.erase(it);
						problemOccured = true;
						break;
					}
					else {
						// Else, we can set the height to the percentage height!
						heights[*it] = std::max(height, 0);
					}
				}
				else heights[*it] = 0;
			}
		}
		// Now we can actually position and resize the elements
		if (alignmentVertical == AlignmentVertical::TOP || alignmentVertical == AlignmentVertical::CENTER) {
			int position = verticalBorder;
			if (alignmentVertical == AlignmentVertical::CENTER) {
				int totalHeight = verticalBorder * static_cast<int>(children.size() - 1);
				for (auto height : heights) totalHeight += height;
				position = (getHeight() - totalHeight) / 2;
			}
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (child) {
					child->setPositionY(position);
					child->setHeight(heights[i]);
				}
				position += heights[i] + verticalBorder;
			}
		}
		else {
			int position = getHeight() - verticalBorder;
			for (int i = static_cast<int>(children.size() - 1); i >= 0; --i) {
				GuiElement* child = getElement(children[i]);
				position -= heights[i];
				if (child) {
					child->setPositionY(position);
					child->setHeight(heights[i]);
				}
				position -= verticalBorder;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<std::pair<unsigned int, int>> GuiVerticalWeightedLayout::getCollidingBorderAndOffset(const Vec2i& offset) const
	{
		const auto& children = getChildren();
		for (int i = 1; i < children.size(); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) {
				if (child->getPositionY() > offset.y + static_cast<int>(resizeAreaHeight)) return {};
				else if (child->getPositionY() > offset.y - static_cast<int>(resizeAreaHeight)) return { { i - 1, offset.y - child->getPositionY() } };
			}
		}
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiVerticalWeightedLayout::isColliding(const Vec2i& offset) const
	{
		if (offset.x < 0 || offset.x > getWidth()) return IsCollidingResult::NOT_COLLIDING;
		return getCollidingBorderAndOffset(offset) ? IsCollidingResult::COLLIDE_STRONG : IsCollidingResult::COLLIDE_CHILD;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			stopResizing();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::callbackMouseMotion(const Vec2d& position)
	{
		int borderPos = getMouseOffsetY() - mouseOffset;
		GuiElement* topChild = getElement(getChildren()[resizeBorder]);
		GuiElement* bottomChild = getElement(getChildren()[static_cast<std::size_t>(resizeBorder) + 1]);
		int newHeightTop = borderPos - topChild->getPositionY();
		int newHeightBottom = 0;
		if (newHeightTop < topChild->getMinHeight()) {
			// New top height is too small!
			newHeightTop = topChild->getMinHeight();
			newHeightBottom = bottomChild->getPositionY() + bottomChild->getHeight() - topChild->getPositionY() - newHeightTop;
			if (newHeightBottom < bottomChild->getMinHeight() || (bottomChild->getMaxHeight() != -1 && newHeightBottom > bottomChild->getMaxHeight())) return;
		}
		else if (topChild->getMaxHeight() != -1 && newHeightTop > topChild->getMaxHeight()) {
			// New top height is too large!
			newHeightTop = topChild->getMaxHeight();
			newHeightBottom = bottomChild->getPositionY() + bottomChild->getHeight() - topChild->getPositionY() - newHeightTop;
			if (newHeightBottom < bottomChild->getMinHeight() || (bottomChild->getMaxHeight() != -1 && newHeightBottom > bottomChild->getMaxHeight())) return;
		}
		else {
			newHeightBottom = bottomChild->getPositionY() + bottomChild->getHeight() - borderPos;
			if (newHeightBottom < bottomChild->getMinHeight()) {
				// New bottom height is too small!
				newHeightBottom = bottomChild->getMinHeight();
				newHeightTop = bottomChild->getPositionY() + bottomChild->getHeight() - newHeightBottom - topChild->getPositionY();
				if (newHeightTop < topChild->getMinHeight() || (topChild->getMaxHeight() != -1 && newHeightTop > topChild->getMaxHeight())) return;
			}
			else if (bottomChild->getMaxHeight() != -1 && newHeightBottom > bottomChild->getMaxHeight()) {
				// New bottom height is too large!
				newHeightBottom = bottomChild->getMaxHeight();
				newHeightTop = bottomChild->getPositionY() + bottomChild->getHeight() - newHeightBottom - topChild->getPositionY();
				if (newHeightTop < topChild->getMinHeight() || (topChild->getMaxHeight() != -1 && newHeightTop > topChild->getMaxHeight())) return;
			}
		}
		topChild->setHeight(newHeightTop);
		bottomChild->setHeight(newHeightBottom);
		bottomChild->setPositionY(topChild->getPositionY() + newHeightTop);
		// Compute new percentages and weights
		double totalPercentage = percentages[resizeBorder] + percentages[static_cast<std::size_t>(resizeBorder) + 1];
		double totalHeight = static_cast<double>(newHeightTop + newHeightBottom);
		percentages[resizeBorder] = static_cast<double>(newHeightTop) / totalHeight * totalPercentage;
		percentages[static_cast<std::size_t>(resizeBorder) + 1] = static_cast<double>(newHeightBottom) / totalHeight * totalPercentage;
		weights[resizeBorder] = getTotalWeight() * percentages[resizeBorder];
		weights[static_cast<std::size_t>(resizeBorder) + 1] = getTotalWeight() * percentages[static_cast<std::size_t>(resizeBorder) + 1];
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i offset = getMouseOffset();
			auto result = getCollidingBorderAndOffset(offset);
			if (result) {
				resizeBorder = result->first;
				mouseOffset = result->second;
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::VRESIZE);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalWeightedLayout::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}
	//--------------------------------------------------------------------------------------------------
}