#pragma once

#include "Gui/GuiLayout.h"

namespace SnackerEngine
{

	class HorizontalListLayout : public GuiLayout
	{
		/// size of the border vertically between elements and to the edges, in pixels
		unsigned border;
		/// If this is set to true, the height of the layout snaps to the largest preferredHeight
		/// of its children
		bool snapHeightToPreferred;
		/// Removes the given child guiElement from this layout
		void removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		HorizontalListLayout(const unsigned& border = 0, const bool& snapHeightToPreferred = false);
		// Copy constructor and assignment operator
		HorizontalListLayout(const HorizontalListLayout& other) noexcept;
		HorizontalListLayout& operator=(const HorizontalListLayout& other) noexcept;
		/// Move constructor and assignment operator
		HorizontalListLayout(HorizontalListLayout&& other) noexcept;
		HorizontalListLayout& operator=(HorizontalListLayout&& other) noexcept;
		/// Getters & Setters
		const bool& isSnapHeightToPreferred() const { return snapHeightToPreferred; };
		void setSnapheightToPreferred(const bool& snapHeightToPreferred);
	};

}