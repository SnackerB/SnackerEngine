#include "Gui/Animation/AnimationFunc.h"

namespace SnackerEngine
{

	double animationFunctionLinear(const double& percent)
	{
		return percent;
	}

	double animationFunctionEaseInSine(const double& percent)
	{
		return 1.0 - cos((percent * M_PI) / 2.0);
	}

	double animationFunctionEaseOutSine(const double& percent)
	{
		return sin(percent * M_PI);
	}

	double animationFunctionEaseInOutSine(const double& percent)
	{
		return -(cos(percent * M_PI) - 1) / 2.0;
	}

}