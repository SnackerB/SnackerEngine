#include "Gui/Layouts/PositioningLayout.h"
#include "Core\Log.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiPositioningLayout::computeSizeHintsFromChildren()
	{
		Vec2i minSize = 0;
		std::optional<int> preferredWidth = std::nullopt;
		std::optional<int> preferredHeight = std::nullopt;
		for (auto childID : getChildren()) {
			GuiElement* child = getElement(childID);
			if (child) {
				// Set minSize to largest minSize
				minSize.x = std::max(minSize.x, child->getMinWidth());
				minSize.y = std::max(minSize.y, child->getMinHeight());
				// If all children have the same preferred width/height, this will be the layouts preferred width/height. 
				// Otherwise, set the preferred width/height to -1 (arbitrary)
				if (preferredWidth.has_value()) {
					if (preferredWidth.value() != -1) {
						int tempPreferredWidth = child->getPreferredWidth();
						if (tempPreferredWidth != preferredWidth.value()) preferredWidth = -1;
					}
				}
				else {
					int tempPreferredWidth = child->getPreferredWidth();
					if (tempPreferredWidth != -1) preferredWidth = tempPreferredWidth;
				}
				if (preferredHeight.has_value()) {
					if (preferredHeight.value() != -1) {
						int tempPreferredHeight = child->getPreferredHeight();
						if (tempPreferredHeight != preferredHeight.value()) preferredHeight = -1;
					}
				}
				else {
					int tempPreferredHeight = child->getPreferredHeight();
					if (tempPreferredHeight != -1) preferredHeight = tempPreferredHeight;
				}
			}
		}
		setMinSize(minSize);
		if (preferredHeight.has_value()) setPreferredHeight(preferredHeight.value());
		else setPreferredHeight(-1);
		if (preferredWidth.has_value()) setPreferredWidth(preferredWidth.value());
		else setPreferredWidth(-1);
	}
	//--------------------------------------------------------------------------------------------------
	GuiPositioningLayout::GuiPositioningLayout(Mode mode)
		: GuiLayout(), mode(mode) {}
	//--------------------------------------------------------------------------------------------------
	/// Helper functions for parsing JSON
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiPositioningLayout::Mode> tag)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "GuiPositioningLayout::Mode has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "CENTER" ||
			json == "LEFT" ||
			json == "TOP_LEFT" ||
			json == "TOP" ||
			json == "TOP_RIGHT" ||
			json == "RIGHT" ||
			json == "BOTTOM_RIGHT" ||
			json == "BOTTOM" ||
			json == "BOTTOM_LEFT") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid GuiPositioningLayout::Mode." << LOGGER::ENDL;
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiPositioningLayout::Mode parseJSON(const nlohmann::json& json, JsonTag<GuiPositioningLayout::Mode> tag)
	{
		if (json == "CENTER") return GuiPositioningLayout::Mode::CENTER;
		if (json == "LEFT") return GuiPositioningLayout::Mode::LEFT;
		if (json == "TOP_LEFT") return GuiPositioningLayout::Mode::TOP_LEFT;
		if (json == "TOP") return GuiPositioningLayout::Mode::TOP;
		if (json == "TOP_RIGHT") return GuiPositioningLayout::Mode::TOP_RIGHT;
		if (json == "RIGHT") return GuiPositioningLayout::Mode::RIGHT;
		if (json == "BOTTOM_RIGHT") return GuiPositioningLayout::Mode::BOTTOM_RIGHT;
		if (json == "BOTTOM") return GuiPositioningLayout::Mode::BOTTOM;
		if (json == "BOTTOM_LEFT") return GuiPositioningLayout::Mode::BOTTOM_LEFT;
		return GuiPositioningLayout::Mode::CENTER;
	}
	//--------------------------------------------------------------------------------------------------
	GuiPositioningLayout::GuiPositioningLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiLayout(json, data, parameterNames), mode(Mode::CENTER)
	{
		parseJsonOrReadFromData(mode, "mode", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiPositioningLayout::GuiPositioningLayout(const GuiPositioningLayout& other) noexcept
		: GuiLayout(other), mode(other.mode) {}
	//--------------------------------------------------------------------------------------------------
	GuiPositioningLayout& GuiPositioningLayout::operator=(const GuiPositioningLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		mode = other.mode;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiPositioningLayout::GuiPositioningLayout(GuiPositioningLayout&& other) noexcept
		: GuiLayout(std::move(other)), mode(other.mode) {}
	//--------------------------------------------------------------------------------------------------
	GuiPositioningLayout& GuiPositioningLayout::operator=(GuiPositioningLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		mode = other.mode;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPositioningLayout::setMode(Mode mode)
	{
		if (this->mode != mode) {
			this->mode = mode; 
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPositioningLayout::enforceLayout()
	{
		const std::vector<GuiID>& children = getChildren();
		if (children.empty()) return;
		const Vec2i& mySize = getSize();
		for (GuiID childID : children) 
		{
			GuiElement* child = getElement(childID);
			if (!child) continue;
			Vec2i childSize = child->getPreferredSize();
			if (child->getResizeMode() == ResizeMode::SAME_AS_PARENT) childSize = getSize();
			else {
				if (childSize.x < 0) childSize.x = getWidth();
				if (childSize.y < 0) childSize.y = getHeight();
			}
			if (childSize.x > getWidth()) childSize.x = getWidth();
			if (childSize.y > getHeight()) childSize.y = getHeight();
			childSize = child->clampToMinMaxSize(childSize);
			// Compute position
			Vec2i childPosition;
			switch (mode)
			{
			case Mode::CENTER:
				childPosition.x = (mySize.x - childSize.x) / 2;
				childPosition.y = (mySize.y - childSize.y) / 2;
				break;
			case Mode::LEFT:
				childPosition.x = 0;
				childPosition.y = (mySize.y - childSize.y) / 2;
				break;
			case Mode::TOP_LEFT:
				childPosition.x = 0;
				childPosition.y = 0;
				break;
			case Mode::TOP:
				childPosition.x = (mySize.x - childSize.x) / 2;
				childPosition.y = 0;
				break;
			case Mode::TOP_RIGHT:
				childPosition.x = mySize.x - childSize.x;
				childPosition.y = 0;
				break;
			case Mode::RIGHT:
				childPosition.x = mySize.x - childSize.x;
				childPosition.y = (mySize.y - childSize.y) / 2;
				break;
			case Mode::BOTTOM_RIGHT:
				childPosition.x = mySize.x - childSize.x;
				childPosition.y = mySize.y - childSize.y;
				break;
			case Mode::BOTTOM:
				childPosition.x = (mySize.x - childSize.x) / 2;
				childPosition.y = mySize.y - childSize.y;
				break;
			case Mode::BOTTOM_LEFT:
				childPosition.x = 0;
				childPosition.y = mySize.y - childSize.y;
				break;
			default:
				break;
			}
			// Enforce Layout
			setPositionAndSizeOfChild(childID, childPosition, childSize);
		}
		computeSizeHintsFromChildren();
	}
	//--------------------------------------------------------------------------------------------------
}