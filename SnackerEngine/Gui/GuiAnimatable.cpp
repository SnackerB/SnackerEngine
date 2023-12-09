#include "Gui\GuiAnimatable.h"
#include "Gui\GuiElement.h"

namespace SnackerEngine
{

	GuiElementAnimatable::GuiElementAnimatable(GuiElement* element, double duration, std::function<double(double)> animationFunction)
		: Animatable(duration, animationFunction), element{ element } 
	{
		this->element->signUpAnimatable(*this);
	}

	GuiElementAnimatable::GuiElementAnimatable(const GuiElementAnimatable& other) noexcept
		: Animatable(other), element{ other.element }
	{
		element->signUpAnimatable(*this);
	}

	GuiElementAnimatable& GuiElementAnimatable::operator=(const GuiElementAnimatable& other) noexcept
	{
		if (element) element->signOffAnimatable(*this);
		Animatable::operator=(other);
		element = other.element;
		if (element) element->signUpAnimatable(*this);
		return *this;
	}

	GuiElementAnimatable::GuiElementAnimatable(GuiElementAnimatable&& other) noexcept
		: Animatable(std::move(other)), element{ other.element }
	{
		if (element) element->onGuiAnimatableMove(&other, *this);
		other.element = nullptr;
	}
	
	GuiElementAnimatable& GuiElementAnimatable::operator=(GuiElementAnimatable& other) noexcept
	{
		if (element) element->signOffAnimatable(*this);
		Animatable::operator=(std::move(other));
		element = other.element;
		if (element) element->onGuiAnimatableMove(&other, *this);
		other.element = nullptr;
		return *this;
	}

	GuiElementAnimatable::~GuiElementAnimatable()
	{
		if (element) element->signOffAnimatable(*this);
	}

	GuiElementDelayAnimatable::GuiElementDelayAnimatable(std::unique_ptr<GuiElementAnimatable>&& animatable, double duration)
		: GuiElementAnimatable(animatable->element, duration), animatable(std::move(animatable))
	{
	}

	void GuiElementDelayAnimatable::update(double dt)
	{
		time = std::min(time + dt, duration);
		if (time >= duration && element && animatable) {
			element->signUpAnimatable(std::move(animatable));
			animatable = nullptr;
		}
	}

}