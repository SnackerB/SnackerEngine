#include "Random.h"
#include <random>
#include <ctime>

namespace SnackerEngine
{

	static std::default_random_engine randomEngine;

	void initializeRNG()
	{
		std::srand(static_cast<unsigned int>(time(NULL)));
		randomEngine.seed(std::rand());
	}

	float randomFloat(float min, float max)
	{
		static std::uniform_real_distribution<float> dist(min, max);
		return dist(randomEngine);
	}

	double randomDouble(double min, double max)
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