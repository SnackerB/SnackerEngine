#pragma once

#include "Gui/GuiLayout.h"

#include <vector>

namespace SnackerEngine
{

	class GridLayout : public GuiLayout
	{
	private:
		friend class GuiManager;
		friend class GuiElement;
		/// Total number of columns in the grid
		unsigned totalColumns;
		/// Total number of rows in the grid
		unsigned totalRows;
		/// struct that stores information where an element is positioned in this layout
		struct LayoutOptions {
			unsigned row;
			unsigned column;
		};
		// Vector of stored layoutOptions
		std::vector<LayoutOptions> layoutOptions;
		/// Removes the given child guiElement from this layout
		void removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		GridLayout(unsigned totalColumns, unsigned totalRows);
		/// Adds a child to this guiElement. Returns true on success
		bool registerChild(GuiElement& guiElement, const unsigned& row, const unsigned& column);
		/// Adds a child to this guiElement (using row = 0, column = 0). Returns true on success
		bool registerChild(GuiElement& guiElement) override;
		// Copy constructor and assignment operator
		GridLayout(const GridLayout& other) noexcept;
		GridLayout& operator=(const GridLayout& other) noexcept;
		/// Move constructor and assignment operator
		GridLayout(GridLayout&& other) noexcept;
		GridLayout& operator=(GridLayout&& other) noexcept;
	};

}