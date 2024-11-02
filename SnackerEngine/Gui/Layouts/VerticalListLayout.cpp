#include "Gui\Layouts\VerticalListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	Color4f GuiVerticalListLayout::defaultBackgroundColor = Color4f(0.0f, 0.0f);
	//--------------------------------------------------------------------------------------------------
	std::vector<int> GuiVerticalListLayout::computeChildHeights(const std::vector<int>& minHeights, const std::vector<int>& preferredHeights, const std::vector<int>& maxHeights, int height)
	{
		std::vector<int> heights{};
		int totalMinHeight = 0;
		int totalPreferredHeight = 0;
		for (std::size_t i = 0; i < minHeights.size(); ++i) {
			heights.push_back(minHeights[i]);
			totalMinHeight += minHeights[i];
			if (preferredHeights[i] == SIZE_HINT_AS_LARGE_AS_POSSIBLE) totalPreferredHeight = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
			else if (preferredHeights[i] >= 0 && totalPreferredHeight >= 0) totalPreferredHeight += preferredHeights[i] == SIZE_HINT_ARBITRARY ? minHeights[i] : preferredHeights[i];
		}
		int remainingHeight = height - totalMinHeight;
		if (remainingHeight > 0) {
			// If the total min Height is smaller than the available Height, we can afford to make
			// elements larger!
			if (totalPreferredHeight == SIZE_HINT_AS_LARGE_AS_POSSIBLE || totalPreferredHeight - totalMinHeight >= remainingHeight) {
				// If the totalPreferredheight is larger than the available height, we need to distribute the available height
				// between the elements
				// First, create a vector of indices of children where the height is still undecided, and where there is
				// actually a preferredHeight larger than the minHeight
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < minHeights.size(); ++i) {
					if (preferredHeights[i] == SIZE_HINT_AS_LARGE_AS_POSSIBLE || preferredHeights[i] > minHeights[i]) indicesOfChildrenLeftToResize.push_back(i);
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingHeight > 0) {
					problemOccured = false;
					int heightSlice = remainingHeight / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstHeightSlice = heightSlice + remainingHeight % indicesOfChildrenLeftToResize.size();
					// Handle first child
					int childIndex = indicesOfChildrenLeftToResize[0];
					if (preferredHeights[childIndex] != SIZE_HINT_AS_LARGE_AS_POSSIBLE && preferredHeights[childIndex] < minHeights[childIndex] + firstHeightSlice) {
						heights[childIndex] = preferredHeights[childIndex];
						remainingHeight -= preferredHeights[childIndex] - minHeights[childIndex];
						indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
						problemOccured = true;
						continue;
					}
					else {
						heights[childIndex] = minHeights[childIndex] + firstHeightSlice;
					}
					// Handle remaining childs
					for (auto it = indicesOfChildrenLeftToResize.begin() + 1; it != indicesOfChildrenLeftToResize.end(); ++it) {
						if (preferredHeights[childIndex] != SIZE_HINT_AS_LARGE_AS_POSSIBLE && preferredHeights[*it] < minHeights[*it] + heightSlice) {
							heights[*it] = preferredHeights[*it];
							remainingHeight -= preferredHeights[*it] - minHeights[*it];
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							heights[*it] = minHeights[*it] + heightSlice;
						}
					}
				}
			}
			else {
				// If instead the total preferred height is smaller than the available height, we can set all children to their
				// preferred height! We also make a list of children who still want to have a larger height, i.e. who dont have a
				// preferred height but are still not at their max height, or don't have a max height as well.
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < minHeights.size(); ++i) {
					if (preferredHeights[i] >= 0) {
						heights[i] = preferredHeights[i];
						remainingHeight -= preferredHeights[i] - minHeights[i];
					}
					else if (maxHeights[i] == SIZE_HINT_ARBITRARY || maxHeights[i] > minHeights[i]) indicesOfChildrenLeftToResize.push_back(i);
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingHeight > 0)
				{
					problemOccured = false;
					int heightSlice = remainingHeight / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstHeightSlice = heightSlice + remainingHeight % indicesOfChildrenLeftToResize.size();
					// Handle first child
					int childIndex = indicesOfChildrenLeftToResize[0];
					if (maxHeights[childIndex] != SIZE_HINT_ARBITRARY && maxHeights[childIndex] < minHeights[childIndex] + firstHeightSlice) {
						remainingHeight -= maxHeights[childIndex] - minHeights[childIndex];
						heights[childIndex] = maxHeights[childIndex];
						indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
						problemOccured = true;
						continue;
					}
					else {
						heights[childIndex] = minHeights[childIndex] + firstHeightSlice;
					}
					// Handle remaining childs
					for (auto it = indicesOfChildrenLeftToResize.begin() + 1; it != indicesOfChildrenLeftToResize.end(); ++it) {
						if (maxHeights[*it] != SIZE_HINT_ARBITRARY && maxHeights[*it] < minHeights[*it] + heightSlice) {
							remainingHeight -= maxHeights[*it] - minHeights[*it];
							heights[*it] = maxHeights[*it];
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							heights[*it] = minHeights[*it] + heightSlice;
						}
					}
				}
			}
		}
		return heights;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::computeHeightHintsFromChildren()
	{
		if (getResizeMode() == ResizeMode::SAME_AS_PARENT) return;
		int totalMinHeight = 0;
		int totalPreferredHeight = SIZE_HINT_ARBITRARY;
		for (auto childID : getChildren()) {
			GuiElement* child = getElement(childID);
			if (child) {
				totalMinHeight += child->getMinHeight();
				if (totalPreferredHeight != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					int tempPreferredHeight = child->getPreferredHeight();
					if (tempPreferredHeight >= 0) {
						if (totalPreferredHeight == SIZE_HINT_ARBITRARY) totalPreferredHeight = tempPreferredHeight;
						else totalPreferredHeight += tempPreferredHeight;
					}
					else if (tempPreferredHeight == SIZE_HINT_AS_LARGE_AS_POSSIBLE) totalPreferredHeight = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				}
			}
		}
		int totalBorders = 2 * outerVerticalBorder + static_cast<int>((getChildren().size() - 1)) * verticalBorder;
		totalMinHeight += totalBorders;
		if (totalPreferredHeight >= 0) totalPreferredHeight += totalBorders;
		setMinHeight(totalMinHeight);
		if (shrinkHeightToChildren) setPreferredHeight(totalPreferredHeight);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout::GuiVerticalListLayout(Color4f backgroundColor)
		: GuiVerticalLayout(), backgroundColor(backgroundColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout::GuiVerticalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVerticalLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(alignmentVertical, "alignmentVertical", json, data, parameterNames);
		parseJsonOrReadFromData(verticalBorder, "verticalBorder", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundColor, "backgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundShader, "backgroundShader", json, data, parameterNames);
		parseJsonOrReadFromData(groupName, "verticalLayoutGroupName", json, data, parameterNames);
		parseJsonOrReadFromData(outerVerticalBorder, "outerVerticalBorder", json, data, parameterNames);
		if (!json.contains("outerVerticalBorder")) outerVerticalBorder = verticalBorder;
		parseJsonOrReadFromData(shrinkHeightToChildren, "shrinkHeightToChildren", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout::~GuiVerticalListLayout()
	{
		leaveAllGroups();
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout::GuiVerticalListLayout(const GuiVerticalListLayout& other) noexcept
		: GuiVerticalLayout(other), alignmentVertical(other.alignmentVertical),
		verticalBorder(other.verticalBorder), outerVerticalBorder(other.outerVerticalBorder),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground), 
		backgroundShader(other.backgroundShader), groupID(-1), groupName("")
	{
		if (other.groupID != -1) setVerticalLayoutGroupID(other.groupID);
		else if (!other.groupName.empty()) setVerticalLayoutGroupName(other.groupName);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout& GuiVerticalListLayout::operator=(const GuiVerticalListLayout& other) noexcept
	{
		GuiVerticalLayout::operator=(other);
		if (groupID != -1) leaveGroup(groupID);
		alignmentVertical = other.alignmentVertical;
		verticalBorder = other.verticalBorder;
		outerVerticalBorder = other.outerVerticalBorder;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = other.backgroundShader;
		groupID = -1;
		groupName = "";
		if (other.groupID != -1) setVerticalLayoutGroupID(other.groupID);
		else if (!other.groupName.empty()) setVerticalLayoutGroupName(other.groupName);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout::GuiVerticalListLayout(GuiVerticalListLayout&& other) noexcept
		: GuiVerticalLayout(std::move(other)), alignmentVertical(other.alignmentVertical),
		verticalBorder(other.verticalBorder), outerVerticalBorder(other.outerVerticalBorder),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground),
		backgroundShader(std::move(other.backgroundShader)), groupID(other.groupID), 
		groupName(std::move(other.groupName))
	{
		other.groupID = -1;
		other.groupName = "";
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout& GuiVerticalListLayout::operator=(GuiVerticalListLayout&& other) noexcept
	{
		GuiVerticalLayout::operator=(std::move(other));
		if (groupID != -1) leaveGroup(groupID);
		alignmentVertical = other.alignmentVertical;
		verticalBorder = other.verticalBorder;
		outerVerticalBorder = other.outerVerticalBorder;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = std::move(other.backgroundShader);
		groupID = other.groupID;
		groupName = std::move(other.groupName);
		other.leaveGroup(other.groupID);
		other.groupID = -1;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::setAlignmentVertical(AlignmentVertical alignmentVertical)
	{
		if (alignmentVertical != this->alignmentVertical) {
			this->alignmentVertical = alignmentVertical;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::setVerticalBorder(unsigned verticalBorder)
	{
		if (verticalBorder != this->verticalBorder) {
			this->verticalBorder = verticalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::setOuterVerticalBorder(unsigned outerVerticalBorder)
	{
		if (outerVerticalBorder != this->outerVerticalBorder) {
			this->outerVerticalBorder = outerVerticalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalListLayout::setVerticalLayoutGroupID(GuiGroupID groupID)
	{
		if (this->groupID != -1) return false;
		if (!groupExists(groupID)) return false;
		GuiGroup* group = getGroup(groupID);
		if (group == nullptr || group->getGroupType() != "VERTICAL_LAYOUT_GROUP") return false;
		if (joinGroup(groupID)) {
			this->groupID = groupID;
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalListLayout::setVerticalLayoutGroupName(const std::string& groupName)
	{
		if (this->groupID != -1) return false;
		this->groupName = groupName;
		std::optional<GuiGroupID> tempGroupID = groupExists(groupName);
		if (!tempGroupID.has_value()) {
			// Create new group!
			std::unique_ptr<VerticalLayoutGroup> group = std::make_unique<VerticalLayoutGroup>(groupName);
			auto result = createGroup(std::move(group));
			if (result.has_value()) {
				groupID = result.value();
				return true;
			}
			else {
				return false;
			}
		}
		return setVerticalLayoutGroupID(tempGroupID.value());
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::setShrinkHeightToChildren(bool shrinkHeightToChildren)
	{
		if (this->shrinkHeightToChildren != shrinkHeightToChildren) {
			this->shrinkHeightToChildren = shrinkHeightToChildren;
			if (shrinkHeightToChildren) computeHeightHintsFromChildren();
			else setPreferredHeight(SIZE_HINT_ARBITRARY);
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiVerticalListLayout::animateVerticalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalListLayoutVerticalBorderAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiVerticalListLayout*>(element)->setVerticalBorder(currentVal); }
		public:
			GuiVerticalListLayoutVerticalBorderAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiVerticalListLayoutVerticalBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiVerticalListLayout::animateOuterVerticalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalListLayoutOuterVerticalBorderAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiVerticalListLayout*>(element)->setOuterVerticalBorder(currentVal); }
		public:
			GuiVerticalListLayoutOuterVerticalBorderAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiVerticalListLayoutOuterVerticalBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiVerticalListLayout::animateBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalListLayoutBackgroundColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiVerticalListLayout*>(element)->setBackgroundColor(currentVal); }
		public:
			GuiVerticalListLayoutBackgroundColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiVerticalListLayoutBackgroundColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::computeModelMatrix()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-getHeight()), 0.0f),
			Vec3f(static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::drawBackground(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (backgroundColor.alpha != 0.0f)
		{
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
			backgroundShader.setUniform<Color4f>("u_color", backgroundColor);
			Renderer::draw(guiManager->getModelSquare());
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::draw(const Vec2i& worldPosition)
	{
		drawBackground(worldPosition);
		pushClippingBox(worldPosition);
		GuiElement::draw(worldPosition);
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::onRegister()
	{
		GuiLayout::onRegister();
		computeModelMatrix();
		if (groupName != "" && groupID == -1) setVerticalLayoutGroupName(groupName);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::onSizeChange()
	{
		GuiLayout::onSizeChange();
		computeModelMatrix();
		if (groupID != -1) {
			GuiGroup* groupPointer = getGroup(groupID);
			if (groupPointer) static_cast<VerticalLayoutGroup*>(groupPointer)->markForRecompute();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::enforceLayout()
	{
		GuiVerticalLayout::enforceLayout();
		std::vector<int> heights;
		if (groupID != -1 && groupExists(groupID)) {
			GuiGroup* groupPointer = getGroup(groupID);
			if (groupPointer != nullptr) {
				VerticalLayoutGroup* verticalLayoutGroupPointer = static_cast<VerticalLayoutGroup*>(groupPointer);
				verticalLayoutGroupPointer->recomputeHeights();
				heights = verticalLayoutGroupPointer->getHeights();
			}
		}
		const auto& childIDs = getChildren();
		if (childIDs.empty()) return;
		if (heights.empty()) {
			std::vector<int> minHeights;
			std::vector<int> preferredHeights;
			std::vector<int> maxHeights;
			for (auto childId : childIDs) {
				GuiElement* child = getElement(childId);
				if (child) {
					minHeights.push_back(child->getMinHeight());
					if (child->getPreferredHeight() < 0) preferredHeights.push_back(child->getPreferredHeight());
					else preferredHeights.push_back(child->clampToMinMaxHeight(child->getPreferredHeight()));
					maxHeights.push_back(child->getMaxHeight());
				}
				else {
					minHeights.push_back(0);
					preferredHeights.push_back(SIZE_HINT_ARBITRARY);
					maxHeights.push_back(SIZE_HINT_ARBITRARY);
				}
			}
			heights = computeChildHeights(minHeights, preferredHeights, maxHeights, getHeight() - static_cast<int>(verticalBorder * (getChildren().size() - 1)) - static_cast<int>(outerVerticalBorder * 2));
		}
		// Now we can actually position and resize the elements
		if (alignmentVertical == AlignmentVertical::TOP || alignmentVertical == AlignmentVertical::CENTER) {
			int position = outerVerticalBorder;
			if (alignmentVertical == AlignmentVertical::CENTER) {
				int totalHeight = verticalBorder * (static_cast<int>(childIDs.size()) - 1);
				for (auto height : heights) totalHeight += height;
				position = (getHeight() - totalHeight) / 2;
			}
			for (unsigned i = 0; i < childIDs.size(); ++i) {
				GuiElement* child = getElement(childIDs[i]);
				if (child) {
					child->setPositionY(position);
					child->setHeight(heights[i]);
				}
				position += heights[i] + verticalBorder;
			}
		}
		else {
			int position = getHeight() - verticalBorder;
			for (int i = static_cast<int>(childIDs.size()) - 1; i >= 0; --i) {
				GuiElement* child = getElement(childIDs[i]);
				position -= heights[i];
				if (child) {
					child->setPositionY(position);
					child->setHeight(heights[i]);
				}
				position -= verticalBorder;
			}
		}
		// Compute new minHeight
		computeHeightHintsFromChildren();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::leaveAllGroups()
	{
		if (groupID != -1) {
			leaveGroup(groupID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::VerticalLayoutGroup::onElementJoin(GuiID element, std::size_t position)
	{
		GuiManager* guiManager = getGuiManager();
		if (guiManager) {
			for (auto element : getElements()) {
				GuiElement* guiElement = guiManager->getGuiElement(element);
				if (guiElement) guiElement->registerEnforceLayoutDown();
			}
		}
		upToDate = false;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::VerticalLayoutGroup::onElementLeave(GuiID element, std::size_t position)
	{
		onElementJoin(element, 0);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::VerticalLayoutGroup::recomputeHeights()
	{
		if (upToDate) return;
		upToDate = true;
		heights.clear();
		GuiManager* guiManager = getGuiManager();
		if (!guiManager) return;
		const std::vector<GuiID>& elements = getElements();
		if (elements.empty()) return;
		// Compute vertical border from first element
		GuiElement* firstElement = guiManager->getGuiElement(elements[0]);
		if (!firstElement) return;
		int verticalBorder = static_cast<GuiVerticalListLayout*>(firstElement)->getVerticalBorder();
		int outerVerticalBorder = static_cast<GuiVerticalListLayout*>(firstElement)->getOuterVerticalBorder();
		// First find out the total height. For this, we choose the smallest hright of all elements in the group.
		// Additionally we need to compute the highest number of children one element has, and the largest vertical border.
		int height = SIZE_HINT_ARBITRARY;
		std::size_t numberOfChildren = 0;
		for (const auto& element : elements) {
			GuiElement* guiElement = guiManager->getGuiElement(element);
			if (guiElement) {
				int tempHeight = guiElement->getHeight();
				if (tempHeight < height || height == SIZE_HINT_ARBITRARY) height = tempHeight;
				int numberOfChildrenTemp = static_cast<int>(guiElement->getChildren().size());
				if (numberOfChildrenTemp > numberOfChildren) numberOfChildren = numberOfChildrenTemp;
			}
		}
		if (height == SIZE_HINT_ARBITRARY) height = 0;
		// Next, we need to collect information about the elements' children: The maximal minHeight, the maximal preferredHeight,
		// and the maximal maxHeight!
		std::vector<int> minHeights(numberOfChildren, 0);
		std::vector<int> preferredHeights(numberOfChildren, SIZE_HINT_ARBITRARY);
		std::vector<int> maxHeights(numberOfChildren, 0);
		std::size_t index = 0;
		for (const auto& element : elements) {
			GuiElement* guiElement = guiManager->getGuiElement(element);
			if (guiElement) {
				const std::vector<GuiID>& children = guiElement->getChildren();
				for (std::size_t i = 0; i < std::min(numberOfChildren, children.size()); ++i) {
					GuiElement* child = guiManager->getGuiElement(children[i]);
					minHeights[i] = std::max(child->getMinHeight(), minHeights[i]);
					if (preferredHeights[i] != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
						int tempPreferredHeight = child->getPreferredHeight();
						if (tempPreferredHeight >= 0 && tempPreferredHeight > preferredHeights[i]) preferredHeights[i] = tempPreferredHeight;
						else if (tempPreferredHeight == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredHeights[i] = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
					}
					int tempMaxHeight = child->getMaxHeight();
					if (tempMaxHeight == SIZE_HINT_ARBITRARY || (maxHeights[i] != SIZE_HINT_ARBITRARY && tempMaxHeight > maxHeights[i])) maxHeights[i] = tempMaxHeight;
				}
			}
		}
		heights = GuiVerticalListLayout::computeChildHeights(minHeights, preferredHeights, maxHeights, height - static_cast<int>(minHeights.size() - 1) * verticalBorder - 2 * outerVerticalBorder);
	}
	//--------------------------------------------------------------------------------------------------
}