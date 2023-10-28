#include "Random.h"
#include <random>

namespace SnackerEngine
{

	static std::mt19937 randomEngine;

	void initializeRNG()
	{
		srand(time(NULL));
		randomEngine = std::mt19937(std::rand());
	}

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

	int randomInteger(int min, int max)
	{
		static std::uniform_int_distribution<> dist(min, max);
		return dist(randomEngine);
	}

	int randomInteger(int max)
	{
		return randomInteger(0, max);
	}

}