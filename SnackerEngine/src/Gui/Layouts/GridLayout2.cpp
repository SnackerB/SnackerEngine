#include "Gui/Layouts/GridLayout2.h"

namespace SnackerEngine
{

	void GridLayout2::removeChild(GuiElement2& guiElement)
	{
		const auto& children = getChildren();
		auto result = std::find(children.begin(), children.end(), guiElement.getGuiID());
		if (result != children.end()) {
			GuiLayout2::removeChild(guiElement);
			layoutOptions.erase(layoutOptions.begin() + (result - children.begin()));
			enforceLayout();
		}
	}

	void GridLayout2::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i mySize = getSize();
		Vec2i elementSize = Vec2i(mySize.x / totalColumns, mySize.y / totalRows);
		Vec2i lastElementSize = elementSize + Vec2i(mySize.x % totalColumns, mySize.y % totalRows);
		const auto& children = getChildren();
		for (unsigned i = 0; i < children.size(); ++i) {
			// Compute size
			Vec2i currentSize = elementSize;
			if (layoutOptions[i].column == totalColumns) currentSize.x = lastElementSize.x;
			if (layoutOptions[i].row == totalRows) currentSize.y = lastElementSize.y;
			// Compute position
			Vec2i currentPosition = Vec2i(elementSize.x * (layoutOptions[i].column - 1), elementSize.y * (layoutOptions[i].row - 1));
			// Set attributes and enforce layouts
			setPositionAndSizeWithoutEnforcingLayouts(children[i], position, size);
			enforceLayoutOnElement(children[i]);
		}
	}

	GridLayout2::GridLayout2(unsigned totalColumns, unsigned totalRows)
		: totalColumns(totalColumns), totalRows(totalRows), layoutOptions{} {}

	bool GridLayout2::registerChild(GuiElement2& guiElement, const unsigned& row, const unsigned& column)
	{
		if (GuiLayout2::registerChildWithoutEnforcingLayouts(guiElement)) {
			layoutOptions.push_back({ row, column });
			enforceLayout();
			return true;
		}
		return false;
	}

	bool GridLayout2::registerChild(GuiElement2& guiElement)
	{
		return registerChild(guiElement, 0, 0);
	}

	GridLayout2::GridLayout2(const GridLayout2& other) noexcept
		: GuiLayout2(other), totalColumns(other.totalColumns), totalRows(other.totalRows), layoutOptions{} {}

	GridLayout2& GridLayout2::operator=(const GridLayout2& other) noexcept
	{
		GuiLayout2::operator=(other);
		totalColumns = other.totalColumns;
		totalRows = other.totalRows;
		layoutOptions.clear();
		return *this;
	}

	GridLayout2::GridLayout2(GridLayout2&& other) noexcept
		: GuiLayout2(std::move(other)), totalColumns(other.totalColumns), totalRows(other.totalRows), layoutOptions(other.layoutOptions)
	{
		other.layoutOptions.clear();
	}

	GridLayout2& GridLayout2::operator=(GridLayout2&& other) noexcept
	{
		GuiLayout2::operator=(std::move(other));
		totalColumns = other.totalColumns;
		totalRows = other.totalRows;
		layoutOptions = other.layoutOptions;
		other.layoutOptions.clear();
		return *this;
	}

}