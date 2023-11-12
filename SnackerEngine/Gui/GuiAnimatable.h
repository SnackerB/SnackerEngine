#pragma once
#include "Utility/Animatable.h"

namespace SnackerEngine
{

	/// Forward declaration of GuiElement
	class GuiElement;

	template<typename T>
	class GuiElementAnimatable : public Animatable<T>
	{
	private:
		/// Pointer to the element we want to animate an attriubte of
		GuiElement* element;
	public:
		/// Creates a new GuiElementAnimatable for the given element
		GuiElementAnimatable(GuiElement& element);
		/// Copy constructor and assignment operator
		GuiElementAnimatable(const GuiElementAnimatable& other) noexcept;
		GuiElementAnimatable& operator=(const GuiElementAnimatable& other) noexcept;
		/// Move constructor and assignment operator
		GuiElementAnimatable(GuiElementAnimatable&& other) noexcept;
		GuiElementAnimatable& operator=(GuiElementAnimatable& other) noexcept;
	};

}