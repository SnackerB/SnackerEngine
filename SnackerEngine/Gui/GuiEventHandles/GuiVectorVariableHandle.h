#pragma once

#include "Gui/GuiEventHandles/GuiVariableHandle.h"

namespace SnackerEngine
{
	/*
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
		GuiVectorVariableHandle(std::size_t count = 0);
		GuiVectorVariableHandle(const std::vector<T>& value);
		/// Returns the boolean active
		bool isActive() const { return active; };
		/// Sets the active boolean back to false and calls reset() on all child event handles
		void reset();
		/// Returns a reference to the GuiVariableHandle at the given index
		GuiVariableHandle<T>& get(std::size_t index) { return childHandles[index]; }
		GuiVariableHandle<T>& operator[](std::size_t index) { return get(index); }
		/// Returns a reference to the last GuiVariableHandle
		GuiVariableHandle<T>& back() { return childHandles.back(); }
		const GuiVariableHandle<T>& back() const { return childHandles.back(); }
		/// Returns the current number of child handles
		std::size_t size() const { return childHandles.size(); }
		/// Creates a new event handle and pushes it into the child handles vector
		GuiVariableHandle<T>& createNewVariableHandle() { childHandles.push_back(ChildVariableHandle(this)); return childHandles.back(); }
		/// Clears childHandles
		void clear() { childHandles.clear(); }
		/// Resizes the childHandle vector to the given size
		void resize(std::size_t size);
		/// Returns the value of the variableHandle at the given index
		const T& get(std::size_t index) const { return childHandles[index].get(); }
		/// Returns a vector of values of all childHandles
		std::vector<T> get();
		/// Sets the value of the variableHandle at the given index
		void set(const T& value, std::size_t index) { childHandles[index].set(value); }
		/// Sets the values of all childHandles
		void set(const std::vector<T>& values);
	};

	template<typename T>
	inline GuiVectorVariableHandle<T>::GuiVectorVariableHandle(std::size_t count)
		: active{ false }, childHandles{}
	{
		for (std::size_t i = 0; i < count; ++i) childHandles.push_back(ChildVariableHandle(this));
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
	inline void GuiVectorVariableHandle<T>::resize(std::size_t size)
	{
		std::size_t oldSize = childHandles.size();
		childHandles.resize(size);
		for (std::size_t i = oldSize; i < size; ++i) {
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
		for (std::size_t i = 0; i < min(values.size(), childHandles.size()); ++i) {
			childHandles[i].set(values[i]);
		}
	}
	*/
}