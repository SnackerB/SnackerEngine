#pragma once
#include "Gui/GuiLayout.h"

#include <optional>

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
		/// The width of the resize area (in pixels)
		unsigned int resizeAreaWidth;
		/// The offset of the mouse to the border (used for resizing the layout)
		int mouseOffset;
		/// The border that is currently being resized. COunting from the left, starting at 0.
		unsigned int resizeBorder;
		/// Vector of stored layoutOptions
		std::vector<LayoutOptions> layoutOptions;
		/// Adds a new element to the layout
		void addChild(const GuiID& guiID, const LayoutOptions& options);
		/// Removes the given child guiElement from this layout
		std::size_t removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Helper function that returns the border and offset, if a collision occured.
		/// position: position of mouse relative to the parent elements (0, 0) coordinate
		std::optional<std::pair<unsigned int, int>> getCollidingBorderAndOffset(const Vec2i& position);
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// This function is called by the guiManager after registering this guiInteractable object.
		/// When this function is called, the guiManager pointer is set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		/// Callback function for mouse button input on this guiInteractable object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
	public:
		/// Constructor
		HorizontalLayout(const bool& forceHeight = true);
	};

}