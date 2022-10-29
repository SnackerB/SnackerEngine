#pragma once

#include "Gui/GuiLayout2.h"

#include <optional>

namespace SnackerEngine
{

	class VerticalLayout2 : public GuiLayout2
	{
	private:
		friend class GuiManager2;
		friend class GuiElement2;
		/// Total weight
		double totalWeight;
		/// If this is set to true, all children are forced to be the same width as the parent element
		bool forceWidth;
		/// The height of the resize area (in pixels)
		unsigned int resizeAreaHeight;
		/// The offset of the mouse to the border (used for resizing the layout)
		int mouseOffset;
		/// The border that is currently being resized. Counting from the top, starting at 0.
		unsigned int resizeBorder;
		/// Vector of stored weights
		std::vector<double> weights;
		/// Vector of stored width percentages
		std::vector<double> percentages;

		/// Removes the given child guiElement from this layout
		void removeChild(GuiElement2& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Helper function that returns the border and offset, if a collision occured.
		/// position: position of mouse relative to the parent elements (0, 0) coordinate
		std::optional<std::pair<unsigned int, int>> getCollidingBorderAndOffset(const Vec2i& position);
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
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
		VerticalLayout2(const bool& forceWidth = true);
		/// Adds a child to this guiElement. Returns true on success
		bool registerChild(GuiElement2& guiElement, const double& weight);
		/// Adds a child to this guiElement (using weight = 0.0). Returns true on success
		bool registerChild(GuiElement2& guiElement) override;
		/// Copy constructor and assignment operator
		VerticalLayout2(const VerticalLayout2& other) noexcept;
		VerticalLayout2& operator=(const VerticalLayout2& other) noexcept;
		/// Move constructor and assignment operator
		VerticalLayout2(VerticalLayout2&& other) noexcept;
		VerticalLayout2& operator=(VerticalLayout2&& other) noexcept;
	};

}