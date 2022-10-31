#pragma once

#include "Gui/GuiElements/GuiPanel2.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declaration of guiStyle
	struct GuiStyle;
	//--------------------------------------------------------------------------------------------------
	class GuiWindow2 : public GuiPanel2
	{
	private:
		/// The size of the resize button in pixels
		double resizeButtonSize;
		/// The color of the resize button
		Color3f resizeButtonColor;
		/// The model matrix of the resize button
		Mat4f resizeButtonModelMatrix;
		/// if this is set to true, the window is currently being moved
		bool isMoving;
		/// If this is set to true, the window is currently being resized
		bool isResizing;
		/// Original mouse position when the moving/resizing started
		Vec2f mouseOffset;
		/// Computes the model matrices
		void computeResizeButtonModelMatrix();
	protected:
		/// Draws this GuiWindow object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function gets called when the position changes. Not called by the constructor!
		virtual void onPositionChange() override;
		/// This function gets called when the size changes. Not called by the constructor!
		virtual void onSizeChange() override;
		/// Returns true if the given position vector (relative to the top left corner of this element)
		/// collides with the resize button, given that a collision with this window element was already
		/// detected
		bool isCollidingWithResizeButton(const Vec2i& offset);
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// This function is called by the guiManager after registering this guiElement.
		/// When this function is called, the guiManager pointer, the guiID, and the parent element id are set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods);
		/// Callback function for mouse button input on this guiElement. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
	public:
		/// Constructor
		GuiWindow2(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Color3f& backgroundColor = Color3f(), const double& resizeButtonSize = 100, const Color3f& resizeButtonColor = Color3f(1.0f));
		/// Constructor using guiStyle
		GuiWindow2(const GuiStyle& style);
		/// Copy constructor
		GuiWindow2(const GuiWindow2& other) noexcept;
		/// Copy assignment operator
		GuiWindow2& operator=(const GuiWindow2& other) noexcept;
		/// Move constructor
		GuiWindow2(GuiWindow2&& other) noexcept;
		/// Move assignment operator
		GuiWindow2& operator=(GuiWindow2&& other) noexcept;
		/// Destructor
		~GuiWindow2();
		/// Getters and Setters
		const double& getResizeButtonSize() const;
		const Color3f& getResizeButtonColor() const;
		void setResizeButtonSize(const double& resizeButtonSize);
		void setResizeButtonColor(const Color3f& resizeButtonColor);
	};
	//--------------------------------------------------------------------------------------------------
}