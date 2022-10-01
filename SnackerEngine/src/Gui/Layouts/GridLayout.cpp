#include "Gui/Layouts/GridLayout.h"
#include "Core/Log.h"

namespace SnackerEngine
{

	GridLayout::GridLayout(unsigned totalColumns, unsigned totalRows)
		: totalColumns(totalColumns), totalRows(totalRows) {}

	void GridLayout::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i parentSize = getSize(elementID);
		Vec2i elementSize = Vec2i(parentSize.x / totalColumns, parentSize.y / totalRows);
		Vec2i lastElementSize = elementSize + Vec2i(parentSize.x % totalColumns, parentSize.y % totalRows);
		for (auto& element : guiElements) {
			// Set size
			Vec2i currentSize = elementSize;
			if (element.second.column == totalColumns) currentSize.x = lastElementSize.x;
			if (element.second.row == totalRows) currentSize.y = lastElementSize.y;
			setSize(element.first, currentSize);
			notifySizeChange(element.first);
			// Set position
			setPosition(element.first, Vec2i(elementSize.x * (element.second.column - 1), elementSize.y * (element.second.row - 1)));
			notifyPositionChange(element.first);
			// Enforce layout
			enforceLayouts(element.first);
		}
	}

	void GridLayout::addElement(const GuiID& guiID, const LayoutOptions& options)
	{
		guiElements.push_back(std::make_pair(guiID, options));
	}

}