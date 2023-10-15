#pragma once

#include "Gui/GuiEventHandles/GuiVariableHandle.h"

namespace SnackerEngine
{

	template<typename T>
	class GuiVectorVariableHandle
	{
	private:
		/// This bool is set to true if any of the child event handles is activated
		bool active;
		/// Class for the child event handles
		class ChildVariableHandle : public GuiVariableHandle<T>
		{
		private:
			friend class GuiVectorVariableHandle;
			/// The parent handle of this childHandle
			GuiVectorVariableHandle* parentHandle;
		protected:
			/// function that is called when the event handle is activated
			void onEvent() override {
				GuiVariableHandle<T>::onEvent();
				parentHandle->active = true;
				parentHandle->onEvent();
			}
		public:
			/// Constructor 
			ChildVariableHandle(GuiVectorVariableHandle* parentHandle = nullptr)
				: GuiVariableHandle<T>(), parentHandle(parentHandle) {}
		};
		/// Vector of child handles
		std::vector<ChildVariableHandle> childHandles;
	protected:
		/// Function that is called when the any of the child event handles activated. Can be overwritten
		virtual void onEvent() {}
	public:
		/// Constructor
		GuiVectorVariableHandle(unsigned count = 0);
		GuiVectorVariableHandle(const std::vector<T>& value);
		/// Returns the boolean active
		bool isActive() const { return active; };
		/// Sets the active boolean back to false and calls reset() on all child event handles
		void reset();
		/// Returns a reference to the GuiVariableHandle at the given index
		GuiVariableHandle<T>& get(unsigned index) { return childHandles[index]; }
		GuiVariableHandle<T>& operator[](unsigned index) { return get(index); }
		/// Returns the current number of child handles
		unsigned size() const { return childHandles.size(); }
		/// Creates a new event handle and pushes it into the child handles vector
		GuiVariableHandle<T>& createNewVariableHandle() { childHandles.push_back(ChildVariableHandle(this)); return childHandles.back(); }
		/// Clears childHandles
		void clear() { childHandles.clear(); }
		/// Resizes the childHandle vector to the given size
		void resize(unsigned size);
		/// Returns the value of the variableHandle at the given index
		const T& get(unsigned index) const { return childHandles[index].get(); }
		/// Returns a vector of values of all childHandles
		std::vector<T> get();
		/// Sets the value of the variableHandle at the given index
		void set(const T& value, unsigned index) { childHandles[index].set(value); }
		/// Sets the values of all childHandles
		void set(const std::vector<T>& values);
	};

	template<typename T>
	inline GuiVectorVariableHandle<T>::GuiVectorVariableHandle(unsigned count)
		: active{ false }, childHandles{}
	{
		for (unsigned i = 0; i < count; ++i) childHandles.push_back(ChildVariableHandle(this));
	}

	template<typename T>
	inline GuiVectorVariableHandle<T>::GuiVectorVariableHandle(const std::vector<T>& value)
		: active{ false }, childHandles{}
	{
		for (const T& val : value) {
			childHandles.push_back(ChildVariableHandle(this));
			childHandles.back().set(val);
		}
	}

	template<typename T>
	inline void GuiVectorVariableHandle<T>::reset()
	{
		active = false;
		for (auto& childHandle : childHandles) childHandle.reset();
	}

	template<typename T>
	inline void GuiVectorVariableHandle<T>::resize(unsigned size)
	{
		unsigned oldSize = childHandles.size();
		childHandles.resize(size);
		for (unsigned i = oldSize; i < size; ++i) {
			childHandles[i].parentHandle = this;
		}
	}

	template<typename T>
	inline std::vector<T> SnackerEngine::GuiVectorVariableHandle<T>::get()
	{
		std::vector<T> result;
		for (const auto& handle : childHandles) result.push_back(handle.get());
		return result;
	}

	template<typename T>
	inline void SnackerEngine::GuiVectorVariableHandle<T>::set(const std::vector<T>& values)
	{
		for (unsigned i = 0; i < min(values.size(), childHandles.size()); ++i) {
			childHandles[i].set(values[i]);
		}
	}

}