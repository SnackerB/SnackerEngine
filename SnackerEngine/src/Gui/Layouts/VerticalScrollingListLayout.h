#pragma once

#include "Gui/Layouts/VerticalListLayout.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"
#include "Graphics/Color.h"


namespace SnackerEngine
{

	/// Forward declaration of GuiStyle
	struct GuiStyle;

	class VerticalScrollingListLayout : public VerticalListLayout
	{
	protected:
		/// current total vertical size of the list, including offset at the beginning and end
		float currentVerticalSize;
		/// current position in the list (offset of from the upper left corner of the list
		/// to the current upper corner of the guiElement. Used for scrolling through the list)
		float currentVerticalOffset;
		/// Speed when scrolling
		float scrollSpeed;
		/// Boolean that controls wether the scrollbar is drawn
		bool drawScrollBar;
		/// Percentage the visible scection holds of the total list
		float visiblePercentage;
		/// Percentage the offset holds of the total list
		float offsetPercentage;
		/// Model matrices for the scroll bar and its background
		Mat4f modelMatrixScrollBarBackground;
		Mat4f modelMatrixScrollBar;
		/// Shader for drawing the scroll bar and its background
		Shader shaderScrollBar;
		/// Colors of the scroll bar and its background
		Color3f scrollBarBackgroundColor;
		Color3f scrollBarColor;
		/// Width of the scroll bar in pixels
		float scrollBarWidth;
		/// Offset from the top, bottom and right border to the scroll bar
		float scrollBarOffsetTop;
		float scrollBarOffsetBottom;
		float scrollBarOffsetRight;
		/// vertical offset from the top of the scroll bar to the mouse (used for scrolling through
		/// the list with the mouse)
		float mouseOffsetFromScrollBar;
		/// The first and last visible element in the children vector
		unsigned firstVisibleElement;
		unsigned lastVisibleElement;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Computes the percentages and wether the scroll bar should be visible
		void computeScrollBar();
	protected:
		/// This function is called by the guiManager after registering this guiInteractable object.
		/// When this function is called, the guiManager pointer is set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Callback function for scrolling the mouse wheel. Parameter the same as in Scene.h
		virtual void callbackMouseScrollOnElement(const Vec2d& offset) override;
		/// Draws this guiInteractable object relative to its parent element
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Callback function for mouse button input on this guiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Returns the first colliding child which collides with the given position vector. The position
		/// vector is relative to the top left corner of the parent. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		GuiID getCollidingChild(const Vec2i& position) override;
		/// Returns the mouse offset of a child element from this element. Can be
		/// overwritten if the children are displayed at a different place than they
		/// are (eg. in a scrolling list etc)
		Vec2i getChildOffset(const GuiID& childID) override;
	public:
		/// Constructor
		VerticalScrollingListLayout(const unsigned& border = 0, 
			AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::LEFT, 
			AlignmentVertical alignmentVertical = AlignmentVertical::TOP,
			const float& scrollSpeed = 20,
			const Color3f& scrollBarBackgroundColor = Color3f(0.0f), const Color3f& scrollBarColor = Color3f(1.0f),
			const float& scrollBarWidth = 10, const float& scrollBarOffsetTop = 10, const float& scrollBarOffsetBottom = 10,
			const float& scrollBarOffsetRight = 10);
		/// Constructor using GuiStyle
		VerticalScrollingListLayout(const GuiStyle& style);
		// Copy constructor and assignment operator
		VerticalScrollingListLayout(const VerticalScrollingListLayout& other) noexcept;
		VerticalScrollingListLayout& operator=(const VerticalScrollingListLayout& other) noexcept;
		/// Move constructor and assignment operator
		VerticalScrollingListLayout(VerticalScrollingListLayout&& other) noexcept;
		VerticalScrollingListLayout& operator=(VerticalScrollingListLayout&& other) noexcept;
	};

}