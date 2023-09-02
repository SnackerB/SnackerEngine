#pragma once
#include "Gui/GuiEventHandles/GuiVariableHandle.h"
#include <vector>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	class GuiVectorVariableHandle : public GuiEventHandle
	{
	private:
		/// Special variableHandle that stores a pointer to the GuiVectorVariableHandle object it belongs to. If the value
		/// of the variableHandle changes, the GuiVectorVariableHandle object is alerted
		friend class HelperVariableHandle;
		class HelperVariableHandle : public GuiVariableHandle<T>
		{
		private:
			GuiVectorVariableHandle* parentHandle;
		protected:
			/// Function that is called when the event happens. Can be overwritten.
			/// This activates the GuiVectorVariableHandle this handle belongs to
			virtual void onEvent() override;
		public:
			/// Constructor
			HelperVariableHandle(GuiVectorVariableHandle& parentHandle, const T& value = T{})
				: GuiVariableHandle(value), parentHandle(&parentHandle) {}
		};
		/// Vector of variable handles holding the values. If any
		std::vector<HelperVariableHandle> variableHandles;
	protected:
		/// GuiElement needs to be able to modify the value
		friend class GuiElement;
	public:
		/// Default Constructor
		GuiVectorVariableHandle();
		/// Constructor with a given initial value
		GuiVectorVariableHandle(const std::vector<T>& value);
		/// Move constructor and assignment operator
		GuiVectorVariableHandle(GuiVectorVariableHandle&& other) noexcept;
		GuiVectorVariableHandle& operator=(GuiVectorVariableHandle&& other) noexcept;
		/// Sets the value.
		/// It is assumed that the vector has the correct size.
		void set(const std::vector<T>& value);
		/// Sets the value at the given index.
		/// It is assumed that the access is in bounds
		void set(unsigned index, const T& value);
		/// Returns the size of this vectorHandle
		unsigned size() { return N; }
		/// Overload for the assignment operator to set the value
		void operator=(const std::vector<T>& value) { set(value); }
		/// Returns a const reference to the stored value at the given position.
		/// It is assumed that the access is in bounds
		const T& get(unsigned index) const;
		/// Access operator
		const T& operator[](unsigned index) const { return get(index); }
		/// Returns the variableHandle with the given index
		GuiVariableHandle<T>& getVariableHandle(unsigned index);
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	void GuiVectorVariableHandle<T, N>::HelperVariableHandle::onEvent()
	{
		parentHandle->activate();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	GuiVectorVariableHandle<T, N>::GuiVectorVariableHandle<T, N>()
		: GuiEventHandle(), variableHandles(N, HelperVariableHandle(*this)) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	GuiVectorVariableHandle<T, N>::GuiVectorVariableHandle<T, N>(const std::vector<T>& value)
		: GuiEventHandle(), variableHandles()
	{
		variableHandles.reserve(N);
		for (const auto& val : value) variableHandles.push_back(HelperVariableHandle(*this, val));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	GuiVectorVariableHandle<T, N>::GuiVectorVariableHandle<T, N>(GuiVectorVariableHandle<T, N>&& other) noexcept
		: GuiEventHandle(std::move(other)), variableHandles(std::move(other.variableHandles))
	{
		for (const auto& variableHandle : variableHandles) variableHandle.parentHandle = this;
		other.variableHandles.clear();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	GuiVectorVariableHandle<T, N>& GuiVectorVariableHandle<T, N>::operator=(GuiVectorVariableHandle<T, N>&& other) noexcept
	{
		GuiEventHandle::operator=(std::move(other));
		variableHandles.clear();
		variableHandles = std::move(other);
		for (const auto& variableHandle : variableHandles) variableHandle.parentHandle = this;
		other.variableHandles.clear();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	void GuiVectorVariableHandle<T, N>::set(const std::vector<T>& value)
	{
		for (const auto& variableHandle : variableHandles) variableHandle.set(value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	void GuiVectorVariableHandle<T, N>::set(unsigned index, const T& value)
	{
		variableHandles[index].set(value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	const T& GuiVectorVariableHandle<T, N>::get(unsigned index) const
	{
		return variableHandles[index].get();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T, unsigned N>
	GuiVariableHandle<T>& GuiVectorVariableHandle<T, N>::getVariableHandle(unsigned index)
	{
		return variableHandles[index];
	}
	//--------------------------------------------------------------------------------------------------
}