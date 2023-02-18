#include "Animatable.h"
#include <algorithm>

namespace SnackerEngine
{

	bool Animatable::tick(const float& dt)
	{
		timePassed = std::min(timeFinal, timePassed + dt);
		animate(std::max(0.0, std::min(timePassed / timeFinal, 1.0)));
		return timePassed == timeFinal;
	}

	Animatable::Animatable(const double& duration, AnimationFuncT animationFunction)
		: timePassed(0.0), timeFinal(duration), animationFunction(animationFunction) {}

}
