#pragma once

#include "Gui/Layout.h"

#include <vector>
#include <algorithm>

namespace SnackerEngine
{

	class ListLayoutReference : public GuiLayoutReference {};

	struct ListLayoutOptions
	{
	};

	class ListLayout : public Layout
	{
	public:
		using LayoutReference = ListLayoutReference;
		using LayoutOptions = ListLayoutOptions;
	private:
		/// vertical offset between the elements in pixels
		float verticalOffset;
		/// Horizontal offset from the left border and the elements
		float leftBorder;
		/// current total vertical size of the list, including offset at the beginning and end
		float currentVerticalSize;
		/// Vector of stored elements
		std::vector<std::pair<GuiID, LayoutOptions>> guiElements;
	public:
		/// Constructor
		ListLayout(float verticalOffset, float leftBorder);
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Adds a new element to the layout
		void addElement(const GuiID& guiID, const LayoutOptions& options);
	};

}