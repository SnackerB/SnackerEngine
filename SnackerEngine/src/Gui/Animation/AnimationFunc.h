#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

namespace SnackerEngine
{

	double animationFunctionLinear(const double& percent);

	double animationFunctionEaseInSine(const double& percent);

	double animationFunctionEaseOutSine(const double& percent);

	double animationFunctionEaseInOutSine(const double& percent);

	using AnimationFuncT = double(&)(const double&);

}