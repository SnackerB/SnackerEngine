#pragma once

#include "VariableHandle.h"
#include <vector>

namespace SnackerEngine
{

	template<typename T>
	class VectorVariableHandle
	{
	private:
		/// This bool is set to true if any of the child event handles is activated
		bool active;
		/// Class for the child event handles
		class ChildVariableHandle : public VariableHandle<T>
		{
		private:
			friend class VectorVariableHandle;
			/// The parent handle of this ChildVariableHandle
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
			ChildVariableHandle(VectorVariableHandle<T>* parentHandle = nullptr)
				: VariableHandle<T>(), parentHandle(parentHandle) {}
		};
		/// Vector of child handles
		std::vector<ChildVariableHandle> ChildVariableHandles;
	protected:
		/// Function that is called when the any of the child event handles activated. Can be overwritten
		virtual void onEvent() {}
	public:
		VectorVariableHandle(std::size_t count = 0);
		VectorVariableHandle(const std::vector<T>& value);
		VectorVariableHandle(const VectorVariableHandle<T>& other) noexcept;
		VectorVariableHandle<T>& operator=(const VectorVariableHandle<T>& other) noexcept;
		VectorVariableHandle(VectorVariableHandle<T>&& other) noexcept;
		VectorVariableHandle<T>& operator=(VectorVariableHandle<T>&& other) noexcept;
		virtual ~VectorVariableHandle() = default;
		/// Returns the boolean active
		bool isActive() const { return active; };
		/// Sets the active boolean back to false and calls reset() on all child event handles
		void reset();
		/// Returns a reference to the VariableHandle at the given index
		VariableHandle<T>& get(std::size_t index) { return ChildVariableHandles[index]; }
		VariableHandle<T>& operator[](std::size_t index) { return get(index); }
		/// Returns a reference to the last VariableHandle
		VariableHandle<T>& back() { return ChildVariableHandles.back(); }
		const VariableHandle<T>& back() const { return ChildVariableHandles.back(); }
		/// Returns the current number of child handles
		std::size_t size() const { return ChildVariableHandles.size(); }
		/// Creates a new event handle and pushes it into the child handles vector
		VariableHandle<T>& createNewVariableHandle() { ChildVariableHandles.push_back(ChildVariableHandle(this)); return ChildVariableHandles.back(); }
		/// Clears ChildVariableHandles
		void clear() { ChildVariableHandles.clear(); }
		/// Resizes the ChildVariableHandle vector to the given size
		void resize(std::size_t size);
		/// Returns the value of the variableHandle at the given index
		const T& get(std::size_t index) const { return ChildVariableHandles[index].get(); }
		/// Returns a vector of values of all values
		std::vector<T> get() const;
		/// Sets the value of the variableHandle at the given index
		void set(const T& value, std::size_t index) { ChildVariableHandles[index].set(value); }
		/// Sets the values of all ChildVariableHandles at once
		void set(const std::vector<T>& values);
	};

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(std::size_t count)
		: active{ false }, ChildVariableHandles{}
	{
		for (std::size_t i = 0; i < count; ++i) ChildVariableHandles.push_back(ChildVariableHandle(this));
	}

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(const std::vector<T>& value)
		: active{ false }, ChildVariableHandles{}
	{
		for (const T& val : value) {
			ChildVariableHandles.push_back(ChildVariableHandle(this));
			ChildVariableHandles.back().set(val);
		}
	}

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(const VectorVariableHandle<T>& other) noexcept
		: VectorVariableHandle<T>(other.ChildVariableHandles) {}

	template<typename T>
	inline VectorVariableHandle<T>& VectorVariableHandle<T>::operator=(const VectorVariableHandle<T>& other) noexcept
	{
		active = false;
		ChildVariableHandles.clear();
		for (std::size_t i = 0; i < other.size(); ++i) ChildVariableHandles.push_back(ChildVariableHandle(other.ChildVariableHandles[i]));
		return *this;
	}

	template<typename T>
	inline VectorVariableHandle<T>::VectorVariableHandle(VectorVariableHandle<T>&& other) noexcept
		: active(other.active), ChildVariableHandles(std::move(other.ChildVariableHandles))
	{
		for (auto& ChildVariableHandle : ChildVariableHandles) ChildVariableHandle.parentHandle = this;
	}

	template<typename T>
	inline VectorVariableHandle<T>& VectorVariableHandle<T>::operator=(VectorVariableHandle<T>&& other) noexcept
	{
		active = other.active;
		ChildVariableHandles = std::move(other.ChildVariableHandles);
		for (auto& ChildVariableHandle : ChildVariableHandles) ChildVariableHandle.parentHandle = this;
		return *this;
	}

	template<typename T>
	inline void VectorVariableHandle<T>::reset()
	{
		active = false;
		for (auto& ChildVariableHandle : ChildVariableHandles) ChildVariableHandle.reset();
	}

	template<typename T>
	inline void VectorVariableHandle<T>::resize(std::size_t size)
	{
		std::size_t oldSize = ChildVariableHandles.size();
		ChildVariableHandles.resize(size);
		for (std::size_t i = oldSize; i < size; ++i) {
			ChildVariableHandles[i].parentHandle = this;
		}
	}

	template<typename T>
	inline std::vector<T> VectorVariableHandle<T>::get() const
	{
		std::vector<T> result;
		for (const auto& handle : ChildVariableHandles) result.push_back(handle.get());
		return result;
	}

	template<typename T>
	inline void VectorVariableHandle<T>::set(const std::vector<T>& values)
	{
		for (std::size_t i = 0; i < std::min(values.size(), ChildVariableHandles.size()); ++i) {
			ChildVariableHandles[i].set(values[i]);
		}
	}

}