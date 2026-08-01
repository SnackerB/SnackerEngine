#include "Gui\Layouts\GuiWrapperLayout.h"

namespace SnackerEngine
{

	GuiWrapperLayout::GuiWrapperLayout(const nlohmann::json& json, ParameterNameSet* parameterNameSet)
		: GuiLayout(json, parameterNameSet)
	{
		auto border = parseJson<int>("border", json, parameterNameSet);
		if (border.has_value()) {
			leftBorder = border.value();
			rightBorder = border.value();
			topBorder = border.value();
			bottomBorder = border.value();
		}
		parseJson(leftBorder, "leftBorder", json, parameterNameSet);
		parseJson(rightBorder, "rightBorder", json, parameterNameSet);
		parseJson(topBorder, "topBorder", json, parameterNameSet);
		parseJson(bottomBorder, "bottomBorder", json, parameterNameSet);
	}

	void GuiWrapperLayout::setBorder(int border)
	{
		setLeftBorder(border);
		setRightBorder(border);
		setTopBorder(border);
		setBottomBorder(border);
	}
	
	void GuiWrapperLayout::setLeftBorder(int leftBorder)
	{
		if (this->leftBorder != leftBorder) {
			this->leftBorder = leftBorder;
			registerEnforceLayoutDown();
		}
	}
	
	void GuiWrapperLayout::setRightBorder(int rightBorder)
	{
		if (this->rightBorder != rightBorder) {
			this->rightBorder = rightBorder;
			registerEnforceLayoutDown();
		}
	}
	
	void GuiWrapperLayout::setTopBorder(int topBorder)
	{
		if (this->topBorder != topBorder) {
			this->topBorder = topBorder;
			registerEnforceLayoutDown();
		}
	}
	
	void GuiWrapperLayout::setBottomBorder(int bottomBorder)
	{
		if (this->bottomBorder != bottomBorder) {
			this->bottomBorder = bottomBorder;
			registerEnforceLayoutDown();
		}
	}

	void GuiWrapperLayout::enforceLayout()
	{
		const std::vector<GuiID>& children = getChildren();
		if (children.empty()) return;
		Vec2i childSize = getSize();
		childSize.x -= leftBorder + rightBorder;
		childSize.y -= topBorder + bottomBorder;
		for (GuiID childID : children)
		{
			setPositionAndSizeOfChild(childID, Vec2i(leftBorder, topBorder), childSize);
		}
		// Compute size hints: Find smallest minWidth and largest preferredWidth and maxWidth
		Vec2i minSize = Vec2i(0, 0);
		Vec2i preferredSize = Vec2i(SIZE_HINT_ARBITRARY, SIZE_HINT_ARBITRARY);
		Vec2i maxSize = Vec2i(SIZE_HINT_ARBITRARY, SIZE_HINT_ARBITRARY);
		for (GuiID childID : children)
		{
			GuiElement* child = getElement(childID);
			if (!child) continue;
			minSize.x = std::max(minSize.x, child->getMinWidth());
			minSize.y = std::max(minSize.y, child->getMinHeight());
			if (preferredSize.x != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
				if (child->getPreferredWidth() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredSize.x = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				else if (child->getPreferredWidth() != SIZE_HINT_ARBITRARY) preferredSize.x = std::max(preferredSize.x, child->getPreferredWidth());
			}
			if (preferredSize.y != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
				if (child->getPreferredHeight() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) preferredSize.y = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				else if (child->getPreferredHeight() != SIZE_HINT_ARBITRARY) preferredSize.y = std::max(preferredSize.y, child->getPreferredHeight());
			}
			if (child->getMaxWidth() >= 0) {
				if (maxSize.x == SIZE_HINT_ARBITRARY) maxSize.x = child->getMaxWidth();
				else maxSize.x = std::min(maxSize.x, child->getMaxWidth());
			}
			if (child->getMaxHeight() >= 0) {
				if (maxSize.y == SIZE_HINT_ARBITRARY) maxSize.y = child->getMaxHeight();
				else maxSize.y = std::min(maxSize.y, child->getMaxHeight());
			}
		}
		minSize += Vec2i(leftBorder + rightBorder, topBorder + bottomBorder);
		setMinSize(minSize);
		if (preferredSize.x >= 0) preferredSize.x += leftBorder + rightBorder;
		if (preferredSize.y >= 0) preferredSize.y += topBorder + bottomBorder;
		setPreferredSize(preferredSize);
		if (maxSize.x >= 0) maxSize.x += leftBorder + rightBorder;
		if (maxSize.y >= 0) maxSize.y += topBorder + bottomBorder;
		setMaxSize(maxSize);
	}
}