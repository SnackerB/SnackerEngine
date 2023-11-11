#pragma once

#include "Gui/GuiEventHandles/GuiEventHandle.h"

namespace SnackerEngine
{

	class GuiVectorEventHandle
	{
	private:
		/// This bool is set to true if any of the child event handles is activated
		bool active;
		/// Class for the child event handles
		class ChildEventHandle : public GuiEventHandle
		{
		private:
			friend class GuiVectorEventHandle;
			/// The parent handle of this childHandle
			GuiVectorEventHandle* parentHandle;
		protected:
			/// function that is called when the event handle is activated
			void onEvent() override {
				GuiEventHandle::onEvent();
				parentHandle->active = true;
				parentHandle->onEvent();
			}
		public:
			/// Constructor 
			ChildEventHandle(GuiVectorEventHandle* parentHandle = nullptr)
				: GuiEventHandle(), parentHandle(parentHandle) {}
		};
		/// Vector of child handles
		std::vector<ChildEventHandle> childHandles;
	protected:
		/// Function that is called when the any of the child event handles activated. Can be overwritten
		virtual void onEvent() {}
	public:
		/// Constructor
		GuiVectorEventHandle(std::size_t count = 0);
		/// Returns the boolean active
		bool isActive() const { return active; };
		/// Sets the active boolean back to false and calls reset() on all child event handles
		void reset();
		/// Returns a reference to the GuiEventHandle at the given index
		GuiEventHandle& get(std::size_t index) { return childHandles[index]; }
		GuiEventHandle& operator[](std::size_t index) { return get(index); }
		/// Returns the current number of child handles
		std::size_t size() const { return childHandles.size(); }
		/// Creates a new event handle and pushes it into the child handles vector
		GuiEventHandle& createNewEventHandle() { childHandles.push_back(ChildEventHandle(this)); return childHandles.back(); }
		/// Clears childHandles
		void clear() { childHandles.clear(); }
		/// Resizes the childHandle vector to the given size
		void resize(std::size_t size);
	};

}