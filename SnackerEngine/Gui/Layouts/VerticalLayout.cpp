#include "Gui/Layouts/VerticalLayout.h"
#include "Utility\Keys.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType<GuiVerticalLayout::VerticalLayoutMode>(const nlohmann::json& json)
	{
		return json.is_string() && (
			json == "CHILD_WIDTH_RANGE" ||
			json == "CHILD_WIDTH_TO_LAYOUT_WIDTH" ||
			json == "LARGEST_PREFERRED_WIDTH");
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiVerticalLayout::VerticalLayoutMode parseJSON(const nlohmann::json& json)
	{
		if (json == "CHILD_WIDTH_RANGE") return GuiVerticalLayout::VerticalLayoutMode::CHILD_WIDTH_RANGE;
		else if (json == "CHILD_WIDTH_TO_LAYOUT_WIDTH") return GuiVerticalLayout::VerticalLayoutMode::CHILD_WIDTH_TO_LAYOUT_WIDTH;
		else if (json == "LARGEST_PREFERRED_WIDTH") return GuiVerticalLayout::VerticalLayoutMode::LARGEST_PREFERRED_WIDTH;
		return GuiVerticalLayout::VerticalLayoutMode::CHILD_WIDTH_RANGE;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalLayout::setAlignmentHorizontal(GuiID childID, AlignmentHorizontal alignmentHorizontal)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		if (index.has_value()) {
			alignmentsHorizontal[index.value()] = alignmentHorizontal;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	int GuiVerticalLayout::computeChildPositionX(int childWidth, AlignmentHorizontal alignmentHorizontal) const
	{
		switch (alignmentHorizontal)
		{
		case SnackerEngine::AlignmentHorizontal::LEFT: return horizontalBorder;
		case SnackerEngine::AlignmentHorizontal::CENTER: return (getWidth() - childWidth) / 2;
		case SnackerEngine::AlignmentHorizontal::RIGHT: return getWidth() - childWidth - horizontalBorder;
		default: return 0;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalLayout::onRegisterChild(AlignmentHorizontal alignmentHorizontal)
	{
		alignmentsHorizontal.push_back(alignmentHorizontal);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalLayout::setShrinkWidthToChildren(bool shrinkWidthToChildren)
	{
		if (this->shrinkWidthToChildren != shrinkWidthToChildren) {
			this->shrinkWidthToChildren = shrinkWidthToChildren;
			if (shrinkWidthToChildren) computeWidthHintsFromChildren();
			else setPreferredWidth(SIZE_HINT_ARBITRARY);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalLayout::computeWidthHintsFromChildren()
	{
		int minWidth = 0;
		std::optional<int> preferredWidth = std::nullopt;
		for (auto childID : getChildren()) {
			GuiElement* child = getElement(childID);
			if (child) {
				// Set minWidth to largest minWidth (+ vertical borders)
				minWidth = std::max(minWidth, child->getMinWidth() + 2 * static_cast<int>(horizontalBorder));
				// If all children have the same preferred width, this will be the layouts preferred width. Otherwise, set
				// preferredWidth to SIZE_HINT_ARBITRARY. If any child has preferredWidth of SIZE_HINT_AS_LARGE_AS_POSSIBLE,
				// set the layouts preferredWidth to SIZE_HINT_AS_LARGE_AS_POSSIBLE
				if (preferredWidth.has_value()) {
					if (preferredWidth.value() >= 0) {
						int tempPreferredWidth = child->getPreferredWidth();
						if (tempPreferredWidth != preferredWidth.value()) preferredWidth = SIZE_HINT_ARBITRARY;
					}
				}
				else {
					int tempPreferredWidth = child->getPreferredWidth();
					if (tempPreferredWidth >= 0) preferredWidth = tempPreferredWidth;
				}
				if (child->getPreferredWidth() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredWidth = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
			}
		}
		setMinWidth(minWidth);
		if ((getResizeMode() == ResizeMode::RESIZE_RANGE || shrinkWidthToChildren) && preferredWidth.has_value() && preferredWidth.value() != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
			setPreferredWidth(preferredWidth.value() + 2 * horizontalBorder);
		}
		else setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout::GuiVerticalLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(verticalLayoutMode, "verticalLayoutMode", json, data, parameterNames);
		parseJsonOrReadFromData(defaultAlignmentHorizontal, "defaultAlignmentHorizontal", json, data, parameterNames);
		parseJsonOrReadFromData(horizontalBorder, "horizontalBorder", json, data, parameterNames);
		parseJsonOrReadFromData(shrinkWidthToChildren, "shrinkWidthToChildren", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout::GuiVerticalLayout(const GuiVerticalLayout& other) noexcept
		: GuiLayout(other), verticalLayoutMode(other.verticalLayoutMode),
		defaultAlignmentHorizontal(other.defaultAlignmentHorizontal),
		alignmentsHorizontal{}, horizontalBorder(other.horizontalBorder),
		shrinkWidthToChildren{ other.shrinkWidthToChildren } {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout& GuiVerticalLayout::operator=(const GuiVerticalLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		verticalLayoutMode = other.verticalLayoutMode;
		defaultAlignmentHorizontal = other.defaultAlignmentHorizontal;
		alignmentsHorizontal.clear();
		horizontalBorder = other.horizontalBorder;
		shrinkWidthToChildren = other.shrinkWidthToChildren;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout::GuiVerticalLayout(GuiVerticalLayout&& other) noexcept
		: GuiLayout(std::move(other)), verticalLayoutMode(other.verticalLayoutMode), 
		defaultAlignmentHorizontal(other.defaultAlignmentHorizontal),
		alignmentsHorizontal(std::move(other.alignmentsHorizontal)), 
		horizontalBorder(other.horizontalBorder),
		shrinkWidthToChildren{ other.shrinkWidthToChildren } {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout& GuiVerticalLayout::operator=(GuiVerticalLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		verticalLayoutMode = other.verticalLayoutMode;
		defaultAlignmentHorizontal = other.defaultAlignmentHorizontal;
		alignmentsHorizontal = std::move(other.alignmentsHorizontal);
		horizontalBorder = other.horizontalBorder;
		shrinkWidthToChildren = other.shrinkWidthToChildren;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalLayout::registerChild(GuiElement& guiElement, AlignmentHorizontal alignmentHorizontal)
	{
		if (GuiLayout::registerChild(guiElement)) {
			onRegisterChild(alignmentHorizontal);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, defaultAlignmentHorizontal);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiVerticalLayout::registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		if (GuiLayout::registerChild(guiElement, json, data, parameterNames)) {
			AlignmentHorizontal alignmentHorizontal = defaultAlignmentHorizontal;
			parseJsonOrReadFromData(alignmentHorizontal, "alignmentHorizontal", json, data, parameterNames);
			onRegisterChild(alignmentHorizontal);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<AlignmentHorizontal> GuiVerticalLayout::getAlignmentHorizontal(GuiID childID)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		return index.has_value() ? alignmentsHorizontal[index.value()] : std::optional<AlignmentHorizontal>();
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<unsigned> GuiVerticalLayout::removeChild(GuiID guiElement)
	{
		std::optional<unsigned> index = GuiLayout::removeChild(guiElement);
		if (index.has_value()) alignmentsHorizontal.erase(alignmentsHorizontal.begin() + index.value());
		return index;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalLayout::enforceLayout()
	{
		/// Set x position and width of children
		const auto& children = getChildren();
		switch (verticalLayoutMode)
		{
		case VerticalLayoutMode::CHILD_WIDTH_RANGE:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				int childWidth = child->getPreferredWidth();
				if (childWidth < 0) childWidth = child->getMaxWidth() >= 0 ? child->getMaxWidth() : getWidth() - 2 * static_cast<int>(horizontalBorder);
				childWidth = std::min(childWidth, getWidth() - 2 * static_cast<int>(horizontalBorder));
				childWidth = child->clampToMinMaxWidth(childWidth);
				child->setWidth(childWidth);
				child->setPositionX(computeChildPositionX(child->getWidth(), alignmentsHorizontal[i]));
			}
			break;
		}
		case VerticalLayoutMode::CHILD_WIDTH_TO_LAYOUT_WIDTH:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				child->setWidth(child->clampToMinMaxWidth(getWidth() - 2 * static_cast<int>(horizontalBorder)));
				child->setPositionX(computeChildPositionX(child->getWidth(), alignmentsHorizontal[i]));
			}
			break;
		}
		case VerticalLayoutMode::LARGEST_PREFERRED_WIDTH:
		{
			// Determine largest preferredWidth
			int largestPreferredWidth = -1;
			for (auto childID : children) {
				GuiElement* child = getElement(childID);
				if (child) {
					int preferredWidth = child->getPreferredWidth();
					if (preferredWidth == SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
						largestPreferredWidth = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
						break;
					}
					else if (preferredWidth > largestPreferredWidth) largestPreferredWidth = preferredWidth;
				}
			}
			largestPreferredWidth = std::min(largestPreferredWidth, getWidth() - 2 * static_cast<int>(horizontalBorder));
			if (largestPreferredWidth == SIZE_HINT_ARBITRARY) largestPreferredWidth = getWidth() - 2 * static_cast<int>(horizontalBorder);
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				child->setWidth(child->clampToMinMaxWidth(largestPreferredWidth));
				child->setPositionX(computeChildPositionX(child->getWidth(), alignmentsHorizontal[i]));
			}
			break;
		}
		}
		computeWidthHintsFromChildren();
	}
	//--------------------------------------------------------------------------------------------------
}