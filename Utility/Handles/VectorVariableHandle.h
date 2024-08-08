#pragma once

#include "VariableHandle.h"
#include <vector>
#undef min

namespace SnackerEngine
{

	template<typename T>
	class VectorVariableHandle
	{
	private:
		/// This bool is set to true if any of the child event handles is activated
		bool active;
		/// Class for the child event handles
		class ChildHandle : public VariableHandle<T>
		{
		private:
			friend class VectorVariableHandle;
			/// The parent handle of this childHandle
			VectorVariableHandle<T>* parentHandle;
		protected:
			/// function that is called when the event handle is activated
			void onEvent() override {
				VariableHandle<T>::onEvent();
				parentHandle->active = true;
				parentHandle->onEvent();
			}
		public:
			/// Constructor
			ChildHandle(VectorVariableHandle<T>* parentHandle = nullptr)
				: VariableHandle<T>(), parentHandle(parentHandle) {}
		};
		/// Vector of child handles
		std::vector<ChildHandle> childHandles;
	protected:
		/// Function that is called when the any of the child event handles activated. Can be overwritten
		virtual void onEvent() {}
	public:
		/// Constructor
		VectorVariableHandle(std::size_t count = 0);
		VectorVariableHandle(const std::vector<T>& value);
		/// Copy constructor and assignment operator
		VectorVariableHandle(const VectorVariableHandle<T>& other) noexcept;
		VectorVariableHandle<T>& operator=(const VectorVariableHandle<T>& other) noexcept;
		/// Move constructor and assignment operator
		VectorVariableHandle(VectorVariableHandle<T>&& other) noexcept;
		VectorVariableHandle<T>& operator=(VectorVariableHandle<T>&& other) noexcept;
		/// Returns the boolean active
		bool isActive() const { return active; };
		/// Sets the active boolean back to false and calls reset() on all child event handles
		void reset();
		/// Returns a reference to the VariableHandle at the given index
		VariableHandle<T>& get(std::size_t index) { return childHandles[index]; }
		VariableHandle<T>& operator[](std::size_t index) { return get(index); }
		/// Returns a reference to the last VariableHandle
		VariableHandle<T>& back() { return childHandles.back(); }
		const VariableHandle<T>& back() const { return childHandles.back(); }
		/// Returns the current number of child handles
		std::size_t size() const { return childHandles.size(); }
		/// Creates a new event handle and pushes it into the child handles vector
		VariableHandle<T>& createNewVariableHandle() { childHandles.push_back(ChildHandle(this)); return childHandles.back(); }
		/// Clears childHandles
		void clear() { childHandles.clear(); }
		/// Resizes the childHandle vector to the given size
		void resize(std::size_t size);
		/// Returns the value of the variableHandle at the given index
		const T& get(std::size_t index) const { return childHandles[index].get(); }
		/// Returns a vector of values of all values
		std::vector<T> get();
		/// Sets the value of the variableHandle at the given index
		void set(const T& value, std::size_t index) { childHandles[index].set(value); }
		/// Sets the values of all childHandles at once
		void set(const std::vector<T>& values);
	};

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(std::size_t count)
		: active{ false }, childHandles{}
	{
		for (std::size_t i = 0; i < count; ++i) childHandles.push_back(ChildHandle(this));
	}

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(const std::vector<T>& value)
		: active{ false }, childHandles{}
	{
		for (const T& val : value) {
			childHandles.push_back(ChildVariableHandle(this));
			childHandles.back().set(val);
		}
	}

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(const VectorVariableHandle<T>& other) noexcept
		: VectirVariableHandle<T>(other.size()) {}

	template<typename T>
	inline VectorVariableHandle<T>& VectorVariableHandle<T>::operator=(const VectorVariableHandle<T>& other) noexcept
	{
		active = false;
		childHandles.clear();
		for (std::size_t i = 0; i < other.size(); ++i) childHandles.push_back(ChildVariableHandle(this));
	}

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(VectorVariableHandle<T>&& other) noexcept
		: active(other.active), childHandles(std::move(other.childHandles))
	{
		for (auto& childHandle : childHandles) childHandle.parentHandle = this;
	}

	template<typename T>
	inline VectorVariableHandle<T>& VectorVariableHandle<T>::operator=(VectorVariableHandle<T>&& other) noexcept
	{
		active = other.active;
		childHandles = std::move(other.childHandles);
		for (auto& childHandle : childHandles) childHandle.parentHandle = this;
		return *this;
	}

	template<typename T>
	inline void VectorVariableHandle<T>::reset()
	{
		active = false;
		for (auto& childHandle : childHandles) childHandle.reset();
	}

	template<typename T>
	inline void VectorVariableHandle<T>::resize(std::size_t size)
	{
		std::size_t oldSize = childHandles.size();
		childHandles.resize(size);
		for (std::size_t i = oldSize; i < size; ++i) {
			childHandles[i].parentHandle = this;
		}
	}

	template<typename T>
	inline std::vector<T> VectorVariableHandle<T>::get()
	{
		std::vector<T> result;
		for (const auto& handle : childHandles) result.push_back(handle.get());
		return result;
	}

	template<typename T>
	inline void VectorVariableHandle<T>::set(const std::vector<T>& values)
	{
		for (std::size_t i = 0; i < std::min(values.size(), childHandles.size()); ++i) {
			childHandles[i].set(values[i]);
		}
	}

}