#include "Random.h"
#include <random>

namespace SnackerEngine
{

	static std::mt19937 randomEngine(std::rand());

	float randomFloat(float min, float max)
	{
		static std::uniform_real_distribution<float> dist(min, max);
		return dist(randomEngine);
	}

	float randomDouble(double min, double max)
	{
		static std::uniform_real_distribution<double> dist(min, max);
		return dist(randomEngine);
	}

}