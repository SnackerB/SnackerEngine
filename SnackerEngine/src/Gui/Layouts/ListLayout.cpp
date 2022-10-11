#include "Gui/Layouts/ListLayout.h"

namespace SnackerEngine
{

	void ListLayout::addChild(const GuiID& guiID, const LayoutOptions& options)
	{
		GuiLayout::addChild(guiID, {});
		layoutOptions.push_back(options);
		enforceLayout();
	}

	std::size_t ListLayout::removeChild(GuiElement& guiElement)
	{
		std::size_t index = GuiLayout::removeChild(guiElement);
		layoutOptions.erase(layoutOptions.begin() + index);
		return index;
	}

	ListLayout::ListLayout(float verticalOffset, float leftBorder)
		: GuiLayout(), verticalOffset(verticalOffset), leftBorder(leftBorder), currentVerticalSize(0.0f), layoutOptions{} {}

	void ListLayout::enforceLayout() 
	{
		if (!guiManager) return;
		float currentYOffset = verticalOffset;
		for (unsigned int i = 0; i < children.size(); ++i) {
			int height = getSize(children[i]).y;
			// Set position
			setPosition(children[i], Vec2i(leftBorder, currentYOffset));
			notifyPositionChange(children[i]);
			// Advance to next element
			currentYOffset += height + verticalOffset;
		}
	}

}