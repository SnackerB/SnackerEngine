#pragma once

#include "Animatable.h"

namespace SnackerEngine
{

	class GuiElement;
	class GuiManager;

	class GuiElementAnimatable : public Animatable
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
		GuiElementAnimatable(GuiElement& guiElement, const double& duration, AnimationFuncT animationFunction = animationFunctionLinear);
	};

}