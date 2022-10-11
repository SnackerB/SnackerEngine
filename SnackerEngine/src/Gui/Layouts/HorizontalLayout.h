#pragma once
#include "Gui/GuiLayout.h"

namespace SnackerEngine
{

	class HorizontalLayoutReference : public GuiLayoutReference {};

	struct HorizontalLayoutOptions
	{
		float weight;
	};

	class HorizontalLayout : public GuiLayout
	{
	public:
		using LayoutReference = HorizontalLayoutReference;
		using LayoutOptions = HorizontalLayoutOptions;
	private:
		friend class GuiManager;
		friend class GuiElement;
		/// Total weight
		float totalWeight;
		/// If this is set to true, all children are forced to be the same height as the parent element
		bool forceHeight;
		/// Vector of stored layoutOptions
		std::vector<LayoutOptions> layoutOptions;
		/// Adds a new element to the layout
		void addChild(const GuiID& guiID, const LayoutOptions& options);
		/// Removes the given child guiElement from this layout
		std::size_t removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		HorizontalLayout(const bool& forceHeight = true);
	};

}