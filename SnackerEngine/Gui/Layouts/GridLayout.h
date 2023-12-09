#pragma once

#include "Gui/GuiLayout.h"
#include "Utility/Alignment.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiGridLayout : public GuiLayout
	{
	public:
		/// Enum for specifying the layout mode. Regardless the mode, each child element is positioned in the center
		/// of its respective grid cell, and resized to the size of the grid cell, if possible
		enum class Mode
		{
			SPLIT_CELLS_EQUALLY,		/// Splits the available total space equally
										/// between the grid cells and tries to resize children
										/// to the correct size. Tries to make the grid as large 
										/// the size of the layout.

			SPLIT_CELLS_EQUALLY_SHRINK, /// Same as SPLIT_CELLS_EQUALLY, but tries to shrink the grid
										/// to the childrens preferredSize/minSize and positions it according
										/// to the alignmentHorizontal and alignmentVertical
										/// member variables
										
			ADAPT_CELLS_SHRINK,			/// Adapts width and heights of the different rows and
										/// columns to best fit the child elements. Tries to shrink the grid
										/// to the childrens preferredSize/minSize and positions it according
										/// to the alignmentHorizontal and alignmentVertical
										/// member variables
		};
	private:
		/// Total number of columns in the grid
		unsigned totalColumns = 1;
		/// Total number of rows in the grid
		unsigned totalRows = 1;
		/// Size of the border between grid cells, in pixels
		unsigned border = 0;
		/// Size of the border between grid cells and the outer edge, in pixels
		unsigned outerBorder = 0;
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
		/// Helper function that computes the largest minSize and preferredSize of all children
		std::pair<Vec2i, Vec2i> computeLargestMinSizeAndPreferredSize() const;
		/// Helper functions that compute the total amount of borders in x and y directions
		int computeTotalBorderX() const;
		int computeTotalBorderY() const;
		/// Computes the anchor point of the grid (upper left corner)
		Vec2i computeAnchorPoint(const Vec2i& firstGridCellSize, const Vec2i& gridCellSize, int totalBorderX, int totalBorderY) const;
		Vec2i computeAnchorPoint(int totalGridWidth, int totalGridHeight) const;
		/// Helper function that positions all child elements according to the given anchor point and cell sizes
		void positionChildren(const Vec2i& anchorPoint, const Vec2i& firstCellSize, const Vec2i& cellSize);
		void positionChildren(const Vec2i& anchorPoint, const std::vector<int>& gridCellWidths, const std::vector<int>& gridCellHeights);
		/// Helper function for enforcing the layout in the different modes
		void enforceLayoutSplitCellsEqually();
		void enforceLayoutSplitCellsEquallyShrink();
		void enforceLayoutAdaptCellsShrink();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_GRID_LAYOUT";
		/// Default constructor
		GuiGridLayout(unsigned totalColumns = 1, unsigned totalRows = 1, Mode mode = Mode::SPLIT_CELLS_EQUALLY, unsigned border = 0, unsigned outerBorder = 0); // NOTE: All parameters must have default values!
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
		/// Getters
		unsigned getTotalColumns() const { return totalColumns; }
		unsigned getTotalRows() const { return totalRows; }
		unsigned getBorder() const { return border; }
		unsigned getOuterBorder() const { return outerBorder; }
		/// Setters
		void setTotalColumns(unsigned totalColumns);
		void setTotalRows(unsigned totalRows);
		void setBorder(unsigned border);
		void setOuterBorder(unsigned outerBorder);
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateTotalColumns(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateTotalRows(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateOuterBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
	protected:
		/// Removes the given child from this GuiElement object
		virtual std::optional<unsigned> removeChild(GuiID guiElement) override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;
	};
	//--------------------------------------------------------------------------------------------------
}