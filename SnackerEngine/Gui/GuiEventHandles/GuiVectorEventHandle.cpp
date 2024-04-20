#include "Gui/GuiEventHandles/GuiVectorEventHandle.h"

namespace SnackerEngine
{
	/*
	GuiVectorEventHandle::GuiVectorEventHandle(std::size_t count)
		: active(false), childHandles{}
	{
		for (std::size_t i = 0; i < count; ++i) childHandles.push_back(ChildEventHandle(this));
	}

	void GuiVectorEventHandle::reset()
	{
		active = false;
		for (auto& childHandle : childHandles) childHandle.reset();
	}

	void GuiVectorEventHandle::resize(std::size_t size)
	{
		std::size_t oldSize = childHandles.size();
		childHandles.resize(size);
		for (std::size_t i = oldSize; i < size; ++i) {
			childHandles[i].parentHandle = this;
		}
	}
	*/
}