#include "Gui/Layouts/HorizontalLayout.h"
#include "core/Keys.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	unsigned GuiHorizontalLayout::defaultVerticalBorder = 0;
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType<GuiHorizontalLayout::HorizontalLayoutMode>(const nlohmann::json& json)
	{
		return json.is_string() && (
			json == "CHILD_HEIGHT_RANGE" ||
			json == "CHILD_HEIGHT_TO_LAYOUT_HEIGHT");
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiHorizontalLayout::HorizontalLayoutMode parseJSON(const nlohmann::json& json)
	{
		if (json == "CHILD_HEIGHT_RANGE") return GuiHorizontalLayout::HorizontalLayoutMode::CHILD_HEIGHT_RANGE;
		else if (json == "CHILD_HEIGHT_TO_LAYOUT_HEIGHT") return GuiHorizontalLayout::HorizontalLayoutMode::CHILD_HEIGHT_TO_LAYOUT_HEIGHT;
		return GuiHorizontalLayout::HorizontalLayoutMode::CHILD_HEIGHT_RANGE;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalLayout::setAlignmentVertical(GuiID childID, AlignmentVertical alignmentVertical)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID);
		if (index.has_value()) {
			alignmentsVertical[index.value()] = alignmentVertical;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	int GuiHorizontalLayout::computeChildPositionY(int childHeight, AlignmentVertical alignmentVertical) const
	{
		switch (alignmentVertical)
		{
		case SnackerEngine::AlignmentVertical::TOP: return verticalBorder;
		case SnackerEngine::AlignmentVertical::CENTER: return (getHeight() - childHeight) / 2;
		case SnackerEngine::AlignmentVertical::BOTTOM: return getHeight() - childHeight - verticalBorder;
		default: return 0;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalLayout::onRegisterChild(AlignmentVertical alignmentVertical)
	{
		alignmentsVertical.push_back(alignmentVertical);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalLayout::computeHeightHintsFromChildren()
	{
		int minHeight = 0;
		std::optional<int> preferredHeight = std::nullopt;
		for (auto childID : getChildren()) {
			GuiElement* child = getElement(childID);
			if (child) {
				// Set minHeight to largest minHeight (+ vertical borders)
				minHeight = std::max(minHeight, child->getMinHeight() + 2 * static_cast<int>(verticalBorder));
				// If all children have the same preferred height, this will be the layouts preferred height. Otherwise, set
				// preferredHeight to SIZE_HINT_ARBITRARY. If any child has preferredHeight of SIZE_HINT_AS_LARGE_AS_POSSIBLE,
				// set the layouts preferredHeight to SIZE_HINT_AS_LARGE_AS_POSSIBLE
				if (preferredHeight.has_value()) {
					if (preferredHeight.value() >= 0) {
						int tempPreferredHeight = child->getPreferredHeight();
						if (tempPreferredHeight != preferredHeight.value()) preferredHeight = SIZE_HINT_ARBITRARY;
					}
				}
				else {
					int tempPreferredHeight = child->getPreferredHeight();
					if (tempPreferredHeight >= 0) preferredHeight = tempPreferredHeight;
				}
				if (child->getPreferredHeight() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredHeight = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
			}
		}
		setMinHeight(minHeight);
		if (preferredHeight.has_value()) setPreferredHeight(preferredHeight.value());
		else setPreferredHeight(SIZE_HINT_ARBITRARY);
	}	
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalLayout::GuiHorizontalLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(horizontalLayoutMode, "horizontalLayoutMode", json, data, parameterNames);
		parseJsonOrReadFromData(defaultAlignmentVertical, "defaultAlignmentVertical", json, data, parameterNames);
		parseJsonOrReadFromData(verticalBorder, "verticalBorder", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalLayout::GuiHorizontalLayout(const GuiHorizontalLayout& other) noexcept
		: GuiLayout(other), horizontalLayoutMode(other.horizontalLayoutMode), defaultAlignmentVertical(other.defaultAlignmentVertical), alignmentsVertical{}, verticalBorder(other.verticalBorder) {}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalLayout& GuiHorizontalLayout::operator=(const GuiHorizontalLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		horizontalLayoutMode = other.horizontalLayoutMode;
		defaultAlignmentVertical = other.defaultAlignmentVertical;
		alignmentsVertical.clear();
		verticalBorder = other.verticalBorder;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalLayout::GuiHorizontalLayout(GuiHorizontalLayout&& other) noexcept
		: GuiLayout(std::move(other)), horizontalLayoutMode(other.horizontalLayoutMode), defaultAlignmentVertical(other.defaultAlignmentVertical),
		alignmentsVertical(std::move(other.alignmentsVertical)), verticalBorder(other.verticalBorder) {}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalLayout& GuiHorizontalLayout::operator=(GuiHorizontalLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		horizontalLayoutMode = other.horizontalLayoutMode;
		defaultAlignmentVertical = other.defaultAlignmentVertical;
		alignmentsVertical = std::move(other.alignmentsVertical);
		verticalBorder = other.verticalBorder;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalLayout::registerChild(GuiElement& guiElement, AlignmentVertical alignmentVertical)
	{
		if (GuiLayout::registerChild(guiElement)) {
			onRegisterChild(alignmentVertical);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, defaultAlignmentVertical);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiHorizontalLayout::registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		if (GuiLayout::registerChild(guiElement, json, data, parameterNames)) {
			AlignmentVertical alignmentVertical = defaultAlignmentVertical;
			parseJsonOrReadFromData(alignmentVertical, "alignmentVertical", json, data, parameterNames);
			onRegisterChild(alignmentVertical);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<AlignmentVertical> GuiHorizontalLayout::getAlignmentVertical(GuiID childID)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(childID); 
		return index.has_value() ? alignmentsVertical[index.value()] : std::optional<AlignmentVertical>();
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<unsigned> GuiHorizontalLayout::removeChild(GuiID guiElement)
	{
		std::optional<unsigned> index = GuiLayout::removeChild(guiElement);
		if (index.has_value()) alignmentsVertical.erase(alignmentsVertical.begin() + index.value());
		return index;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalLayout::enforceLayout()
	{
		/// Set y position and height of children
		const auto& children = getChildren();
		switch (horizontalLayoutMode)
		{
		case HorizontalLayoutMode::CHILD_HEIGHT_RANGE:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				int childHeight = child->getPreferredHeight();
				if (childHeight < 0) childHeight = child->getMaxHeight() >= 0 ? child->getMaxHeight() : getHeight() - 2 * static_cast<int>(verticalBorder);
				childHeight = std::min(childHeight, getHeight() - 2 * static_cast<int>(verticalBorder));
				childHeight = child->clampToMinMaxHeight(childHeight);
				child->setHeight(childHeight);
				child->setPositionY(computeChildPositionY(child->getHeight(), alignmentsVertical[i]));
			}
			break;
		}
		case HorizontalLayoutMode::CHILD_HEIGHT_TO_LAYOUT_HEIGHT:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				child->setHeight(child->clampToMinMaxHeight(getHeight() - 2 * static_cast<int>(verticalBorder)));
				child->setPositionY(computeChildPositionY(child->getHeight(), alignmentsVertical[i]));
			}
			break;
		}
		}
		computeHeightHintsFromChildren();
	}
	//--------------------------------------------------------------------------------------------------
}