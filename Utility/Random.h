#pragma once

#include <limits>

namespace SnackerEngine
{

	/// Should be called on program start by the engine
	void initializeRNG();

	float randomFloat(float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max());

	double randomDouble(double min = std::numeric_limits<double>::min(), double max = std::numeric_limits<double>::max());

	/// Returns a random integer in [min, max]
	int randomInteger(int min, int max);

	/// Returns a random integer in [0, max]
	int randomInteger(int max);

}