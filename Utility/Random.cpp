#include "Random.h"
#include <random>
#include <ctime>
#include <cstdint>

namespace SnackerEngine
{

	static std::mt19937& engine()
	{
		thread_local std::mt19937 randomEngine{ std::random_device{}() };
		return randomEngine;
	}

	void seedRNG(std::uint32_t seed)
	{
		engine().seed(seed);
	}

	float randomFloat(float min, float max)
	{
		return std::uniform_real_distribution<float>(min, max)(engine());
	}

	double randomDouble(double min, double max)
	{
		return std::uniform_real_distribution<double>(min, max)(engine());
	}

	int randomInteger(int min, int max)
	{
		return std::uniform_int_distribution<>(min, max)(engine());
	}

	int randomInteger(int max)
	{
		return randomInteger(0, max);
	}

}