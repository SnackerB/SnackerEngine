#include "Gui/Animation/AnimationFunc.h"
#define _USE_MATH_DEFINES
#include <math.h>

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

	double animationFunctionEaseInOutExpo(const double& percent)
	{
		return percent == 0.0
			? 0.0
			: percent == 1.0
			? 1.0
			: percent < 0.5 ? pow(2.0, 20.0 * percent - 10.0) / 2.0
			: (2.0 - pow(2.0, -20.0 * percent + 10.0)) / 2.0;
	}

	double animationFunctionEaseOutBack(const double& percent)
	{
		const double c1 = 1.70158;
		const double c3 = c1 + 1.0;

		return 1.0 + c3 * pow(percent - 1.0, 3.0) + c1 * pow(percent - 1.0, 2.0);
	}

	double animationFunctionEaseOutBounce(const double& percent)
	{
		const double n1 = 7.5625;
		const double d1 = 2.75;

		if (percent < 1.0 / d1) {
			return n1 * percent * percent;
		}
		else if (percent < 2 / d1) {
			return n1 * (percent - 1.5 / d1) * (percent - 1.5 / d1) + 0.75;
		}
		else if (percent < 2.5 / d1) {
			return n1 * (percent - 2.25 / d1) * (percent - 2.25 / d1) + 0.9375;
		}
		else {
			return n1 * (percent - 2.625 / d1) * (percent - 2.625 / d1) + 0.984375;
		};
	}

	double animationFunctionEaseOutElastic(const double& percent)
	{
		const double c4 = (2.0 * M_PI) / 3.0;

		return percent == 0.0
			? 0.0
			: percent == 1.0
			? 1.0
			: pow(2.0, -10.0 * percent) * sin((percent * 10.0 - 0.75) * c4) + 1.0;
	}

}