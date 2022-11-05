#pragma once

#include "Gui/GuiLayout2.h"

#include <vector>

namespace SnackerEngine
{

	class GridLayout2 : public GuiLayout2
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
		void removeChild(GuiElement2& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		GridLayout2(unsigned totalColumns, unsigned totalRows);
		/// Adds a child to this guiElement. Returns true on success
		bool registerChild(GuiElement2& guiElement, const unsigned& row, const unsigned& column);
		/// Adds a child to this guiElement (using row = 0, column = 0). Returns true on success
		bool registerChild(GuiElement2& guiElement) override;
		// Copy constructor and assignment operator
		GridLayout2(const GridLayout2& other) noexcept;
		GridLayout2& operator=(const GridLayout2& other) noexcept;
		/// Move constructor and assignment operator
		GridLayout2(GridLayout2&& other) noexcept;
		GridLayout2& operator=(GridLayout2&& other) noexcept;
	};

}