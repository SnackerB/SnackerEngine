#include "Gui\Group.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	bool GuiGroup::join(GuiID element)
	{
		for (auto e : elements) {
			if (e == element) return false;
		}
		elements.push_back(element);
		onElementJoin(element, elements.size() - 1);
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGroup::leave(GuiID element)
	{
		for (auto it = elements.begin(); it != elements.end(); ++it) {
			if (*it == element) {
				onElementLeave(element, it - elements.begin());
				elements.erase(it);
				return;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
}