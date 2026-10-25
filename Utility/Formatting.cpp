#include "Formatting.h"

#include <string>
#include <cstdint>
#include <charconv>

#include <iostream>

namespace SnackerEngine
{

	template<> 
	std::string to_string(const float& val)
	{
		return std::to_string(val);
	}

	template<>
	std::string to_string(const double& val)
	{
		return std::to_string(val);
	}

	template<>
	std::string to_string(const int& val)
	{
		return std::to_string(val);
	}

	template<>
	std::string to_string(const long long int& val)
	{
		return std::to_string(val);
	}

	template<>
	std::string to_string(const unsigned int& val)
	{
		return std::to_string(val);
	}

	template<>
	std::string to_string(const uint8_t& val)
	{
		return std::to_string(val);
	}

	template<>
	std::string to_string(const uint16_t& val)
	{
		return std::to_string(val);
	}

	template<>
	std::string to_string(const std::size_t& val)
	{
		return std::to_string(val);
	}

	template<>
	std::optional<float> from_string(std::string_view string_view)
	{
		float value{};
		std::from_chars_result result = std::from_chars(string_view.data(), string_view.data() + string_view.size(), value);
		if (result.ec != std::errc{} || result.ptr != string_view.data() + string_view.size())
			return std::nullopt;
		return value;
	}

	template<>
	std::optional<double> from_string(std::string_view string_view)
	{
		double value{};
		std::from_chars_result result = std::from_chars(string_view.data(), string_view.data() + string_view.size(), value);
		if (result.ec != std::errc{} || result.ptr != string_view.data() + string_view.size())
			return std::nullopt;
		return value;
	}

	template<>
	std::optional<int> from_string(std::string_view string_view)
	{
		int value{};
		std::from_chars_result result = std::from_chars(string_view.data(), string_view.data() + string_view.size(), value);
		if (result.ec != std::errc{} || result.ptr != string_view.data() + string_view.size())
			return std::nullopt;
		return value;
	}

	template<>
	std::optional<long long int> from_string(std::string_view string_view)
	{
		long long int value{};
		std::from_chars_result result = std::from_chars(string_view.data(), string_view.data() + string_view.size(), value);
		if (result.ec != std::errc{} || result.ptr != string_view.data() + string_view.size())
			return std::nullopt;
		return value;
	}

	template<>
	std::optional<unsigned int> from_string(std::string_view string_view)
	{
		unsigned int value{};
		std::from_chars_result result = std::from_chars(string_view.data(), string_view.data() + string_view.size(), value);
		if (result.ec != std::errc{} || result.ptr != string_view.data() + string_view.size())
			return std::nullopt;
		return value;
	}

	template <>
	std::optional<std::size_t> from_string(std::string_view string_view)
	{
		std::size_t value{};
		std::from_chars_result result = std::from_chars(string_view.data(), string_view.data() + string_view.size(), value);
		if (result.ec != std::errc{} || result.ptr != string_view.data() + string_view.size())
			return std::nullopt;
		return value;
	}

	static char digitToChar(int digit) {
		if (digit < 0 || digit > 9) return 'X';
		return '0' + digit;
	}

	FloatFormatter::FloatFormatter(int digitsBeforeDecimal, int digitsAfterDecimal)
		: digitsBeforeDecimal(digitsBeforeDecimal), digitsAfterDecimal(digitsAfterDecimal) {}

	std::string SnackerEngine::FloatFormatter::to_string(const float& val)
	{
		return std::to_string(val);
	}

	DoubleFormatter::DoubleFormatter(int digitsBeforeDecimal, int digitsAfterDecimal)
		: digitsBeforeDecimal(digitsBeforeDecimal), digitsAfterDecimal(digitsAfterDecimal) {}

	std::string SnackerEngine::DoubleFormatter::to_string(const double& val)
	{
		if (digitsBeforeDecimal < 0 && digitsAfterDecimal < 0) return SnackerEngine::to_string(val);
		int totalDigits = 0;
		if (digitsBeforeDecimal > 0) totalDigits += digitsBeforeDecimal;
		if (digitsAfterDecimal > 0) totalDigits += digitsAfterDecimal;
		std::string result(totalDigits + 1, '0');
		if (digitsBeforeDecimal > 0) result[digitsBeforeDecimal] = '.';
		int scalar = 1;
		int valueAsInt = static_cast<int>(val);
		for (int i = 0; i < digitsBeforeDecimal; ++i) {
			result[static_cast<std::size_t>(digitsBeforeDecimal) - 1 - i] = digitToChar((valueAsInt % (scalar * 10)) / scalar);
			scalar *= 10;
		}
		double scalar2 = 10.0;
		for (int i = 0; i < digitsAfterDecimal; ++i) {
			result[static_cast<std::size_t>(digitsBeforeDecimal) + 1 + i] = digitToChar(static_cast<std::size_t>(val * scalar2) % static_cast<std::size_t>(10));
			scalar2 *= 10.0;
		}
		return result;
	}

	SnackerEngine::IntFormatter::IntFormatter(int digits)
		: digits(digits) {}

	std::string SnackerEngine::IntFormatter::to_string(const int& val)
	{
		return std::string();
	}

	SnackerEngine::UnsignedFormatter::UnsignedFormatter(int digits)
		: digits(digits) {}

	std::string SnackerEngine::UnsignedFormatter::to_string(const unsigned& val)
	{
		return std::string();
	}

}