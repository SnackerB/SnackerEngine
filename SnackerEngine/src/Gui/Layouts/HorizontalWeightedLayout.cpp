#include "Gui/Layouts/HorizontalWeightedLayout.h"
#include "Graphics/Renderer.h"
#include "Core/Keys.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::stopResizing()
	{
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::computePercentagesFromWeights()
	{
		if (totalWeight != 0) {
			for (unsigned int i = 0; i < weights.size(); ++i) {
				percentages[i] = weights[i] / totalWeight;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::computeWeightsFromPercentages()
	{
		for (unsigned int i = 1; i < weights.size(); ++i) {
			weights[i] = percentages[i] * totalWeight;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::onRegisterChild(double weight)
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
	void GuiHorizontalWeightedLayout::computeWidthHintsFromChildren()
	{
		if (getResizeMode() != ResizeMode::RESIZE_RANGE) return;
		int totalMinWidth = 0;
		int totalPreferredWidth = SIZE_HINT_ARBITRARY;
		for (auto childID : getChildren()) {
			GuiElement* child = getElement(childID);
			if (child) {
				totalMinWidth += child->getMinWidth();
				if (totalPreferredWidth != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					int tempPreferredWidth = child->getPreferredWidth();
					if (tempPreferredWidth >= 0) {
						if (totalPreferredWidth >= 0) totalPreferredWidth += tempPreferredWidth;
						else totalPreferredWidth = tempPreferredWidth;
					}
					else if (tempPreferredWidth == SIZE_HINT_AS_LARGE_AS_POSSIBLE) totalPreferredWidth = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				}
			}
		}
		int totalBorders = 2 * outerHorizontalBorder + static_cast<int>((getChildren().size() - 1)) * horizontalBorder;
		totalMinWidth += totalBorders;
		if (totalPreferredWidth >= 0) totalPreferredWidth += totalBorders;
		setMinWidth(totalMinWidth);
		setPreferredWidth(totalPreferredWidth);
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalWeightedLayout::GuiHorizontalWeightedLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiHorizontalLayout(json, data, parameterNames)
	{
		if (!json.contains("horizontalLayoutMode")) setHorizontalLayoutMode(HorizontalLayoutMode::CHILD_HEIGHT_TO_LAYOUT_HEIGHT);
		parseJsonOrReadFromData(horizontalBorder, "horizontalBorder", json, data);
		parseJsonOrReadFromData(outerHorizontalBorder, "outerHorizontalBorder", json, data);
		parseJsonOrReadFromData(alignmentHorizontal, "alignmentHorizontal", json, data, parameterNames);
		parseJsonOrReadFromData(resizeAreaWidth, "resizeAreaWidth", json, data, parameterNames);
		parseJsonOrReadFromData(allowMoveBorders, "allowMoveBorders", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalWeightedLayout::registerChild(GuiElement& guiElement, double weight)
	{
		if (GuiHorizontalLayout::registerChild(guiElement)) {
			onRegisterChild(weight);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalWeightedLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, 1.0);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalWeightedLayout::registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		if (GuiHorizontalLayout::registerChild(guiElement, json, data, parameterNames)) {
			double weight = 1.0;
			parseJsonOrReadFromData(weight, "weight", json, data, parameterNames);
			onRegisterChild(weight);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<double> GuiHorizontalWeightedLayout::getWeight(GuiID childID) const
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		return index.has_value() ? weights[index.value()] : std::optional<double>();
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<double> GuiHorizontalWeightedLayout::getPercentage(GuiID childID) const
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		return index.has_value() ? percentages[index.value()] : std::optional<double>();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::setWeight(GuiID childID, double weight)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		if (index.has_value() && weights[index.value()] != weight) {
			weights[index.value()] = weight;
			computePercentagesFromWeights();
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::setPercentage(GuiID childID, double percentage)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		if (index.has_value() && percentages[index.value()] != percentage) {
			percentages[index.value()] = percentage;
			computeWeightsFromPercentages();
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::setHorizontalBorder(int horizontalBorder)
	{
		if (this->horizontalBorder != horizontalBorder) {
			this->horizontalBorder = horizontalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::setOuterHorizontalBorder(int outerHorizontalBorder)
	{
		if (this->outerHorizontalBorder != outerHorizontalBorder) {
			this->outerHorizontalBorder = outerHorizontalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::setAlignmentHorizontal(AlignmentHorizontal alignmentHorizontal)
	{
		if (this->alignmentHorizontal != alignmentHorizontal) {
			this->alignmentHorizontal = alignmentHorizontal;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::setAllowMoveBorders(bool allowMoveBorders)
	{
		if (isResizing) stopResizing();
		if (!allowMoveBorders) Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
		this->allowMoveBorders = allowMoveBorders;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::onRegister()
	{
		GuiLayout::onRegister();
		if (allowMoveBorders) {
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<unsigned> GuiHorizontalWeightedLayout::removeChild(GuiID guiElement)
	{
		std::optional<unsigned> index = GuiHorizontalLayout::removeChild(guiElement);
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
	void GuiHorizontalWeightedLayout::enforceLayout()
	{
		GuiHorizontalLayout::enforceLayout();
		unsigned totalRemainingWidth = getWidth() - static_cast<int>(weights.size() == 0 ? 0 : weights.size() - 1) * horizontalBorder - 2 * outerHorizontalBorder;
		double remainingPercentage = 1.0f;
		const auto& children = getChildren();
		std::vector<int> widths(children.size());
		std::vector<unsigned> indicesOfChildrenLeftToResize;
		if (alignmentHorizontal == AlignmentHorizontal::LEFT || alignmentHorizontal == AlignmentHorizontal::CENTER) {
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
					int width = static_cast<int>(percentages[*it] / remainingPercentage * static_cast<double>(totalRemainingWidth));
					if (child->getMaxWidth() >= 0 && width > child->getMaxWidth()) {
						// If the percentage width is too large, we just set the child to its maximum width
						widths[*it] = child->getMaxWidth();
						totalRemainingWidth -= widths[*it];
						remainingPercentage -= percentages[*it];
						indicesOfChildrenLeftToResize.erase(it);
						problemOccured = true;
						break;
					}
					else if (width < child->getMinWidth()) {
						// If the percentage width is too small, we just set the child to its minimum width
						widths[*it] = child->getMinWidth();
						totalRemainingWidth -= widths[*it];
						remainingPercentage -= percentages[*it];
						indicesOfChildrenLeftToResize.erase(it);
						problemOccured = true;
						break;
					}
					else {
						// Else, we can set the width to the percentage width!
						widths[*it] = std::max(width, 0);
					}
				}
				else widths[*it] = 0;
			}
		}
		// Now we can actually position and resize the elements
		if (alignmentHorizontal == AlignmentHorizontal::LEFT || alignmentHorizontal == AlignmentHorizontal::CENTER) {
			int position = outerHorizontalBorder;
			if (alignmentHorizontal == AlignmentHorizontal::CENTER) {
				int totalWidth = horizontalBorder * static_cast<int>((children.size() - 1));
				for (auto width : widths) totalWidth += width;
				position = (getWidth() - totalWidth) / 2;
			}
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (child) {
					child->setPositionX(position);
					child->setWidth(widths[i]);
				}
				position += widths[i] + horizontalBorder;
			}
		}
		else {
			int position = getWidth() - outerHorizontalBorder;
			for (int i = static_cast<int>(children.size() - 1); i >= 0; --i) {
				GuiElement* child = getElement(children[i]);
				position -= widths[i];
				if (child) {
					child->setPositionX(position);
					child->setWidth(widths[i]);
				}
				position -= horizontalBorder;
			}
		}
		// Compute the min width of this layout from its children
		computeWidthHintsFromChildren();
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<std::pair<unsigned int, int>> GuiHorizontalWeightedLayout::getCollidingBorderAndOffset(const Vec2i& offset) const
	{
		const auto& children = getChildren();
		for (unsigned i = 1; i < children.size(); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) {
				if (child->getPositionX() > offset.x + resizeAreaWidth) return {};
				else if (child->getPositionX() > offset.x - resizeAreaWidth) return { { i - 1, offset.x - child->getPositionX() } };
			}
		}
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiHorizontalWeightedLayout::isColliding(const Vec2i& offset) const
	{
		if (offset.y < 0 || offset.y > getHeight()) return IsCollidingResult::NOT_COLLIDING;
		return getCollidingBorderAndOffset(offset) ? IsCollidingResult::COLLIDE_STRONG : IsCollidingResult::COLLIDE_CHILD;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			stopResizing();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::callbackMouseMotion(const Vec2d& position)
	{
		int borderPos = getMouseOffsetX() - mouseOffset;
		GuiElement* leftChild = getElement(getChildren()[resizeBorder]);
		GuiElement* rightChild = getElement(getChildren()[resizeBorder + 1]);
		int newWidthLeft = borderPos - leftChild->getPositionX();
		int newWidthRight = 0;
		if (newWidthLeft < leftChild->getMinWidth()) {
			// New left width is too small!
			newWidthLeft = leftChild->getMinWidth();
			newWidthRight = rightChild->getPositionX() + rightChild->getWidth() - leftChild->getPositionX() - newWidthLeft;
			if (newWidthRight < rightChild->getMinWidth() || (rightChild->getMaxWidth() >= 0 && newWidthRight > rightChild->getMaxWidth())) return;
		}
		else if (leftChild->getMaxWidth() >= 0 && newWidthLeft > leftChild->getMaxWidth()) {
			// New left width is too large!
			newWidthLeft = leftChild->getMaxWidth();
			newWidthRight = rightChild->getPositionX() + rightChild->getWidth() - leftChild->getPositionX() - newWidthLeft;
			if (newWidthRight < rightChild->getMinWidth() || (rightChild->getMaxWidth() >= 0 && newWidthRight > rightChild->getMaxWidth())) return;
		}
		else {
			newWidthRight = rightChild->getPositionX() + rightChild->getWidth() - borderPos;
			if (newWidthRight < rightChild->getMinWidth()) {
				// New right width is too small!
				newWidthRight = rightChild->getMinWidth();
				newWidthLeft = rightChild->getPositionX() + rightChild->getWidth() - newWidthRight - leftChild->getPositionX();
				if (newWidthLeft < leftChild->getMinWidth() || (leftChild->getMaxWidth() >= 0 && newWidthLeft > leftChild->getMaxWidth())) return;
			}
			else if (rightChild->getMaxWidth() >= 0 && newWidthRight > rightChild->getMaxWidth()) {
				// New right width is too large!
				newWidthRight = rightChild->getMaxWidth();
				newWidthLeft = rightChild->getPositionX() + rightChild->getWidth() - newWidthRight - leftChild->getPositionX();
				if (newWidthLeft < leftChild->getMinWidth() || (leftChild->getMaxWidth() >= 0 && newWidthLeft > leftChild->getMaxWidth())) return;
			}
		}
		leftChild->setWidth(newWidthLeft);
		rightChild->setWidth(newWidthRight);
		rightChild->setPositionX(leftChild->getPositionX() + newWidthLeft);
		// Compute new percentages and weights
		double totalPercentage = percentages[resizeBorder] + percentages[resizeBorder + 1];
		double totalWidth = static_cast<double>(newWidthLeft + newWidthRight);
		percentages[resizeBorder] = static_cast<double>(newWidthLeft) / totalWidth * totalPercentage;
		percentages[resizeBorder + 1] = static_cast<double>(newWidthRight) / totalWidth * totalPercentage;
		weights[resizeBorder] = getTotalWeight() * percentages[resizeBorder];
		weights[resizeBorder + 1] = getTotalWeight() * percentages[resizeBorder + 1];
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
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
	void GuiHorizontalWeightedLayout::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::HRESIZE);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalWeightedLayout::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}
	//--------------------------------------------------------------------------------------------------
}