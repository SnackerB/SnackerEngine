#pragma once

#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/GuiEventHandles/GuiEventHandle.h"
#include "Gui/GuiElements/GuiTextBox.h"

namespace SnackerEngine
{

	/// Forward declaration of GuiStyle
	struct GuiStyle;

	class GuiButton : public GuiDynamicTextBox
	{
	private:
		/// The event that happens when the button is pressed
		GuiEventHandle* eventHandle;
		/// Different colors of the button for the different stages
		Color4f backgroundColor;
		Color4f hoverColor;
		Color4f pressedColor;
		Color4f pressedHoverColor;
		/// Set to true if the button was pressed and the mouse is held down currently
		bool isBeingPressed;
		/// Set to true if the mouse is currently inside the button
		bool isBeingHovered;
	protected:
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// This function is called by the guiManager after registering this guiElement.
		/// When this function is called, the guiManager pointer, the guiID, and the parent element id are set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Callback function for mouse button input on this guiElement. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
	public:
		/// Constructor
		GuiButton(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Color4f& backgroundColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f),
			const Color4f& hoverColor  = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const Color4f& pressedColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), 
			const Color4f& pressedHoverColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, Color4f textColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE);
		/// Constructor that already registers an event handle
		GuiButton(GuiEventHandle& eventHandle, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Color4f& backgroundColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f),
			const Color4f& hoverColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const Color4f& pressedColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f),
			const Color4f& pressedHoverColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, Color4f textColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE);
		/// Constructors using GuiStyle
		GuiButton(const std::string& label, const GuiStyle& style);
		GuiButton(GuiEventHandle& eventHandle, const std::string& label, const GuiStyle& style);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setEventHandle(GuiEventHandle& eventHandle);
		/// Copy constructor
		GuiButton(const GuiButton& other) noexcept;
		/// Copy assignment operator
		GuiButton& operator=(const GuiButton& other) noexcept;
		/// Move constructor
		GuiButton(GuiButton&& other) noexcept;
		/// Move assignment operator
		GuiButton& operator=(GuiButton&& other) noexcept;
		/// Destructor
		~GuiButton();
	};

}