#include "Gui/GuiEventHandles/GuiEventHandle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiEventHandle::activate()
	{
		active = true;
		onEvent();
		onHandleUpdate();
	}
	//--------------------------------------------------------------------------------------------------
	GuiEventHandle::GuiEventHandle()
		: GuiHandle(), active(false) {}
	//--------------------------------------------------------------------------------------------------
	bool GuiEventHandle::isActive() const
	{
		return active;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiEventHandle::reset()
	{
		active = false;
	}
	//--------------------------------------------------------------------------------------------------
}
