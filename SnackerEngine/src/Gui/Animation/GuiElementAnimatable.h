#pragma once

#include "Animatable.h"

namespace SnackerEngine
{

	class GuiElement;
	class GuiManager;

	template<typename T>
	class GuiElementAnimatable : public ValueAnimatable<T>
	{
	private:
		friend class GuiManager;
		/// Pointer to the guiElement in question
		GuiElement* elementPtr;
		/// Called by the guiManager when the corresponding guiElement is moved
		void onGuiElementMove(GuiElement& guiElement);
	protected:
		/// Returns a const pointer to the guiElement in question. Careful: Can be nullptr!
		GuiElement* const getGuiElement() { return elementPtr; }
		/// Constructor using a reference to a guiElement
		GuiElementAnimatable(GuiElement& guiElement, const T& initial, const T & final, const double& duration, AnimationFuncT animationFunction = animationFunctionLinear);
	};

	template<typename T>
	inline void GuiElementAnimatable<T>::onGuiElementMove(GuiElement& guiElement)
	{
		elementPtr = &guiElement;
	}

	template<typename T>
	inline GuiElementAnimatable<T>::GuiElementAnimatable(GuiElement& guiElement, const T& initial, const T & final, const double& duration, AnimationFuncT animationFunction)
		: Animatable<T>(initial, final, duration, animationFunction), elementPtr(&guiElement) {}

}