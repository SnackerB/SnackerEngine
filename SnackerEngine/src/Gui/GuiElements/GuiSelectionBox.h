#pragma once

#include "Gui/GuiElements/GuiButton.h"

namespace SnackerEngine
{

	class GuiSelectionBox : public GuiElement
	{
	private:
		/// Main button displaying the active selection
		GuiButton activeSelectionButton;
		GuiEventHandle activeSelectionButtonHandle;
		/// Button with arrow
		GuiButton downArrowButton;
		/// Vector of buttons with the different options for selection
		std::vector<GuiButton> selectionOptionButtons;
		std::vector<GuiEventHandle> selectionOptionHandles;
		/// Wether the selection box is currently open
		bool isOpen;
		/// int handle that stores the index of the current selection
		GuiVariableHandle<int>* selectionHandle;
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// Draws this GuiElement object relative to its parent element. This draw call is executed after
		/// all regular draw calls. This will NOT recursively draw all children of this element.
		/// If this is the desired behaviour, call the normal draw() function on all children
		/// in this function.
		/// worldPosition:		position of the upper left corner of the element in world space
		virtual void callbackDrawOnTop(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) override;
		/// Returns the first colliding child which collides with the given offset vector. The offset
		/// vector is relative to the top left corner of the guiElement. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& offset) override;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle);
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		void callbackMouseButton(const int& button, const int& action, const int& mods) override;
	public:
		/// Constructor
		GuiSelectionBox(const Vec2i& position, const Vec2i& size, const std::string& defaultLabel, const std::vector<std::string>& options, 
			const Font& font, const double& fontSize, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f }, 
			const Color4f& defaultColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), 
			const Color4f& hoverColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const Color4f& pressedColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f),
			const Color4f& pressedHoverColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), int border = 0);
		/// Constructor that already registers variable handle
		GuiSelectionBox(GuiVariableHandle<int>& handle, const Vec2i& position, const Vec2i& size, const std::string& defaultLabel, const std::vector<std::string>& options,
			const Font& font, const double& fontSize, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			const Color4f& defaultColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f),
			const Color4f& hoverColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const Color4f& pressedColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f),
			const Color4f& pressedHoverColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), int border = 0);
		/// Sets the variable handle. Cannot be done if a variable handle is already set, 
		/// delete the previous variable handle first!
		void setVariableHandle(GuiVariableHandle<int>& variableHandle);
		/// Constructors using GuiStyle
		GuiSelectionBox(const std::string& defaultLabel, const std::vector<std::string>& options, const GuiStyle& style);
		GuiSelectionBox(GuiVariableHandle<int>& handle, const std::string& defaultLabel, const std::vector<std::string>& options, const GuiStyle& style);
		/// Copy constructor and assignment operator
		GuiSelectionBox(const GuiSelectionBox& other) noexcept;
		GuiSelectionBox& operator=(const GuiSelectionBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiSelectionBox(GuiSelectionBox&& other) noexcept;
		GuiSelectionBox& operator=(GuiSelectionBox&& other) noexcept;
		/// Destructor
		~GuiSelectionBox();
	};

}