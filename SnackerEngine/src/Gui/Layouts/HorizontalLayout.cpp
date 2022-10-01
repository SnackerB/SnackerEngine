#include "Gui/Layouts/HorizontalLayout.h"
#include "Core/Log.h"

namespace SnackerEngine
{

	HorizontalLayout::HorizontalLayout() {}

	void HorizontalLayout::enforceLayout()
	{
		infoLogger << LOGGER::BEGIN << "Enforced horizontal layout!" << LOGGER::ENDL;
	}

	void HorizontalLayout::addElement(const GuiID& guiID, const LayoutOptions& options)
	{
		infoLogger << LOGGER::BEGIN << "Added element to horizontal layout!" << LOGGER::ENDL;
	}

}