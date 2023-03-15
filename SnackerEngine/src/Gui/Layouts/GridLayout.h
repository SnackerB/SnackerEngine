#pragma once

#include "Gui/GuiLayout.h"

#include <vector>

namespace SnackerEngine
{

	class GridLayout : public GuiLayout
	{
	public:
		/// Enum for specifying the layout mode
		enum class Mode 
		{
			SPLIT_CELLS_EQUALLY,		/// Splits the available total space equally
										/// between the grid cells and forces children
										/// to be the correct size. Default mode.
			
			ADAPT_CELLS_EQUALLY,		/// Tries to find a size that fits all children.
										/// All cells will have the same size. The total
										/// grid will fit inside the parent bounding box.
										/// Use the Alignment enum to specify where the
										/// grid is placed inside the parent bounding box.
										
			ADAPT_CELLS_EQUALLY_WEAK,	/// Same as ADAPT_CELLS_EQUALLY, but the grid is not
										/// forced to fit inside the parent bounding box
		};
		/// Enums for specifying the alignment. Default is CENTER.
		enum class AlignmentHorizontal
		{
			LEFT,
			CENTER,
			RIGHT,
		};
		enum class AlignmentVertical
		{
			TOP,
			CENTER,
			BOTTOM,
		};
	private:
		friend class GuiManager;
		friend class GuiElement;
		/// Total number of columns in the grid
		unsigned totalColumns;
		/// Total number of rows in the grid
		unsigned totalRows;
		/// size of the border between grid cells, in pixels
		unsigned border;
		/// mode of the layout
		Mode mode;
		/// Alignment enums
		AlignmentHorizontal alignmentHorizontal;
		AlignmentVertical alignmentVertical;
		/// struct that stores information where an element is positioned in this layout
		struct LayoutOptions {
			unsigned row;
			unsigned column;
		};
		// Vector of stored layoutOptions
		std::vector<LayoutOptions> layoutOptions;
		/// Removes the given child guiElement from this layout
		void removeChild(GuiID guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Helper function, that computes the layout as if the mode were set to SPLIT_CELLS_EQUALLY,
		/// but with the given element size and using the centering method specified in the member variables.
		void enforceLayoutSplitCellsEqually(const Vec2i& elementSize);
		/// Helper function that computes the layout when the mode is set to SPLIT_CELLS_EQUALLY
		void enforceLayoutSplitCellsEqually();
	public:
		/// Constructor
		GridLayout(const unsigned& totalColumns, const unsigned& totalRows, const Mode& mode=Mode::SPLIT_CELLS_EQUALLY, const unsigned& border=0);
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
		/// Getters
		const unsigned& getTotalColumns() const;
		const unsigned& getTotalRows() const;
		const unsigned& getBorder() const;
		const Mode& getMode() const;
		const AlignmentHorizontal& getAlignmentHorizontal() const;
		const AlignmentVertical& getAlignmentVertical() const;
		void setTotalColumns(const unsigned& totalColumns);
		void setTotalRows(const unsigned& totalRows);
		void setBorder(const unsigned& border);
		void setMode(const Mode& mode);
		void setAlignmentHorizontal(const AlignmentHorizontal& alignmentHorizontal);
		void setAlignmentVertical(const AlignmentVertical& alignmentVertical);
	};

}