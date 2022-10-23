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

	ListLayout::ListLayout(float verticalOffset, float border, ListLayoutResizeMode resizeMode)
		: GuiLayout(), verticalOffset(verticalOffset), border(border), currentVerticalSize(0.0f), layoutOptions{}, resizeMode(resizeMode) {}

	void ListLayout::enforceLayout() 
	{
		if (!guiManager) return;
		float currentYOffset = verticalOffset;
		double parentWidth = getSize(parentID).x;
		parentWidth -= 2 * border;
		for (unsigned int i = 0; i < children.size(); ++i) {
			int height = getSize(children[i]).y;
			// Set position
			setPosition(children[i], Vec2i(border, currentYOffset));
			notifyPositionChange(children[i]);
			// Resize
			switch (resizeMode)
			{
			case SnackerEngine::ListLayout::ListLayoutResizeMode::DONT_RESIZE:
			{
				break;
			}
			case SnackerEngine::ListLayout::ListLayoutResizeMode::RESIZE_WIDTH_IF_POSSIBLE:
			{
				if (parentWidth >= getPreferredMinSize(children[i]).x &&
					parentWidth <= getPreferredMaxSize(children[i]).x) {
					setWidth(children[i], static_cast<int>(parentWidth));
				}
				break;
			}
			case SnackerEngine::ListLayout::ListLayoutResizeMode::RESIZE_WIDTH:
			{
				setWidth(children[i], static_cast<int>(parentWidth));
			}
			default:
				break;
			}
			// Advance to next element
			currentYOffset += height + verticalOffset;
		}
	}

}