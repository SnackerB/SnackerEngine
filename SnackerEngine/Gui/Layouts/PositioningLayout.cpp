#include "Gui/Layouts/PositioningLayout.h"
#include "Core\Log.h"

namespace SnackerEngine
{
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
		auto border = parseJsonOrReadFromData<int>("border", json, data, parameterNames);
		if (border.has_value()) {
			leftBorder = border.value();
			rightBorder = border.value();
			topBorder = border.value();
			bottomBorder = border.value();
		}
		parseJsonOrReadFromData(leftBorder, "leftBorder", json, data, parameterNames);
		parseJsonOrReadFromData(rightBorder, "rightBorder", json, data, parameterNames);
		parseJsonOrReadFromData(topBorder, "topBorder", json, data, parameterNames);
		parseJsonOrReadFromData(bottomBorder, "bottomBorder", json, data, parameterNames);
		parseJsonOrReadFromData(shrinkWidthToChildren, "shrinkWidthToChildren", json, data, parameterNames);
		parseJsonOrReadFromData(shrinkHeightToChildren, "shrinkHeightToChildren", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPositioningLayout::setLeftBorder(int leftBorder)
	{
		if (this->leftBorder != leftBorder) {
			this->leftBorder = leftBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPositioningLayout::setRightBorder(int rightBorder)
	{
		if (this->rightBorder != rightBorder) {
			this->rightBorder = rightBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPositioningLayout::setTopBorder(int topBorder)
	{
		if (this->topBorder != topBorder) {
			this->topBorder = topBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPositioningLayout::setBottomBorder(int bottomBorder)
	{
		if (this->bottomBorder != bottomBorder) {
			this->bottomBorder = bottomBorder;
			registerEnforceLayoutDown();
		}
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
			Vec2i childPosition{};
			if (child->getResizeMode() == ResizeMode::SAME_AS_PARENT) {
				childSize = mySize;
				childPosition = Vec2i(0, 0);
			}
			else {
				if (childSize.x < 0) childSize.x = mySize.x - leftBorder - rightBorder;
				if (childSize.y < 0) childSize.y = mySize.y - topBorder - bottomBorder;
				if (childSize.x > mySize.x - leftBorder - rightBorder) childSize.x = mySize.x - leftBorder - rightBorder;
				if (childSize.y > mySize.y - topBorder - bottomBorder) childSize.y = mySize.y - topBorder - bottomBorder;
				// Compute position
				switch (mode)
				{
				case Mode::CENTER:
					childPosition.x = (mySize.x - childSize.x) / 2;
					childPosition.y = (mySize.y - childSize.y) / 2;
					break;
				case Mode::LEFT:
					childPosition.x = leftBorder;
					childPosition.y = (mySize.y - childSize.y) / 2;
					break;
				case Mode::TOP_LEFT:
					childPosition.x = leftBorder;
					childPosition.y = topBorder;
					break;
				case Mode::TOP:
					childPosition.x = (mySize.x - childSize.x) / 2;
					childPosition.y = topBorder;
					break;
				case Mode::TOP_RIGHT:
					childPosition.x = mySize.x - childSize.x - rightBorder;
					childPosition.y = topBorder;
					break;
				case Mode::RIGHT:
					childPosition.x = mySize.x - childSize.x - rightBorder;
					childPosition.y = (mySize.y - childSize.y) / 2;
					break;
				case Mode::BOTTOM_RIGHT:
					childPosition.x = mySize.x - childSize.x - rightBorder;
					childPosition.y = mySize.y - childSize.y - bottomBorder;
					break;
				case Mode::BOTTOM:
					childPosition.x = (mySize.x - childSize.x) / 2;
					childPosition.y = mySize.y - childSize.y - bottomBorder;
					break;
				case Mode::BOTTOM_LEFT:
					childPosition.x = leftBorder;
					childPosition.y = mySize.y - childSize.y - bottomBorder;
					break;
				default:
					break;
				}
				childPosition.x = std::min(childPosition.x, mySize.x - rightBorder - childSize.x);
				childPosition.x = std::max(childPosition.x, leftBorder);
				childPosition.y = std::min(childPosition.y, mySize.y - bottomBorder - childSize.y);
				childPosition.y = std::max(childPosition.y, topBorder);
			}
			// Enforce Layout
			setPositionAndSizeOfChild(childID, childPosition, childSize);
		}
		// Check if we need to modify sizeHints
		if (shrinkWidthToChildren) {
			// Find smallest minWidth and largest preferredWidth and maxWidth
			int minWidth = 0;
			int preferredWidth = SIZE_HINT_ARBITRARY;
			int maxWidth = SIZE_HINT_ARBITRARY;
			for (GuiID childID : children)
			{
				GuiElement* child = getElement(childID);
				if (!child) continue;
				minWidth = std::max(minWidth, child->getMinWidth());
				if (preferredWidth != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					if (child->getPreferredWidth() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredWidth = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
					else if (child->getPreferredWidth() != SIZE_HINT_ARBITRARY) preferredWidth = std::max(preferredWidth, child->getPreferredWidth());
				}
				if (child->getMaxWidth() >= 0) {
					if (maxWidth == SIZE_HINT_ARBITRARY) maxWidth = child->getMaxWidth();
					else maxWidth = std::min(maxWidth, child->getMaxWidth());
				}
			}
			setMinWidth(minWidth + leftBorder + rightBorder);
			if (preferredWidth >= 0) preferredWidth += leftBorder + rightBorder;
			setPreferredWidth(preferredWidth);
			if (maxWidth >= 0) maxWidth += leftBorder + rightBorder;
			setMaxWidth(maxWidth);
		}
		if (shrinkHeightToChildren) {
			// Find smallest minHeight and largest preferredHeight and maxHeight
			int minHeight = 0;
			int preferredHeight = SIZE_HINT_ARBITRARY;
			int maxHeight = SIZE_HINT_ARBITRARY;
			for (GuiID childID : children)
			{
				GuiElement* child = getElement(childID);
				if (!child) continue;
				minHeight = std::max(minHeight, child->getMinHeight());
				if (preferredHeight != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					if (child->getPreferredHeight() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredHeight = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
					else if (child->getPreferredHeight() != SIZE_HINT_ARBITRARY) preferredHeight = std::max(preferredHeight, child->getPreferredHeight());
				}
				if (child->getMaxHeight() >= 0) {
					if (maxHeight == SIZE_HINT_ARBITRARY) maxHeight = child->getMaxHeight();
					else maxHeight = std::min(maxHeight, child->getMaxHeight());
				}
			}
			setMinHeight(minHeight + topBorder + bottomBorder);
			if (preferredHeight >= 0) preferredHeight += topBorder + bottomBorder;
			setPreferredHeight(preferredHeight);
			if (maxHeight >= 0) maxHeight += topBorder + bottomBorder;
			setMaxHeight(maxHeight);
		}
	}
	//--------------------------------------------------------------------------------------------------
}