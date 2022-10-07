#pragma once

#include "Gui/GuiElements/GuiPanel.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declaration of guiStyle
	struct GuiStyle;
	//--------------------------------------------------------------------------------------------------
	class GuiWindow : public GuiPanel
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
		// Draws this GuiWindow
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
		GuiWindow(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Color3f& backgroundColor = Color3f(), const double& resizeButtonSize = 100, const Color3f& resizeButtonColor = Color3f(1.0f));
		/// Constructor using guiStyle
		GuiWindow(const GuiStyle& style);
		/// Copy constructor
		GuiWindow(const GuiWindow& other) noexcept;
		/// Copy assignment operator
		GuiWindow& operator=(const GuiWindow& other) noexcept;
		/// Move constructor
		GuiWindow(GuiWindow&& other) noexcept;
		/// Move assignment operator
		GuiWindow& operator=(GuiWindow&& other) noexcept;
		/// Destructor
		~GuiWindow();
		/// Getters and Setters
		const double& getResizeButtonSize() const;
		const Color3f& getResizeButtonColor() const;
		void setResizeButtonSize(const double& resizeButtonSize);
		void setResizeButtonColor(const Color3f& resizeButtonColor);
	};
	//--------------------------------------------------------------------------------------------------
}