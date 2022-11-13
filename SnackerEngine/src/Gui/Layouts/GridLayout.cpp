#include "Gui/Layouts/GridLayout.h"

namespace SnackerEngine
{

	void GridLayout::removeChild(GuiElement& guiElement)
	{
		const auto& children = getChildren();
		auto result = std::find(children.begin(), children.end(), guiElement.getGuiID());
		if (result != children.end()) {
			GuiLayout::removeChild(guiElement);
			layoutOptions.erase(layoutOptions.begin() + (result - children.begin()));
			enforceLayout();
		}
	}

	void GridLayout::enforceLayout()
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

	GridLayout::GridLayout(unsigned totalColumns, unsigned totalRows)
		: totalColumns(totalColumns), totalRows(totalRows), layoutOptions{} {}

	bool GridLayout::registerChild(GuiElement& guiElement, const unsigned& row, const unsigned& column)
	{
		if (GuiLayout::registerChildWithoutEnforcingLayouts(guiElement)) {
			layoutOptions.push_back({ row, column });
			enforceLayout();
			return true;
		}
		return false;
	}

	bool GridLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, 0, 0);
	}

	GridLayout::GridLayout(const GridLayout& other) noexcept
		: GuiLayout(other), totalColumns(other.totalColumns), totalRows(other.totalRows), layoutOptions{} {}

	GridLayout& GridLayout::operator=(const GridLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		totalColumns = other.totalColumns;
		totalRows = other.totalRows;
		layoutOptions.clear();
		return *this;
	}

	GridLayout::GridLayout(GridLayout&& other) noexcept
		: GuiLayout(std::move(other)), totalColumns(other.totalColumns), totalRows(other.totalRows), layoutOptions(other.layoutOptions)
	{
		other.layoutOptions.clear();
	}

	GridLayout& GridLayout::operator=(GridLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		totalColumns = other.totalColumns;
		totalRows = other.totalRows;
		layoutOptions = other.layoutOptions;
		other.layoutOptions.clear();
		return *this;
	}

}