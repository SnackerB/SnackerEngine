#pragma once

#include "Gui/GuiLayout.h"
#include "Utility/Alignment.h"

namespace SnackerEngine
{

	class GuiGridLayout : public GuiLayout
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
	private:
		/// Total number of columns in the grid
		unsigned totalColumns = 1;
		/// Total number of rows in the grid
		unsigned totalRows = 1;
		/// size of the border between grid cells, in pixels
		unsigned border = 0;
		/// mode of the layout
		Mode mode = Mode::SPLIT_CELLS_EQUALLY;
		/// Alignment enums
		AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::CENTER;
		AlignmentVertical alignmentVertical = AlignmentVertical::CENTER;
		/// struct that stores information where an element is positioned in this layout
		struct LayoutOptions {
			unsigned row;
			unsigned column;
		};
		// Vector of stored layoutOptions
		std::vector<LayoutOptions> layoutOptions{};
		/// Helper function, that computes the layout as if the mode were set to SPLIT_CELLS_EQUALLY,
		/// but with the given element size and using the centering method specified in the member variables.
		void enforceLayoutSplitCellsEqually(const Vec2i& elementSize);
		/// Helper function that computes the layout when the mode is set to SPLIT_CELLS_EQUALLY
		void enforceLayoutSplitCellsEqually();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_GRID_LAYOUT";
		/// Default constructor
		GuiGridLayout(const unsigned& totalColumns = 1, const unsigned& totalRows = 1, const Mode& mode = Mode::SPLIT_CELLS_EQUALLY, const unsigned& border = 0); // NOTE: All parameters must have default values!
		/// Constructor from JSON
		GuiGridLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiGridLayout() {};
		/// Copy constructor and assignment operator
		GuiGridLayout(const GuiGridLayout& other) noexcept;
		GuiGridLayout& operator=(const GuiGridLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiGridLayout(GuiGridLayout&& other) noexcept;
		GuiGridLayout& operator=(GuiGridLayout&& other) noexcept;

		/// Adds a child to this guiElement. Returns true on success
		virtual bool registerChild(GuiElement& guiElement) override;
		/// Adds a child to this guiElement. Returns true on success
		bool registerChild(GuiElement& guiElement, unsigned row, unsigned column);
		/// Adds a child to this guiElement, with options given in JSON
		virtual bool registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames) override;

	protected:

		/// Removes the given child from this GuiElement object
		virtual std::optional<unsigned> removeChild(GuiID guiElement) override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;
	};

}