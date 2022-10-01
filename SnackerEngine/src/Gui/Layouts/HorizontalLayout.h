#pragma once
#include "Gui/Layout.h"

namespace SnackerEngine
{

	class HorizontalLayoutReference : public GuiLayoutReference {};

	struct HorizontalLayoutOptions
	{
		float weight;
	};

	class HorizontalLayout : public Layout
	{
	public:
		using LayoutReference = HorizontalLayoutReference;
		using LayoutOptions = HorizontalLayoutOptions;
		/// Constructor
		HorizontalLayout();
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Adds a new element to the layout
		void addElement(const GuiID& guiID, const LayoutOptions& options);
	};

}