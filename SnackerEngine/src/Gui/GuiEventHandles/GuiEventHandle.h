#pragma once

#include "Gui/GuiEventHandles/GuiHandle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiEventHandle : public GuiHandle
	{
	private:
		/// This boolean is set to true by the GuiElement that owns this eventHandle, when the 
		/// event occurs (e.g. on the press of a GuiButton element)
		bool active;
	protected:
		/// GuiElement needs to be able to set the boolean
		friend class GuiElement;
		friend class GuiElement2;
		/// Function that is called when the event happens. Can be overwritten
		virtual void onEvent() {}
		/// Sets the boolean to active and executes the event function. This is called by guiElement
		void activate();
	public:
		/// Constructor
		GuiEventHandle();
		/// Returns the boolean active
		bool isActive() const;
		/// Sets the active boolean back to false
		void reset();
	};
	//--------------------------------------------------------------------------------------------------
}