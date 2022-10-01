#include "Gui/Layouts/VerticalLayout.h"
#include "Core/Log.h"

namespace SnackerEngine
{

	VerticalLayout::VerticalLayout() {}

	void VerticalLayout::enforceLayout()
	{
		infoLogger << LOGGER::BEGIN << "Enforced vertical layout!" << LOGGER::ENDL;
	}

	void VerticalLayout::addElement(const GuiID& guiID, const LayoutOptions& options)
	{
		infoLogger << LOGGER::BEGIN << "Added element to vertical layout!" << LOGGER::ENDL;
	}

}