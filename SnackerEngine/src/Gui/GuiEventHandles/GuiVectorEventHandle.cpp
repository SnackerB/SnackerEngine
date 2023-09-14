#include "Gui/GuiEventHandles/GuiVectorEventHandle.h"

namespace SnackerEngine
{

	GuiVectorEventHandle::GuiVectorEventHandle(unsigned count)
		: active(false), childHandles{}
	{
		for (unsigned i = 0; i < count; ++i) childHandles.push_back(ChildEventHandle(this));
	}

	void GuiVectorEventHandle::reset()
	{
		active = false;
		for (auto& childHandle : childHandles) childHandle.reset();
	}

	void GuiVectorEventHandle::resize(unsigned size)
	{
		unsigned oldSize = childHandles.size();
		childHandles.resize(size);
		for (unsigned i = oldSize; i < size; ++i) {
			childHandles[i].parentHandle = this;
		}
	}

}