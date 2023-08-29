#include "Gui/Layouts/VerticalLayout.h"
#include "Core/Keys.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType<GuiVerticalLayout::VerticalLayoutMode>(const nlohmann::json& json)
	{
		return json.is_string() && (
			json == "VARIABLE_WIDTH" ||
			json == "FORCED_WIDTH" ||
			json == "FORCE_CHILD_WIDTH"
			);
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiVerticalLayout::VerticalLayoutMode parseJSON(const nlohmann::json& json)
	{
		if (json == "VARIABLE_WIDTH") return GuiVerticalLayout::VerticalLayoutMode::VARIABLE_WIDTH;
		else if (json == "FORCED_WIDTH") return GuiVerticalLayout::VerticalLayoutMode::FORCED_WIDTH;
		else if (json == "FORCE_CHILD_WIDTH") return GuiVerticalLayout::VerticalLayoutMode::FORCE_CHILD_WIDTH;
		return GuiVerticalLayout::VerticalLayoutMode::VARIABLE_WIDTH;
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
	GuiVerticalLayout::GuiVerticalLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(verticalLayoutMode, "verticalLayoutMode", json, data, parameterNames);
		parseJsonOrReadFromData(defaultAlignmentHorizontal, "defaultAlignmentHorizontal", json, data, parameterNames);
		parseJsonOrReadFromData(horizontalBorder, "horizontalBorder", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout::GuiVerticalLayout(const GuiVerticalLayout& other) noexcept
		: GuiLayout(other), verticalLayoutMode(other.verticalLayoutMode), defaultAlignmentHorizontal(other.defaultAlignmentHorizontal), alignmentsHorizontal{}, horizontalBorder(other.horizontalBorder) {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout& GuiVerticalLayout::operator=(const GuiVerticalLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		verticalLayoutMode = other.verticalLayoutMode;
		defaultAlignmentHorizontal = other.defaultAlignmentHorizontal;
		alignmentsHorizontal.clear();
		horizontalBorder = other.horizontalBorder;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout::GuiVerticalLayout(GuiVerticalLayout&& other) noexcept
		: GuiLayout(std::move(other)), verticalLayoutMode(other.verticalLayoutMode), defaultAlignmentHorizontal(other.defaultAlignmentHorizontal),
		alignmentsHorizontal(std::move(other.alignmentsHorizontal)), horizontalBorder(other.horizontalBorder) {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalLayout& GuiVerticalLayout::operator=(GuiVerticalLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		verticalLayoutMode = other.verticalLayoutMode;
		defaultAlignmentHorizontal = other.defaultAlignmentHorizontal;
		alignmentsHorizontal = std::move(other.alignmentsHorizontal);
		horizontalBorder = other.horizontalBorder;
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
		case SnackerEngine::GuiVerticalLayout::VerticalLayoutMode::VARIABLE_WIDTH:
		{
			int minWidth= 0;
			int preferredWidth = -1;
			int maxWidth = 0;
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				if (child->getMinWidth() + 2 * static_cast<int>(horizontalBorder) > minWidth) minWidth = child->getMinWidth() + 2 * horizontalBorder;
				if (child->getPreferredWidth() + 2 * static_cast<int>(horizontalBorder) > preferredWidth) preferredWidth = child->getPreferredWidth() + 2 * horizontalBorder;
				if (maxWidth != -1) {
					if (child->getMaxWidth() == -1) maxWidth = -1;
					else if (child->getMaxWidth() + 2 * static_cast<int>(horizontalBorder) > maxWidth) maxWidth = child->getMaxWidth() + 2 * horizontalBorder;
				}
				int childWidth = child->getPreferredWidth() != -1 ? child->getPreferredWidth() : child->getWidth();
				childWidth = std::min(childWidth, getWidth() - 2 * static_cast<int>(horizontalBorder));
				childWidth = child->clampToMinMaxWidth(childWidth);
				child->setWidth(childWidth);
				child->setPositionX(computeChildPositionX(child->getWidth(), alignmentsHorizontal[i]));
			}
			if (preferredWidth != -1 && preferredWidth < minWidth) preferredWidth = minWidth;
			setMinWidth(minWidth);
			setPreferredWidth(preferredWidth);
			setMaxWidth(maxWidth);
			break;
		}
		case SnackerEngine::GuiVerticalLayout::VerticalLayoutMode::FORCED_WIDTH:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				int childWidth = child->getPreferredWidth() != -1 ? child->getPreferredWidth() : child->getWidth();
				childWidth = std::min(childWidth, getWidth() - 2 * static_cast<int>(horizontalBorder));
				childWidth = child->clampToMinMaxWidth(childWidth);
				child->setWidth(childWidth);
				child->setPositionX(computeChildPositionX(child->getWidth(), alignmentsHorizontal[i]));
			}
			break;
		}
		case SnackerEngine::GuiVerticalLayout::VerticalLayoutMode::FORCE_CHILD_WIDTH:
		{
			for (unsigned i = 0; i < children.size(); ++i) {
				GuiElement* child = getElement(children[i]);
				if (!child) continue;
				child->setWidth(child->clampToMinMaxWidth(getWidth() - 2 * static_cast<int>(horizontalBorder)));
				child->setPositionX(computeChildPositionX(child->getWidth(), alignmentsHorizontal[i]));
			}
			break;
		}
		default:
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------
}