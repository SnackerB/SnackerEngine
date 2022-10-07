#include "Gui/Layouts/ListLayout.h"

namespace SnackerEngine
{

	ListLayout::ListLayout(float verticalOffset, float leftBorder)
		: verticalOffset(verticalOffset), leftBorder(leftBorder), currentVerticalSize(0.0f), guiElements{} {}

	void ListLayout::enforceLayout() 
	{
		if (!guiManager) return;
		float currentYOffset = verticalOffset;
		for (const auto& element : guiElements) {
			int height = getSize(element.first).y;
			// Set position
			setPosition(element.first, Vec2i(leftBorder, currentYOffset));
			notifyPositionChange(element.first);
			// Advance to next element
			currentYOffset += height + verticalOffset;
		}
	}

	void ListLayout::addElement(const GuiID& guiID, const LayoutOptions& options)
	{
		guiElements.push_back(std::make_pair(guiID, options));
	}

}