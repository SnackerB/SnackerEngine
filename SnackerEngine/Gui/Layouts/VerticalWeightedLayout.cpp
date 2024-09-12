#include "Gui/Layouts/VerticalWeightedLayout.h"
#include "Graphics/Renderer.h"
#include "Utility\Keys.h"

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
	void GuiVerticalWeightedLayout::computeHeightHintsFromChildren()
	{
		if (getResizeMode() != ResizeMode::RESIZE_RANGE) return;
		int totalMinHeight = 0;
		int totalPreferredHeight = SIZE_HINT_ARBITRARY;
		for (auto childID : getChildren()) {
			GuiElement* child = getElement(childID);
			if (child) {
				totalMinHeight += child->getMinHeight();
				if (totalPreferredHeight != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					int tempPreferredHeight = child->getPreferredHeight();
					if (tempPreferredHeight >= 0) {
						if (totalPreferredHeight >= 0) totalPreferredHeight += tempPreferredHeight;
						else totalPreferredHeight = tempPreferredHeight;
					}
					else if (tempPreferredHeight == SIZE_HINT_AS_LARGE_AS_POSSIBLE) totalPreferredHeight = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				}
			}
		}
		int totalBorders = 2 * outerVerticalBorder + static_cast<int>((getChildren().size() - 1)) * verticalBorder;
		totalMinHeight += totalBorders;
		if (totalPreferredHeight >= 0) totalPreferredHeight += totalBorders;
		setMinHeight(totalMinHeight);
		setPreferredHeight(totalPreferredHeight);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalWeightedLayout::GuiVerticalWeightedLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVerticalLayout(json, data, parameterNames)
	{
		if (!json.contains("verticalLayoutMode")) setVerticalLayoutMode(VerticalLayoutMode::CHILD_WIDTH_TO_LAYOUT_WIDTH);
		parseJsonOrReadFromData(verticalBorder, "verticalBorder", json, data, parameterNames);
		parseJsonOrReadFromData(outerVerticalBorder, "outerVerticalBorder", json, data, parameterNames);
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
	void GuiVerticalWeightedLayout::setOuterVerticalBorder(int outerVerticalBorder)
	{
		if (this->outerVerticalBorder != outerVerticalBorder) {
			this->outerVerticalBorder = outerVerticalBorder;
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
	std::unique_ptr<GuiElementAnimatable> GuiVerticalWeightedLayout::animateWeight(GuiID childID, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalWeightedLayoutWeightAnimatable : public GuiElementValueAnimatable<double>
		{
			GuiID childID;
			virtual void onAnimate(const double& currentVal) override { if (element) static_cast<GuiVerticalWeightedLayout*>(element)->setWeight(childID, currentVal); }
		public:
			GuiVerticalWeightedLayoutWeightAnimatable(GuiElement& element, GuiID childID, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<double>(element, startVal, stopVal, duration, animationFunction), childID{ childID } {}
		};
		return std::make_unique<GuiVerticalWeightedLayoutWeightAnimatable>(*this, childID, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiVerticalWeightedLayout::animatePercentage(GuiID childID, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalWeightedLayoutPercentageAnimatable : public GuiElementValueAnimatable<double>
		{
			GuiID childID;
			virtual void onAnimate(const double& currentVal) override { if (element) static_cast<GuiVerticalWeightedLayout*>(element)->setPercentage(childID, currentVal); }
		public:
			GuiVerticalWeightedLayoutPercentageAnimatable(GuiElement& element, GuiID childID, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<double>(element, startVal, stopVal, duration, animationFunction), childID{ childID } {}
		};
		return std::make_unique<GuiVerticalWeightedLayoutPercentageAnimatable>(*this, childID, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiVerticalWeightedLayout::animateVerticalBorder(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalWeightedLayoutVerticalBorderAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) static_cast<GuiVerticalWeightedLayout*>(element)->setVerticalBorder(currentVal); }
		public:
			GuiVerticalWeightedLayoutVerticalBorderAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiVerticalWeightedLayoutVerticalBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiVerticalWeightedLayout::animateOuterVerticalBorder(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalWeightedLayoutOuterVerticalBorderAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) static_cast<GuiVerticalWeightedLayout*>(element)->setOuterVerticalBorder(currentVal); }
		public:
			GuiVerticalWeightedLayoutOuterVerticalBorderAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiVerticalWeightedLayoutOuterVerticalBorderAnimatable >(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiVerticalWeightedLayout::animateResizeAreaHeight(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalWeightedLayoutResizeAreaHeightAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiVerticalWeightedLayout*>(element)->setResizeAreaHeight(currentVal); }
		public:
			GuiVerticalWeightedLayoutResizeAreaHeightAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiVerticalWeightedLayoutResizeAreaHeightAnimatable>(*this, startVal, stopVal, duration, animationFunction);
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
		unsigned totalRemainingHeight = getHeight() - static_cast<int>(weights.size() == 0 ? 0 : weights.size() - 1) * verticalBorder - 2 * outerVerticalBorder;
		double remainingPercentage = 1.0f;
		const auto& children = getChildren();
		std::vector<int> heights(children.size());
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
					int height = static_cast<int>(percentages[*it] / remainingPercentage * static_cast<double>(totalRemainingHeight));
					if (child->getMaxHeight() >= 0 && height > child->getMaxHeight()) {
						// If the percentage height is too large, we just set the child to its maximum height
						heights[*it] = child->getMaxHeight();
						totalRemainingHeight = heights[*it];
						remainingPercentage -= percentages[*it];
						indicesOfChildrenLeftToResize.erase(it);
						problemOccured = true;
						break;
					}
					else if (height < child->getMinHeight()) {
						// If the percentage hegight is too small, we just set the child to its minimum height
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
			int position = outerVerticalBorder;
			if (alignmentVertical == AlignmentVertical::CENTER) {
				int totalHeight = verticalBorder * static_cast<int>((children.size() - 1));
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
			int position = getHeight() - outerVerticalBorder;
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
		// Compute the min height of this layout from its children
		computeHeightHintsFromChildren();
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<std::pair<unsigned int, int>> GuiVerticalWeightedLayout::getCollidingBorderAndOffset(const Vec2i& offset) const
	{
		const auto& children = getChildren();
		for (unsigned i = 1; i < children.size(); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) {
				if (child->getPositionY() > offset.y + resizeAreaHeight) return {};
				else if (child->getPositionY() > offset.y - resizeAreaHeight) return { { i - 1, offset.y - child->getPositionY() } };
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
		GuiElement* upperChild = getElement(getChildren()[resizeBorder]);
		GuiElement* lowerChild = getElement(getChildren()[resizeBorder + 1]);
		int newHeightUp = borderPos - upperChild->getPositionY();
		int newHeightDown = 0;
		if (newHeightUp < upperChild->getMinHeight()) {
			// New upper height is too small!
			newHeightUp = upperChild->getMinHeight();
			newHeightDown = lowerChild->getPositionY() + lowerChild->getHeight() - upperChild->getPositionY() - newHeightUp;;
			if (newHeightDown < lowerChild->getMinHeight() || (upperChild->getMaxHeight() >= 0 && newHeightDown > lowerChild->getMaxHeight())) return;
		}
		else if (upperChild->getMaxHeight() >= 0 && newHeightUp > upperChild->getMaxHeight()) {
			// New upper height is too large!
			newHeightUp = upperChild->getMaxHeight();
			newHeightDown = lowerChild->getPositionY() + lowerChild->getHeight() - upperChild->getPositionY() - newHeightUp;
			if (newHeightDown < lowerChild->getMinHeight() || (lowerChild->getMaxHeight() >= 0 && newHeightDown > lowerChild->getMaxHeight())) return;
		}
		else {
			newHeightDown = lowerChild->getPositionY() + lowerChild->getHeight() - borderPos;
			if (newHeightDown < lowerChild->getMinHeight()) {
				// New lower height is too small!
				newHeightDown = lowerChild->getMinHeight();
				newHeightUp = lowerChild->getPositionY() + lowerChild->getHeight() - newHeightDown - upperChild->getPositionY();
				if (newHeightUp < upperChild->getMinHeight() || (upperChild->getMaxHeight() >= 0 && newHeightUp > upperChild->getMaxHeight())) return;
			}
			else if (lowerChild->getMaxHeight() >= 0 && newHeightDown > lowerChild->getMaxHeight()) {
				// New lower height is too large!
				newHeightDown = lowerChild->getMaxHeight();
				newHeightDown = lowerChild->getPositionY() + lowerChild->getHeight() - newHeightDown - upperChild->getPositionY();
				if (newHeightUp < upperChild->getMinHeight() || (upperChild->getMaxHeight() >= 0 && newHeightUp > upperChild->getMaxHeight())) return;
			}
		}
		upperChild->setHeight(newHeightUp);
		lowerChild->setHeight(newHeightDown);
		lowerChild->setPositionY(upperChild->getPositionY() + newHeightUp);
		// Compute new percentages and weights
		double totalPercentage = percentages[resizeBorder] + percentages[resizeBorder + 1];
		double totalHeight = static_cast<double>(newHeightUp + newHeightDown);
		percentages[resizeBorder] = static_cast<double>(newHeightUp) / totalHeight * totalPercentage;
		percentages[resizeBorder + 1] = static_cast<double>(newHeightDown) / totalHeight * totalPercentage;
		weights[resizeBorder] = getTotalWeight() * percentages[resizeBorder];
		weights[resizeBorder + 1] = getTotalWeight() * percentages[resizeBorder + 1];
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