#include "Formatting.h"

#include <string>
#include <vector>

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
	std::string to_string(const unsigned int& val)
	{
		return std::to_string(val);
	}

	template<>
	std::optional<float> from_string(const std::string& string)
	{
		try
		{
			return std::stof(string);
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	template<>
	std::optional<double> from_string(const std::string& string)
	{
		try
		{
			return std::stod(string);
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	template<>
	std::optional<int> from_string(const std::string& string)
	{
		try
		{
			return std::stoi(string);
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	template<>
	std::optional<unsigned int> from_string(const std::string& string)
	{
		try
		{
			return static_cast<unsigned>(std::stoul(string));
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	char digitToChar(int digit) {
		if (digit < 0 || digit > 9) return 'X';
		switch (digit)
		{
		case 0: return '0';
		case 1: return '1';
		case 2: return '2';
		case 3: return '3';
		case 4: return '4';
		case 5: return '5';
		case 6: return '6';
		case 7: return '7';
		case 8: return '8';
		case 9: return '9';
		default:
			break;
		}
		return 'X';
	}

	FloatFormatter::FloatFormatter(int digitsBeforeDecimal, int digitsAfterDecimal)
		: digitsBeforeDecimal(digitsBeforeDecimal), digitsAfterDecimal(digitsAfterDecimal) {}

	std::string SnackerEngine::FloatFormatter::to_string(const float& val)
	{
		return std::string();
	}

	DoubleFormatter::DoubleFormatter(int digitsBeforeDecimal, int digitsAfterDecimal)
		: digitsBeforeDecimal(digitsBeforeDecimal), digitsAfterDecimal(digitsAfterDecimal) {}

	std::string SnackerEngine::DoubleFormatter::to_string(const double& val)
	{
		if (digitsBeforeDecimal < 0 && digitsAfterDecimal < 0) return to_string(val);
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