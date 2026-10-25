#pragma once

#include <cstdint>

namespace SnackerEngine
{

	/// Seeds the PRNG.
	void seedRNG(std::uint32_t seed);

	float randomFloat(float min = 0.0f, float max = 1.0f);

	double randomDouble(double min = 0.0, double max = 0.0);

	/// Returns a random integer in [min, max]
	int randomInteger(int min, int max);

	/// Returns a random integer in [0, max]
	int randomInteger(int max);

}