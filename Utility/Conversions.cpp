#include "Conversions.h"

namespace SnackerEngine
{

	template<>
	int interpolate(const int& a, const int& b, double percentage)
	{
		return a + roundFromDouble<int>((static_cast<double>(b) - static_cast<double>(a)) * percentage);
	}

	template<>
	unsigned interpolate(const unsigned& a, const unsigned& b, double percentage)
	{
		return roundFromDouble<unsigned>(static_cast<double>(a) + ((static_cast<double>(b) - static_cast<double>(a)) * percentage));
	}

	template<>
	float interpolate(const float& a, const float& b, double percentage)
	{
		return a + roundFromDouble<float>((static_cast<double>(b) - static_cast<double>(a)) * percentage);
	}

	template<>
	double interpolate(const double& a, const double& b, double percentage)
	{
		return a + (b - a) * percentage;
	}
}