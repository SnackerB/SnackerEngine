#include "AnimationFunctions.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace SnackerEngine
{

	namespace AnimationFunction
	{

		// Implementations taken from https://easings.net/de#easeOutElastic

		double linear(double x)
		{
			return x;
		}

		double easeInOutSine(double x)
		{
			return -(cos(M_PI * x) - 1) / 2;
		}

		double easeInOutQuad(double x)
		{
			return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
		}

		double easeInOutCubic(double x)
		{
			return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
		}

		double easeInOutExponential(double x)
		{
			return x == 0
				? 0
				: x == 1
				? 1
				: x < 0.5 ? pow(2, 20 * x - 10) / 2
				: (2 - pow(2, -20 * x + 10)) / 2;
		}

		double easeOutElastic(double x)
		{
			constexpr double c4 = (2.0 * M_PI) / 3.0;
			return x == 0
				? 0
				: x == 1
				? 1
				: pow(2.0, -10.0 * x) * sin((x * 10.0 - 0.75) * c4) + 1.0;
		}

		double easeOutBounce(double x)
		{
			constexpr double n1 = 7.5625;
			constexpr double d1 = 2.75;

			if (x < 1 / d1) {
				return n1 * x * x;
			}
			else if (x < 2 / d1) {
				return n1 * (x -= 1.5 / d1) * x + 0.75;
			}
			else if (x < 2.5 / d1) {
				return n1 * (x -= 2.25 / d1) * x + 0.9375;
			}
			else {
				return n1 * (x -= 2.625 / d1) * x + 0.984375;
			}
		}

	}

}