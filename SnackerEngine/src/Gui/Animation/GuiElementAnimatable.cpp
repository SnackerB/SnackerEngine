#include "GuiElementAnimatable.h"
#include "Gui/Animation/GuiElementAnimatable.h"

namespace SnackerEngine
{

	void GuiElementAnimatable::onGuiElementMove(GuiElement& guiElement)
	{
		elementPtr = &guiElement;
	}

	GuiElementAnimatable::GuiElementAnimatable(GuiElement& guiElement, const double& duration, AnimationFuncT animationFunction)
		: Animatable(duration, animationFunction), elementPtr(&guiElement) {}

}
