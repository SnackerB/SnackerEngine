#pragma once

#include <limits>

namespace SnackerEngine
{

	float randomFloat(float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max());

	float randomDouble(double min = std::numeric_limits<double>::min(), double max = std::numeric_limits<double>::max());

}