#include "Gui/Layouts/HorizontalLayout.h"
#include "core/Keys.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType<GuiHorizontalLayout::HorizontalLayoutMode>(const nlohmann::json& json)
	{
		return json.is_string() && (
			json == "VARIABLE_HEIGHT" ||
			json == "FORCED_HEIGHT" ||
			json == "FORCE_CHILD_HEIGHT"
			);
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiHorizontalLayout::HorizontalLayoutMode parseJSON(const nlohmann::json& json)
	{
		if (json == "VARIABLE_HEIGHT") return GuiHorizontalLayout::HorizontalLayoutMode::VARIABLE_HEIGHT;
		else if (json == "FORCED_HEIGHT") return GuiHorizontalLayout::HorizontalLayoutMode::FORCED_HEIGHT;
		else if (json == "FORCE_CHILD_HEIGHT") return GuiHorizontalLayout::HorizontalLayoutMode::FORCE_CHILD_HEIGHT;
		return GuiHorizontalLayout::HorizontalLayoutMode::VARIABLE_HEIGHT;
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
		case SnackerEngine::GuiHorizontalLayout::HorizontalLayoutMode::VARIABLE_HEIGHT:
		{
			int minHeight = 0;
			int preferredHeight = -1;
			int maxHeight = 0;
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				if (child->getMinHeight() + 2 * static_cast<int>(verticalBorder) > minHeight) minHeight = child->getMinHeight() + 2 * verticalBorder;
				if (child->getPreferredHeight() != -1 && child->getPreferredHeight() + static_cast<int>(2 * verticalBorder) > preferredHeight) preferredHeight = child->getPreferredHeight() + 2 * verticalBorder;
				if (maxHeight != -1) {
					if (child->getMaxHeight() == -1) maxHeight = -1;
					else if (child->getMaxHeight() + 2 * static_cast<int>(verticalBorder) > maxHeight) maxHeight = child->getMaxHeight() + 2 * verticalBorder;
				}
				int childHeight = child->getPreferredHeight() != -1 ? child->getPreferredHeight() : child->getHeight();
				childHeight = std::min(childHeight, getHeight() - 2 * static_cast<int>(verticalBorder));
				childHeight = child->clampToMinMaxHeight(childHeight);
				child->setHeight(childHeight);
				child->setPositionY(computeChildPositionY(child->getHeight(), alignmentsVertical[i]));
			}
			if (preferredHeight != -1 && preferredHeight < minHeight) preferredHeight = minHeight;
			setMinHeight(minHeight);
			setPreferredHeight(preferredHeight);
			setMaxHeight(maxHeight);
			break;
		}
		case SnackerEngine::GuiHorizontalLayout::HorizontalLayoutMode::FORCED_HEIGHT:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				int childHeight = child->getPreferredHeight() != -1 ? child->getPreferredHeight() : child->getHeight();
				childHeight = std::min(childHeight, getHeight() - 2 * static_cast<int>(verticalBorder));
				childHeight = child->clampToMinMaxHeight(childHeight);
				child->setHeight(childHeight);
				child->setPositionY(computeChildPositionY(child->getHeight(), alignmentsVertical[i]));
			}
			break;
		}
		case SnackerEngine::GuiHorizontalLayout::HorizontalLayoutMode::FORCE_CHILD_HEIGHT:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				child->setHeight(child->clampToMinMaxHeight(getHeight() - 2 * static_cast<int>(verticalBorder)));
				child->setPositionY(computeChildPositionY(child->getHeight(), alignmentsVertical[i]));
			}
			break;
		}
		default:
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------
}