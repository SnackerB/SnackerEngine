#include "Gui\Layouts\HorizontalListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	Color4f GuiHorizontalListLayout::defaultBackgroundColor = Color4f(0.0f, 0.0f);
	//--------------------------------------------------------------------------------------------------
	std::vector<int> GuiHorizontalListLayout::computeChildWidths(const std::vector<int>& minWidths, const std::vector<int>& preferredWidths, const std::vector<int>& maxWidths, int width)
	{
		std::vector<int> widths{};
		int totalMinWidth = 0;
		int totalPreferredWidth = 0;
		for (std::size_t i = 0; i < minWidths.size(); ++i) {
			widths.push_back(minWidths[i]);
			totalMinWidth += minWidths[i];
			if (preferredWidths[i] == SIZE_HINT_AS_LARGE_AS_POSSIBLE) totalPreferredWidth = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
			else if (preferredWidths[i] >= 0 && totalPreferredWidth >= 0) totalPreferredWidth += preferredWidths[i] == SIZE_HINT_ARBITRARY ? minWidths[i] : preferredWidths[i];
		}
		int remainingWidth = width - totalMinWidth;
		if (remainingWidth > 0) {
			// If the total min Width is smaller than the available Width, we can afford to make
			// elements larger!
			if (totalPreferredWidth == SIZE_HINT_AS_LARGE_AS_POSSIBLE || totalPreferredWidth - totalMinWidth >= remainingWidth) {
				// If the totalPreferredWidth is larger than the available width, we need to distribute the available width
				// between the elements
				// First, create a vector of indices of children where the width is still undecided, and where there is
				// actually a preferredWidth larger than the minWidth
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < minWidths.size(); ++i) {
					if (preferredWidths[i] == SIZE_HINT_AS_LARGE_AS_POSSIBLE || preferredWidths[i] > minWidths[i]) indicesOfChildrenLeftToResize.push_back(i);
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingWidth > 0) {
					problemOccured = false;
					int widthSlice = remainingWidth / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstWidthSlice = widthSlice + remainingWidth % indicesOfChildrenLeftToResize.size();
					// Handle first child
					int childIndex = indicesOfChildrenLeftToResize[0];
					if (preferredWidths[childIndex] != SIZE_HINT_AS_LARGE_AS_POSSIBLE && preferredWidths[childIndex] < minWidths[childIndex] + firstWidthSlice) {
						widths[childIndex] = preferredWidths[childIndex];
						remainingWidth -= preferredWidths[childIndex] - minWidths[childIndex];
						indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
						problemOccured = true;
						continue;
					}
					else {
						widths[childIndex] = minWidths[childIndex] + firstWidthSlice;
					}
					// Handle remaining childs
					for (auto it = indicesOfChildrenLeftToResize.begin() + 1; it != indicesOfChildrenLeftToResize.end(); ++it) {
						if (preferredWidths[childIndex] != SIZE_HINT_AS_LARGE_AS_POSSIBLE && preferredWidths[*it] < minWidths[*it] + widthSlice) {
							widths[*it] = preferredWidths[*it];
							remainingWidth -= preferredWidths[*it] - minWidths[*it];
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							widths[*it] = minWidths[*it] + widthSlice;
						}
					}
				}
			}
			else {
				// If instead the total preferred width is smaller than the available width, we can set all children to their
				// preferred width! We also make a list of children who still want to have a larger width, i.e. who dont have a
				// preferred width but are still not at their max width, or don't have a max width as well.
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < minWidths.size(); ++i) {
					if (preferredWidths[i] >= 0) {
						widths[i] = preferredWidths[i];
						remainingWidth -= preferredWidths[i] - minWidths[i];
					}
					else if (maxWidths[i] == SIZE_HINT_ARBITRARY || maxWidths[i] > minWidths[i]) indicesOfChildrenLeftToResize.push_back(i);
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingWidth > 0)
				{
					problemOccured = false;
					int widthSlice = remainingWidth / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstWidthSlice = widthSlice + remainingWidth % indicesOfChildrenLeftToResize.size();
					// Handle first child
					int childIndex = indicesOfChildrenLeftToResize[0];
					if (maxWidths[childIndex] != SIZE_HINT_ARBITRARY && maxWidths[childIndex] < minWidths[childIndex] + firstWidthSlice) {
						remainingWidth -= maxWidths[childIndex] - minWidths[childIndex];
						widths[childIndex] = maxWidths[childIndex];
						indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
						problemOccured = true;
						continue;
					}
					else {
						widths[childIndex] = minWidths[childIndex] + firstWidthSlice;
					}
					// Handle remaining childs
					for (auto it = indicesOfChildrenLeftToResize.begin() + 1; it != indicesOfChildrenLeftToResize.end(); ++it) {
						if (maxWidths[*it] != SIZE_HINT_ARBITRARY && maxWidths[*it] < minWidths[*it] + widthSlice) {
							remainingWidth -= maxWidths[*it] - minWidths[*it];
							widths[*it] = maxWidths[*it];
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							widths[*it] = minWidths[*it] + widthSlice;
						}
					}
				}
			}
		}
		return widths;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::computeWidthHintsFromChildren()
	{
		if (getResizeMode() == ResizeMode::SAME_AS_PARENT) return;
		int totalMinWidth = 0;
		int totalPreferredWidth = SIZE_HINT_ARBITRARY;
		for (auto childID : getChildren()) {
			GuiElement* child = getElement(childID);
			if (child) {
				totalMinWidth += child->getMinWidth();
				if (totalPreferredWidth != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					int tempPreferredWidth = child->getPreferredWidth();
					if (tempPreferredWidth >= 0) {
						if (totalPreferredWidth == SIZE_HINT_ARBITRARY) totalPreferredWidth = tempPreferredWidth;
						else totalPreferredWidth += tempPreferredWidth;
					}
					else if (tempPreferredWidth == SIZE_HINT_AS_LARGE_AS_POSSIBLE) totalPreferredWidth = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				}
			}
		}
		int totalBorders = 2 * outerHorizontalBorder + static_cast<int>((getChildren().size() - 1)) * horizontalBorder;
		totalMinWidth += totalBorders;
		if (totalPreferredWidth >= 0) totalPreferredWidth += totalBorders;
		setMinWidth(totalMinWidth);
		if (shrinkWidthToChildren) setPreferredWidth(totalPreferredWidth);
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout::GuiHorizontalListLayout(Color4f backgroundColor)
		: GuiHorizontalLayout(), backgroundColor(backgroundColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout::GuiHorizontalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiHorizontalLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(alignmentHorizontal, "alignmentHorizontal", json, data, parameterNames);
		parseJsonOrReadFromData(horizontalBorder, "horizontalBorder", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundColor, "backgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundShader, "backgroundShader", json, data, parameterNames);
		parseJsonOrReadFromData(groupName, "horizontalLayoutGroupName", json, data, parameterNames);
		parseJsonOrReadFromData(outerHorizontalBorder, "outerHorizontalBorder", json, data, parameterNames);
		if (!json.contains("outerHorizontalBorder")) outerHorizontalBorder = horizontalBorder;
		parseJsonOrReadFromData(shrinkWidthToChildren, "shrinkWidthToChildren", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout::~GuiHorizontalListLayout()
	{
		leaveAllGroups();
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout::GuiHorizontalListLayout(const GuiHorizontalListLayout& other) noexcept
		: GuiHorizontalLayout(other), alignmentHorizontal(other.alignmentHorizontal), 
		horizontalBorder(other.horizontalBorder), outerHorizontalBorder(other.outerHorizontalBorder),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground), 
		backgroundShader(other.backgroundShader), groupID(-1), groupName("")
	{
		if (other.groupID != -1) setHorizontalLayoutGroupID(other.groupID);
		else if (!other.groupName.empty()) setHorizontalLayoutGroupName(other.groupName);
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout& GuiHorizontalListLayout::operator=(const GuiHorizontalListLayout& other) noexcept
	{
		GuiHorizontalLayout::operator=(other);
		if (groupID != -1) leaveGroup(groupID);
		alignmentHorizontal = other.alignmentHorizontal;
		horizontalBorder = other.horizontalBorder;
		outerHorizontalBorder = other.outerHorizontalBorder;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = other.backgroundShader;
		groupID = -1;
		groupName = "";
		if (other.groupID != -1) setHorizontalLayoutGroupID(other.groupID);
		else if (!other.groupName.empty()) setHorizontalLayoutGroupName(other.groupName);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout::GuiHorizontalListLayout(GuiHorizontalListLayout&& other) noexcept
		: GuiHorizontalLayout(std::move(other)), alignmentHorizontal(other.alignmentHorizontal),
		horizontalBorder(other.horizontalBorder), outerHorizontalBorder(other.outerHorizontalBorder),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground), 
		backgroundShader(std::move(other.backgroundShader)),
		groupID(other.groupID), groupName(std::move(other.groupName))
	{
		other.groupID = -1;
		other.groupName = "";
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout& GuiHorizontalListLayout::operator=(GuiHorizontalListLayout&& other) noexcept
	{
		GuiHorizontalLayout::operator=(std::move(other));
		if (groupID != -1) leaveGroup(groupID);
		alignmentHorizontal = other.alignmentHorizontal;
		horizontalBorder = other.horizontalBorder;
		outerHorizontalBorder = other.outerHorizontalBorder;
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
	void GuiHorizontalListLayout::setAlignmentHorizontal(AlignmentHorizontal alignmentHorizontal)
	{
		if (alignmentHorizontal != this->alignmentHorizontal) {
			this->alignmentHorizontal = alignmentHorizontal;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::setHorizontalBorder(unsigned horizontalBorder)
	{
		if (horizontalBorder != this->horizontalBorder) {
			this->horizontalBorder = horizontalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::setOuterHorizontalBorder(unsigned outerHorizontalBorder)
	{
		if (outerHorizontalBorder != this->outerHorizontalBorder) {
			this->outerHorizontalBorder = outerHorizontalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalListLayout::setHorizontalLayoutGroupID(GuiGroupID groupID)
	{
		if (this->groupID != -1) return false;
		if (!groupExists(groupID)) return false;
		GuiGroup* group = getGroup(groupID);
		if (group == nullptr || group->getGroupType() != "HORIZONTAL_LAYOUT_GROUP") return false;
		if (joinGroup(groupID)) {
			this->groupID = groupID;
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalListLayout::setHorizontalLayoutGroupName(const std::string& groupName)
	{
		if (this->groupID != -1) return false;
		this->groupName = groupName;
		std::optional<GuiGroupID> tempGroupID = groupExists(groupName);
		if (!tempGroupID.has_value()) {
			// Create new group!
			std::unique_ptr<HorizontalLayoutGroup> group = std::make_unique<HorizontalLayoutGroup>(groupName);
			auto result = createGroup(std::move(group));
			if (result.has_value()) {
				groupID = result.value();
				return true;
			}
			else {
				return false;
			}
		}
		return setHorizontalLayoutGroupID(tempGroupID.value());
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::setShrinkWidthToChildren(bool shrinkWidthToChildren)
	{
		if (this->shrinkWidthToChildren != shrinkWidthToChildren) {
			this->shrinkWidthToChildren = shrinkWidthToChildren;
			if (shrinkWidthToChildren) computeWidthHintsFromChildren();
			else setPreferredWidth(SIZE_HINT_ARBITRARY);
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiHorizontalListLayout::animateHorizontalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiHorizontalListLayoutHorizontalBorderAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiHorizontalListLayout*>(element)->setHorizontalBorder(currentVal); }
		public:
			GuiHorizontalListLayoutHorizontalBorderAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiHorizontalListLayoutHorizontalBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiHorizontalListLayout::animateOuterHorizontalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiHorizontalListLayoutOuterHorizontalBorderAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiHorizontalListLayout*>(element)->setOuterHorizontalBorder(currentVal); }
		public:
			GuiHorizontalListLayoutOuterHorizontalBorderAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiHorizontalListLayoutOuterHorizontalBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiHorizontalListLayout::animateBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiHorizontalListLayoutBackgroundColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiHorizontalListLayout*>(element)->setBackgroundColor(currentVal); }
		public:
			GuiHorizontalListLayoutBackgroundColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiHorizontalListLayoutBackgroundColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::computeModelMatrix()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-getHeight()), 0.0f),
			Vec3f(static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::drawBackground(const Vec2i& worldPosition)
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
	void GuiHorizontalListLayout::draw(const Vec2i& worldPosition)
	{
		drawBackground(worldPosition);
		pushClippingBox(worldPosition);
		GuiElement::draw(worldPosition);
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::onRegister()
	{
		GuiLayout::onRegister();
		computeModelMatrix();
		if (groupName != "" && groupID == -1) setHorizontalLayoutGroupName(groupName);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::onSizeChange()
	{
		GuiLayout::onSizeChange();
		computeModelMatrix();
		if (groupID != -1) {
			GuiGroup* groupPointer = getGroup(groupID);
			if (groupPointer) static_cast<HorizontalLayoutGroup*>(groupPointer)->markForRecompute();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::enforceLayout()
	{
		GuiHorizontalLayout::enforceLayout();
		std::vector<int> widths;
		if (groupID != -1 && groupExists(groupID)) {
			GuiGroup* groupPointer = getGroup(groupID);
			if (groupPointer != nullptr) {
				HorizontalLayoutGroup* horizontalLayoutGroupPointer = static_cast<HorizontalLayoutGroup*>(groupPointer);
				horizontalLayoutGroupPointer->recomputeWidths();
				widths = horizontalLayoutGroupPointer->getWidths();
			}
		}
		const auto& childIDs = getChildren();
		if (childIDs.empty()) return;
		if (widths.empty()) {
			std::vector<int> minWidths;
			std::vector<int> preferredWidths;
			std::vector<int> maxWidths;
			for (auto childId : childIDs) {
				GuiElement* child = getElement(childId);
				if (child) {
					minWidths.push_back(child->getMinWidth());
					if (child->getPreferredWidth() < 0) preferredWidths.push_back(child->getPreferredWidth());
					else preferredWidths.push_back(child->clampToMinMaxWidth(child->getPreferredWidth()));
					maxWidths.push_back(child->getMaxWidth());
				}
				else {
					minWidths.push_back(0);
					preferredWidths.push_back(SIZE_HINT_ARBITRARY);
					maxWidths.push_back(SIZE_HINT_ARBITRARY);
				}
			}
			widths = computeChildWidths(minWidths, preferredWidths, maxWidths, getWidth() - static_cast<int>(horizontalBorder * (getChildren().size() - 1)) - static_cast<int>(outerHorizontalBorder * 2));
		}
		// Now we can actually position and resize the elements
		if (alignmentHorizontal == AlignmentHorizontal::LEFT || alignmentHorizontal == AlignmentHorizontal::CENTER) {
			int position = outerHorizontalBorder;
			if (alignmentHorizontal == AlignmentHorizontal::CENTER) {
				int totalWidth = horizontalBorder * (static_cast<int>(childIDs.size()) - 1);
				for (auto width : widths) totalWidth += width;
				position = (getWidth() - totalWidth) / 2;
			}
			for (unsigned i = 0; i < childIDs.size(); ++i) {
				GuiElement* child = getElement(childIDs[i]);
				if (child) {
					child->setPositionX(position);
					child->setWidth(widths[i]);
				}
				position += widths[i] + horizontalBorder;
			}
		}
		else {
			int position = getWidth() - horizontalBorder;
			for (int i = static_cast<int>(childIDs.size()) - 1; i >= 0; --i) {
				GuiElement* child = getElement(childIDs[i]);
				position -= widths[i];
				if (child) {
					child->setPositionX(position);
					child->setWidth(widths[i]);
				}
				position -= horizontalBorder;
			}
		}
		// Compute new minWidth
		computeWidthHintsFromChildren();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::leaveAllGroups()
	{
		if (groupID != -1) {
			leaveGroup(groupID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::HorizontalLayoutGroup::onElementJoin(GuiElement::GuiID element, std::size_t position)
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
	void GuiHorizontalListLayout::HorizontalLayoutGroup::onElementLeave(GuiElement::GuiID element, std::size_t position)
	{
		onElementJoin(element, 0);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::HorizontalLayoutGroup::recomputeWidths()
	{
		if (upToDate) return;
		upToDate = true;
		widths.clear();
		GuiManager* guiManager = getGuiManager();
		if (!guiManager) return;
		const std::vector<GuiID>& elements = getElements();
		if (elements.empty()) return;
		// Compute horizontal border from first element
		GuiElement* firstElement = guiManager->getGuiElement(elements[0]);
		if (!firstElement) return;
		int horizontalBorder = static_cast<GuiHorizontalListLayout*>(firstElement)->getHorizontalBorder();
		int outerHorizontalBorder = static_cast<GuiHorizontalListLayout*>(firstElement)->getOuterHorizontalBorder();
		// First find out the total width. For this, we choose the smallest width of all elements in the group.
		// Additionally we need to compute the highest number of children one element has, and the largest horizontal border.
		int width = SIZE_HINT_ARBITRARY;
		std::size_t numberOfChildren = 0;
		for (const auto& element : elements) {
			GuiElement* guiElement = guiManager->getGuiElement(element);
			if (guiElement) {
				int tempWidth = guiElement->getWidth();
				if (tempWidth < width || width == SIZE_HINT_ARBITRARY) width = tempWidth;
				int numberOfChildrenTemp = static_cast<int>(guiElement->getChildren().size());
				if (numberOfChildrenTemp > numberOfChildren) numberOfChildren = numberOfChildrenTemp;
			}
		}
		if (width == SIZE_HINT_ARBITRARY) width = 0;
		// Next, we need to collect information about the elements' children: The maximal minWidth, the maximal preferredWidth,
		// and the maximal maxWidth!
		std::vector<int> minWidths(numberOfChildren, 0);
		std::vector<int> preferredWidths(numberOfChildren, SIZE_HINT_ARBITRARY);
		std::vector<int> maxWidths(numberOfChildren, 0);
		std::size_t index = 0;
		for (const auto& element : elements) {
			GuiElement* guiElement = guiManager->getGuiElement(element);
			if (guiElement) {
				const std::vector<GuiID>& children = guiElement->getChildren();
				for (std::size_t i = 0; i < std::min(numberOfChildren, children.size()); ++i) {
					GuiElement* child = guiManager->getGuiElement(children[i]);
					minWidths[i] = std::max(child->getMinWidth(), minWidths[i]);
					if (preferredWidths[i] != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
						int tempPreferredWidth = child->getPreferredWidth();
						if (tempPreferredWidth >= 0 && tempPreferredWidth > preferredWidths[i]) preferredWidths[i] = tempPreferredWidth;
						else if (tempPreferredWidth == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredWidths[i] = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
					}
					int tempMaxWidth = child->getMaxWidth();
					if (tempMaxWidth == SIZE_HINT_ARBITRARY || (maxWidths[i] != SIZE_HINT_ARBITRARY && tempMaxWidth > maxWidths[i])) maxWidths[i] = tempMaxWidth;
				}
			}
		}
		widths = GuiHorizontalListLayout::computeChildWidths(minWidths, preferredWidths, maxWidths, width - static_cast<int>(minWidths.size() - 1) * horizontalBorder - 2 * outerHorizontalBorder);
	}
	//--------------------------------------------------------------------------------------------------
}