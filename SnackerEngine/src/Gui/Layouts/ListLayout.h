#pragma once

#include "Gui/GuiLayout.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"
#include "Graphics/Color.h"

namespace SnackerEngine
{

	/// Forward declaration of class GuiStyle
	struct GuiStyle;

	class ListLayout : public GuiLayout
	{
	public:
		/// enum class that holds different modes that the layout can have regarding
		/// how and when it resizes children
		enum class ListLayoutResizeMode
		{
			DONT_RESIZE,					/// Does not resize children
			RESIZE_WIDTH_IF_POSSIBLE,		/// Resizes width of children to fit in 
											/// the window If there preferred min 
											/// and max size allows
			RESIZE_WIDTH,					/// Resizes width of children to fit in 
											/// the window regardless of there 
											/// preferred sizes
		};
	private:
		/// vertical offset between consecutive elements in pixels
		float verticalOffset;
		/// Horizontal offset from the left border and the elements in pixels
		float leftBorder;
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
		/// Resize mode
		ListLayoutResizeMode listLayoutResizeMode;
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
		ListLayout(const float& verticalOffset, const float& leftBorder, const float& scrollSpeed,
			const Color3f& scrollBarBackgroundColor, const Color3f& scrollBarColor,
			const float& scrollBarWidth, const float& scrollBarOffsetTop, const float& scrollBarOffsetBottom, 
			const float& scrollBarOffsetRight, const ListLayoutResizeMode& resizeMode);
		/// Constructor using GuiStyle
		ListLayout(const GuiStyle& style);
	};

}