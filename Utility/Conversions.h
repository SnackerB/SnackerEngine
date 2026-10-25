#pragma once

namespace SnackerEngine
{

	template<typename T>
	inline T roundFromDouble(double value) 
	{
		static_assert(std::is_arithmetic_v<T>, "roundFromDouble requires an arithmetic type!");
		if constexpr (std::is_floating_point_v<T>) {
			return static_cast<T>(value);
		}
		else {
			constexpr double lowest = static_cast<double>(std::numeric_limits<T>::lowest());
			constexpr double highest = static_cast<double>(std::numeric_limits<T>::max());
			if (value <= lowest) return std::numeric_limits<T>::lowest();
			if (value >= highest) return std::numeric_limits<T>::max();
			return static_cast<T>(std::round(value));
		}
	}

	template<typename T>
	T interpolate(const T& a, const T& b, double percentage);

}