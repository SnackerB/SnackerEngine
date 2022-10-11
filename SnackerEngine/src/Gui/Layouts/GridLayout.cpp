#include "Gui/Layouts/GridLayout.h"
#include "Core/Log.h"

namespace SnackerEngine
{

	void GridLayout::addChild(const GuiID& guiID, const LayoutOptions& options)
	{
		GuiLayout::addChild(guiID, {});
		layoutOptions.push_back(options);
		enforceLayout();
	}

	std::size_t GridLayout::removeChild(GuiElement& guiElement)
	{
		std::size_t index = GuiLayout::removeChild(guiElement);
		layoutOptions.erase(layoutOptions.begin() + index);
		return index;
	}

	void GridLayout::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i parentSize = getSize(parentID);
		Vec2i elementSize = Vec2i(parentSize.x / totalColumns, parentSize.y / totalRows);
		Vec2i lastElementSize = elementSize + Vec2i(parentSize.x % totalColumns, parentSize.y % totalRows);
		for (unsigned i = 0; i < children.size(); ++i) {
			// Set size
			Vec2i currentSize = elementSize;
			if (layoutOptions[i].column == totalColumns) currentSize.x = lastElementSize.x;
			if (layoutOptions[i].row == totalRows) currentSize.y = lastElementSize.y;
			setSize(children[i], currentSize);
			notifySizeChange(children[i]);
			// Set position
			setPosition(children[i], Vec2i(elementSize.x * (layoutOptions[i].column - 1), elementSize.y * (layoutOptions[i].row - 1)));
			notifyPositionChange(children[i]);
			// Enforce layout
			enforceLayouts(children[i]);
		}
	}

	GridLayout::GridLayout(unsigned totalColumns, unsigned totalRows)
		: totalColumns(totalColumns), totalRows(totalRows), layoutOptions{} {}

}