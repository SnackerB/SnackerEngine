#pragma once
#pragma once

#include "Gui/GuiEventHandles/GuiEventHandle2.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiVariableHandle2 : public GuiEventHandle2
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
		GuiVariableHandle2();
		/// Constructor with a given initial value
		GuiVariableHandle2(const T& value);
		/// Sets the value
		void set(const T& value);
		/// Overload for the assignment operator to set the value
		void operator=(const T& value);
		/// Returns a const reference to the stored value
		const T& get();
		/// Implicit conversion to const T& for ease of use
		operator const T& ();
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiVariableHandle2<T>::GuiVariableHandle2()
		: GuiEventHandle2(), val(T{}) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiVariableHandle2<T>::GuiVariableHandle2(const T& value)
		: GuiEventHandle2(), val(value) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiVariableHandle2<T>::set(const T& value)
	{
		reset();
		val = value;
		onHandleUpdate();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiVariableHandle2<T>::operator=(const T& value)
	{
		reset();
		val = value;
		onHandleUpdate();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline const T& GuiVariableHandle2<T>::get()
	{
		reset();
		return val;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiVariableHandle2<T>::operator const T& ()
	{
		reset();
		return val;
	}
	//--------------------------------------------------------------------------------------------------
	using GuiVariableHandleInt2 = GuiVariableHandle2<int>;
	using GuiVariableHandleUnsignedInt2 = GuiVariableHandle2<unsigned int>;
	using GuiVariableHandleFloat2 = GuiVariableHandle2<float>;
	using GuiVariableHandleDouble2 = GuiVariableHandle2<double>;
	//--------------------------------------------------------------------------------------------------
}