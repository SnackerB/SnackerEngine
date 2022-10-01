#pragma once

#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/GuiEventHandles/GuiEventHandle.h"

namespace SnackerEngine
{

	class GuiButton : public GuiPanel
	{
	private:
		/// The event that happens when the button is pressed
		GuiEventHandle* eventHandle;
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
	public:
		/// Constructor
		GuiButton(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Color3f& backgroundColor = Color3f());
		/// Constructor that already registers an event handle
		GuiButton(GuiEventHandle& eventHandle, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Color3f& backgroundColor = Color3f());
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setEventHandle(GuiEventHandle& eventHandle);
		/// Copy constructor
		GuiButton(GuiButton& other) noexcept;
		/// Copy assignment operator
		GuiButton& operator=(GuiButton& other) noexcept;
		/// Move constructor
		GuiButton(GuiButton&& other) noexcept;
		/// Move assignment operator
		GuiButton& operator=(GuiButton&& other) noexcept;
		/// Destructor
		~GuiButton();
	};

}