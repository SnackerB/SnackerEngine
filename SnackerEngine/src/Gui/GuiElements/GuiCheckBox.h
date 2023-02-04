#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiEventHandles/GuiVariableHandle.h"
#include "Gui/GuiElements/GuiButton.h"

namespace SnackerEngine
{

	class GuiCheckBox : public GuiElement
	{
		/// Variable Handle for bool
		GuiVariableHandle<bool>* boolHandle;
		/// Checkbox button
		std::unique_ptr<GuiButton> button;
		/// Checkbox label
		std::unique_ptr<GuiDynamicTextBox> label;
		/// Button colors for when the bool is set to true
		Color4f colorDefaultTrue;
		Color4f colorHoverTrue;
		Color4f colorPressedTrue;
		Color4f ColorHoverPressedTrue;
		/// Button colors for when the bool is set to false
		Color4f colorDefaultFalse;
		Color4f colorHoverFalse;
		Color4f colorPressedFalse;
		Color4f ColorHoverPressedFalse;
		/// Event handle that is used to change the bool when the checkbox button is pressed
		struct GuiCheckBoxToggleEventHandle : public GuiEventHandle
		{
			friend class GuiCheckBox;
			GuiCheckBox* guiCheckBox;
		protected:
			void onEvent() override
			{
				if (guiCheckBox) guiCheckBox->toggle();
			}
			/// Constructor
			GuiCheckBoxToggleEventHandle(GuiCheckBox& guiCheckBox) { this->guiCheckBox = &guiCheckBox; }
		};
		/// The GuiCheckBoxToggleEventHandle of this GuiCheckBox
		GuiCheckBoxToggleEventHandle toggleEventHandle;
		/// Helper function that updates the button colors. Should be called when the boolHandle changes its value.
		void determineState();
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Returns how the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Returns the first colliding child which collides with the given position vector. The position
		/// vector is relative to the top left corner of the parent. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& position) override;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle) override;
		/// Sets the bool
		void setValue(const bool& value);
		/// Returns the bool
		const bool& getValue() const;
		/// Toggles the value of the bool
		void toggle();
	public:
		/// Constructor
		GuiCheckBox(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const GuiElement::ResizeMode& resizeMode = GuiElement::ResizeMode::DO_NOT_RESIZE,
			const std::string& label = "", const Font& font = Font(), const double& fontSize = 0,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Color4f& labelTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const int& border = 0, 
			const Color4f& CheckBoxButtonDefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& CheckBoxButtonHoverColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& CheckBoxButtonPressedColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& CheckBoxButtonPressedHoverColor = { 1.0f, 1.0f, 1.0f, 1.0f });
		/// Constructor that already registers variable handle
		GuiCheckBox(GuiVariableHandle<bool>& boolHandle, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const GuiElement::ResizeMode& resizeMode = GuiElement::ResizeMode::DO_NOT_RESIZE,
			const std::string& label = "", const Font& font = Font(), const double& fontSize = 0,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Color4f& labelTextBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const int& border = 0,
			const Color4f& CheckBoxButtonDefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& CheckBoxButtonHoverColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& CheckBoxButtonPressedColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Color4f& CheckBoxButtonPressedHoverColor = { 1.0f, 1.0f, 1.0f, 1.0f });
		/// Constructors using GuiStyle
		GuiCheckBox(const std::string& label, GuiVariableHandle<bool>& handle, const GuiStyle& style);
		GuiCheckBox(const std::string& label, const GuiStyle& style);
		GuiCheckBox(const std::string& label, const double& fontSize, GuiVariableHandle<bool>& handle, const GuiStyle& style);
		GuiCheckBox(const std::string& label, const double& fontSize, const GuiStyle& style);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setBoolHandle(GuiVariableHandle<bool>& variableHandle);
		/// Copy constructor and assignment operator
		GuiCheckBox(const GuiCheckBox& other) noexcept;
		GuiCheckBox& operator=(const GuiCheckBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiCheckBox(GuiCheckBox&& other) noexcept;
		GuiCheckBox& operator=(GuiCheckBox&& other) noexcept;
		/// Destructor
		~GuiCheckBox();
	};

}