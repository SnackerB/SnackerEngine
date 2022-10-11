#pragma once

#include "Gui/GuiLayout.h"

#include <vector>
#include <algorithm>

namespace SnackerEngine
{

	class ListLayoutReference : public GuiLayoutReference {};

	struct ListLayoutOptions {};

	class ListLayout : public GuiLayout
	{
	public:
		using LayoutReference = ListLayoutReference;
		using LayoutOptions = ListLayoutOptions;
	private:
		friend class GuiManager;
		friend class GuiElement;
		/// vertical offset between the elements in pixels
		float verticalOffset;
		/// Horizontal offset from the left border and the elements
		float leftBorder;
		/// current total vertical size of the list, including offset at the beginning and end
		float currentVerticalSize;
		/// Vector of stored layoutOptions
		std::vector<LayoutOptions> layoutOptions;
		/// Adds a new element to the layout
		void addChild(const GuiID& guiID, const LayoutOptions& options);
		/// Removes the given child guiElement from this layout
		std::size_t removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		ListLayout(float verticalOffset, float leftBorder);
	};

}