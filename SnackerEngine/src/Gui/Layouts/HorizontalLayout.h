#pragma once

#include "Gui/GuiLayout.h"

#include <optional>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class HorizontalLayout : public GuiLayout
	{
	private:
		friend class GuiManager;
		friend class GuiElement;
		/// Total weight
		double totalWeight;
		/// If this is set to true, all children are forced to be the same height as the parent element
		bool forceHeight;
		/// The width of the resize area (in pixels)
		unsigned int resizeAreaWidth;
		/// The offset of the mouse to the border (used for resizing the layout)
		int mouseOffset;
		/// The border that is currently being resized. Counting from the left, starting at 0.
		unsigned int resizeBorder;
		/// Vector of stored weights
		std::vector<double> weights;
		/// Vector of stored width percentages
		std::vector<double> percentages;
		/// If this is set to true, moving the border with the mouse is possible
		bool allowMoveBorders;

		/// Removes the given child guiElement from this layout
		void removeChild(GuiID guiElement) override;
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
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
		/// Computes percentages from weights
		void computePercentagesFromWeights();
		/// Computes weights from percentages
		void computeWeightsFromPercentages();
	public:
		/// Constructor
		HorizontalLayout(const bool& forceHeight = true, const bool& allowMoveBorders = true);
		/// Adds a child to this guiElement. Returns true on success
		bool registerChild(GuiElement& guiElement, const double& weight);
		/// Adds a child to this guiElement (using weight = 0.0). Returns true on success
		bool registerChild(GuiElement& guiElement) override;
		/// Copy constructor and assignment operator
		HorizontalLayout(const HorizontalLayout& other) noexcept;
		HorizontalLayout& operator=(const HorizontalLayout& other) noexcept;
		/// Move constructor and assignment operator
		HorizontalLayout(HorizontalLayout&& other) noexcept;
		HorizontalLayout& operator=(HorizontalLayout&& other) noexcept;
		/// Getters
		const bool& isAllowMoveBorders() const;
		const std::vector<double>& getWeights() { computeWeightsFromPercentages(); return weights; }
		const std::vector<double>& getPercentages() const { return percentages; }
		/// Setters
		void setAllowMoveBorders(const bool& allowMoveBorders);
	};
	//--------------------------------------------------------------------------------------------------
}