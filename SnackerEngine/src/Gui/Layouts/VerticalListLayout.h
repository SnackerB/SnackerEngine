#pragma once

#include "Gui/GuiLayout.h"
#include "Utility/Alignment.h"

namespace SnackerEngine
{

	class VerticalListLayout : public GuiLayout
	{
		/// size of the border vertically between elements and to the edges, in pixels
		unsigned border;
		/// Horizontal and vertical alignment. Default: LEFT, TOP
		AlignmentHorizontal alignmentHorizontal;
		AlignmentVertical alignmentVertical;
		/// Removes the given child guiElement from this layout
		void removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		VerticalListLayout(const unsigned& border = 0, AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::LEFT, AlignmentVertical alignmentVertical = AlignmentVertical::TOP);
		/// Adds a child to this guiElement (using row = 0, column = 0). Returns true on success
		bool registerChild(GuiElement& guiElement) override;
		// Copy constructor and assignment operator
		VerticalListLayout(const VerticalListLayout& other) noexcept;
		VerticalListLayout& operator=(const VerticalListLayout& other) noexcept;
		/// Move constructor and assignment operator
		VerticalListLayout(VerticalListLayout&& other) noexcept;
		VerticalListLayout& operator=(VerticalListLayout&& other) noexcept;
	};

}