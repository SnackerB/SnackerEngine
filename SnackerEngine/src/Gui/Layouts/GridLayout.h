#pragma once

#include "Gui/GuiLayout.h"

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

	class GridLayout : public GuiLayout
	{
	public:
		using LayoutReference = GridLayoutReference;
		using LayoutOptions = GridLayoutOptions;
	private:
		friend class GuiManager;
		friend class GuiElement;
		/// Total number of columns in the grid
		unsigned totalColumns;
		/// Total number of rows in the grid
		unsigned totalRows;
		// Vector of stored layoutOptions
		std::vector<LayoutOptions> layoutOptions;
		/// Adds a new element to the layout
		void addChild(const GuiID& guiID, const LayoutOptions& options);
		/// Removes the given child guiElement from this layout
		std::size_t removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		GridLayout(unsigned totalColumns, unsigned totalRows);
	};

}