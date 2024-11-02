#pragma once

#include <functional>

namespace SnackerEngine
{

	namespace AnimationFunction
	{

		using AnimationFunction = std::function<double(double)>;

		double linear(double x);
		double easeInOutSine(double x);
		double easeInOutQuad(double x);
		double easeInOutCubic(double x);
		double easeInOutExponential(double x);
		double easeOutElastic(double x);
		double easeOutBounce(double x);

	}

}