#pragma once

#include "Gui/GuiEventHandles/GuiEventHandle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiVariableHandle : public GuiEventHandle
	{
		/// The variable that is controlled by this handle. Modifying this variable will notify 
		/// the corresponding GuiElement, and the variable can be modified by the corresponding
		/// GuiElement
		T val;
	protected:
		/// GuiElement needs to be able to modify the value
		friend class GuiElement;
	public:
		/// Default Constructor
		GuiVariableHandle();
		/// Constructor with a given initial value
		GuiVariableHandle(const T& value);
		/// Sets the value
		void set(const T& value);
		/// Overload for the assignment operator to set the value
		void operator=(const T& value);
		/// Returns a const reference to the stored value
		const T& get() const;
		/// Implicit conversion to const T& for ease of use
		operator const T& ();
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiVariableHandle<T>::GuiVariableHandle()
		: GuiEventHandle(), val(T{}) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiVariableHandle<T>::GuiVariableHandle(const T& value)
		: GuiEventHandle(), val(value) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiVariableHandle<T>::set(const T& value)
	{
		val = value;
		activate();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiVariableHandle<T>::operator=(const T& value)
	{
		val = value;
		activate();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline const T& GuiVariableHandle<T>::get() const
	{
		return val;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiVariableHandle<T>::operator const T& ()
	{
		return val;
	}
	//--------------------------------------------------------------------------------------------------
	using GuiVariableHandleInt = GuiVariableHandle<int>;
	using GuiVariableHandleUnsignedInt = GuiVariableHandle<unsigned int>;
	using GuiVariableHandleFloat = GuiVariableHandle<float>;
	using GuiVariableHandleDouble = GuiVariableHandle<double>;
	//--------------------------------------------------------------------------------------------------
}