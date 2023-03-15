#pragma once

#include <optional>
#include <string>

namespace SnackerEngine
{
	template<typename T>
	std::optional<T> convertFromString(const std::string& string)
	{
		return {};
	}

	template<typename T>
	std::string convertToString(const T& value)
	{
		return std::to_string(value);
	}

	template<typename T>
	T roundFromDouble(const double& value)
	{
		return T();
	}

	template<>
	std::optional<int> convertFromString<int>(const std::string& string)
	{
		try {
			return static_cast<int>(std::strtol(string.c_str(), 0, 10));
		}
		catch (...) {
			return {};
		}
	}

	template<>
	int roundFromDouble(const double& value)
	{
		return static_cast<int>(std::lround(value));
	}

	template<>
	std::optional<unsigned int> convertFromString<unsigned int>(const std::string& string)
	{
		try {
			return static_cast<unsigned int>(std::strtoul(string.c_str(), 0, 10));
		}
		catch (...) {
			return {};
		}
	}

	template<>
	unsigned int roundFromDouble(const double& value)
	{
		return static_cast<unsigned int>(std::max(0, static_cast<int>(std::lround(value))));
	}

	template<>
	std::optional<float> convertFromString<float>(const std::string& string)
	{
		try {
			return static_cast<float>(std::strtof(string.c_str(), 0));
		}
		catch (...) {
			return {};
		}
	}

	template<>
	float roundFromDouble(const double& value)
	{
		return static_cast<float>(value);
	}

	template<>
	std::optional<double> convertFromString<double>(const std::string& string)
	{
		try {
			return static_cast<double>(std::strtod(string.c_str(), 0));
		}
		catch (...) {
			return {};
		}
	}

	template<>
	double roundFromDouble(const double& value)
	{
		return value;
	}

	template<>
	std::optional<uint16_t> convertFromString<uint16_t>(const std::string& string)
	{
		try {
			long result = std::strtol(string.c_str(), 0, 10);
			if (result >= 0 && result <= 65535) return static_cast<uint16_t>(result);
			if (result > 65535) return static_cast<uint16_t>(65535);
			return static_cast<uint16_t>(0);
		}
		catch (...) {
			return {};
		}
	}

	template<>
	uint16_t roundFromDouble(const double& value)
	{
		long result = std::lround(value);
		if (result >= 0 && result <= 65535) return static_cast<uint16_t>(result);
		if (result > 65535) return static_cast<uint16_t>(65535);
		return static_cast<uint16_t>(0);
	}

	template<>
	std::optional<std::string> convertFromString<std::string>(const std::string& string)
	{
		return string;
	}

	template<>
	std::string convertToString(const std::string& value)
	{
		return value;
	}

	template<typename T>
	T interpolate(const T& a, const T& b, const double& percentage)
	{
		return a + roundFromDouble<T>((static_cast<double>(b) - static_cast<double>(a)) * percentage);
	}

}