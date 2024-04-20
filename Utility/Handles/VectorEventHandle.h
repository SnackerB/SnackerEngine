#pragma once

#include "EventHandle.h"
#include <vector>

namespace SnackerEngine
{

	class VectorEventHandle
	{
	private:
		/// This bool is set to true if any of the child event handles is activated
		bool active;
		/// Class for the child event handles
		class ChildEventHandle : public EventHandle
		{
		private:
			friend class VectorEventHandle;
			/// The parent handle of this childHandle
			VectorEventHandle* parentHandle;
		protected:
			/// function that is called when the event handle is activated
			void onEvent() override {
				EventHandle::onEvent();
				parentHandle->active = true;
				parentHandle->onEvent();
			}
		public:
			/// Constructor
			ChildEventHandle(VectorEventHandle* parentHandle = nullptr)
				: EventHandle(), parentHandle(parentHandle) {}
		};
		/// Vector of child handles
		std::vector<ChildEventHandle> childHandles;
	protected:
		/// Function that is called when the any of the child event handles activated. Can be overwritten
		virtual void onEvent() {}
	public:
		/// Constructor
		VectorEventHandle(std::size_t count = 0);
		/// Copy constructor and assignment operator
		VectorEventHandle(const VectorEventHandle& other) noexcept;
		VectorEventHandle& operator=(const VectorEventHandle& other) noexcept;
		/// Move constructor and assignment operator
		VectorEventHandle(VectorEventHandle&& other) noexcept;
		VectorEventHandle& operator=(VectorEventHandle&& other) noexcept;
		/// Returns the boolean active
		bool isActive() const { return active; };
		/// Sets the active boolean back to false and calls reset() on all child event handles
		void reset();
		/// Returns a reference to the GuiEventHandle at the given index
		EventHandle& get(std::size_t index) { return childHandles[index]; }
		EventHandle& operator[](std::size_t index) { return get(index); }
		/// Returns the current number of child handles
		std::size_t size() const { return childHandles.size(); }
		/// Returns a reference to the last GuiEventHandle
		EventHandle& back() { return childHandles.back(); }
		const EventHandle& back() const { return childHandles.back(); }
		/// Creates a new event handle and pushes it into the child handles vector
		EventHandle& createNewEventHandle() { childHandles.push_back(ChildEventHandle(this)); return childHandles.back(); }
		/// Clears childHandles
		void clear() { childHandles.clear(); }
		/// Resizes the childHandle vector to the given size
		void resize(std::size_t size);
	};

}