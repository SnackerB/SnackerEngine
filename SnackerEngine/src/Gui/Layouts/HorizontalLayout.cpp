#include "Gui/Layouts/HorizontalLayout.h"
#include "Core/Log.h"

namespace SnackerEngine
{

	void HorizontalLayout::addChild(const GuiID& guiID, const LayoutOptions& options)
	{
		GuiLayout::addChild(guiID, {});
		layoutOptions.push_back(options);
		totalWeight += options.weight;
		enforceLayout();
	}

	std::size_t HorizontalLayout::removeChild(GuiElement& guiElement)
	{
		std::size_t index = GuiLayout::removeChild(guiElement);
		layoutOptions.erase(layoutOptions.begin() + index);
		return index;
	}

	void HorizontalLayout::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i parentSize = getSize(parentID);
		unsigned int remainder = parentSize.x;
		for (unsigned int i = 0; i < children.size(); ++i) {
			Vec2i position = Vec2i(parentSize.x - remainder);
			Vec2i size = (i == children.size() - 1) ? Vec2i(remainder, parentSize.y)
				: Vec2i(static_cast<int>(static_cast<float>(parentSize.x) * layoutOptions[i].weight / totalWeight, parentSize.y));
			remainder -= size.x;
			setPosition(children[i], position);
			notifyPositionChange(children[i]);
			setSize(children[i], size);
			notifySizeChange(children[i]);
			enforceLayouts(children[i]);
		}
	}

	HorizontalLayout::HorizontalLayout(const bool& forceHeight)
		: GuiLayout(), totalWeight(0.0f), forceHeight(forceHeight) {}

}