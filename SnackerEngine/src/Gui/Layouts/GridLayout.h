#pragma once

#include "Gui/Layout.h"

#include <vector>
#include <algorithm>

namespace SnackerEngine
{

	class GridLayoutReference : public GuiLayoutReference {};

	struct GridLayoutOptions
	{
		unsigned column;
		unsigned row;
	};

	class GridLayout : public Layout
	{
	public:
		using LayoutReference = GridLayoutReference;
		using LayoutOptions = GridLayoutOptions;
	private:
		unsigned totalColumns;
		unsigned totalRows;
		std::vector<std::pair<GuiID, LayoutOptions>> guiElements;
	public:
		/// Constructor
		GridLayout(unsigned totalColumns, unsigned totalRows);
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Adds a new element to the layout
		void addElement(const GuiID& guiID, const LayoutOptions& options);
	};

}