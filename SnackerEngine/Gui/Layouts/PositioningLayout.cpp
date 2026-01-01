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
		parseJsonOrReadFromData(leftBorder, "leftBorder", json, data, parameterNames);
		parseJsonOrReadFromData(rightBorder, "rightBorder", json, data, parameterNames);
		parseJsonOrReadFromData(topBorder, "topBorder", json, data, parameterNames);
		parseJsonOrReadFromData(bottomBorder, "bottomBorder", json, data, parameterNames);
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
				childPosition.x = std::min(childPosition.x, mySize.x - leftBorder - childSize.x);
				childPosition.x = std::max(childPosition.x, rightBorder);
				childPosition.y = std::min(childPosition.y, mySize.y - bottomBorder - childSize.y);
				childPosition.y = std::max(childPosition.y, topBorder);
			}
			// Enforce Layout
			setPositionAndSizeOfChild(childID, childPosition, childSize);
		}
	}
	//--------------------------------------------------------------------------------------------------
}