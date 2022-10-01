#pragma once
#include "Gui/Layout.h"

namespace SnackerEngine
{

	class VerticalLayoutReference : public GuiLayoutReference {};

	struct VerticalLayoutOptions
	{
		float weight;
	};

	class VerticalLayout : public Layout
	{
	public:
		using LayoutReference = VerticalLayoutReference;
		using LayoutOptions = VerticalLayoutOptions;
		/// Constructor
		VerticalLayout();
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Adds a new element to the layout
		void addElement(const GuiID& guiID, const LayoutOptions& options);
	};

}