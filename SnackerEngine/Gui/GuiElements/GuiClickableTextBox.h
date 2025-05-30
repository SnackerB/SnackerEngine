#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Utility\Handles\EventHandle.h"

namespace SnackerEngine
{
	class GuiClickableTextBox : public GuiTextBox
	{
	private:
		/// The event that happens when the text box is pressed
		EventHandle::Observable observableTextBoxClicked;
	protected:
		/// Helper function that is called when the text box is clicked.
		/// Can be overwritten by child elements
		virtual void onTextBoxClicked();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_CLICKABLE_TEXT_BOX";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiClickableTextBox(const Vec2i& position = Vec2i{}, const Vec2i& size = Vec2i{}, const std::string& text = "");
		GuiClickableTextBox(const std::string& label);
		/// Constructor from JSON
		GuiClickableTextBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiClickableTextBox();
		/// Copy constructor and assignment operator
		GuiClickableTextBox(const GuiClickableTextBox& other) noexcept;
		GuiClickableTextBox& operator=(const GuiClickableTextBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiClickableTextBox(GuiClickableTextBox&& other) noexcept;

	protected:

		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;

		//==============================================================================================
		// EventHandles
		//==============================================================================================

	public:
		/// Subscribes an event handle
		void subscribeToEventTextBoxClicked(EventHandle& eventHandle);

		//==============================================================================================
		// Collisions
		//==============================================================================================

	public:
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;

		//==============================================================================================
		// Events
		//==============================================================================================

	protected:
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
	};
}