#pragma once

#include <string>
#include <optional>
#include <memory>

namespace SnackerEngine
{

	template<typename T>
	std::string to_string(const T& val);

	template<typename T>
	std::optional<T> from_string(std::string_view string_view);

	template<typename T>
	class Formatter
	{
	public:
		Formatter() {};
		// TODO: Maybe replace formatter via static functions.
		virtual ~Formatter() = default;
		virtual std::unique_ptr<Formatter<T>> clone() const = 0;
		/// Takes a value and returns a formatted string
		virtual std::string to_string(const T& val);
		/// Takes a string and returns a value
		virtual std::optional<T> from_string(std::string_view string_view);
	};

	class FloatFormatter : public Formatter<float>
	{
	private:
		int digitsBeforeDecimal = -1;
		int digitsAfterDecimal = -1;
	public:
		FloatFormatter(int digitsBeforeDecimal = -1, int digitsAfterDecimal = -1);
		std::unique_ptr<Formatter<float>> clone() const override 
		{
			return std::make_unique<FloatFormatter>(*this);
		}
		/// Takes a value and returns a formatted string
		std::string to_string(const float& val) override;
		/// Getters
		int getDigitsBeforeDecimal() const { return digitsBeforeDecimal; }
		int getDigitsAfterDecimal() const { return digitsAfterDecimal; }
		/// Setters
		void setDigitsBeforeDecimal(int digitsBeforeDecimal) { this->digitsBeforeDecimal = digitsBeforeDecimal; }
		void setDigitsAfterDecimal(int digitsAfterDecimal) { this->digitsAfterDecimal = digitsAfterDecimal; }
	};

	class DoubleFormatter : public Formatter<double>
	{
	private:
		int digitsBeforeDecimal = -1;
		int digitsAfterDecimal = -1;
	public:
		DoubleFormatter(int digitsBeforeDecimal = -1, int digitsAfterDecimal = -1);
		std::unique_ptr<Formatter<double>> clone() const override
		{
			return std::make_unique<DoubleFormatter>(*this);
		}
		/// Takes a value and returns a formatted string
		std::string to_string(const double& val) override;
		/// Getters
		int getDigitsBeforeDecimal() const { return digitsBeforeDecimal; }
		int getDigitsAfterDecimal() const { return digitsAfterDecimal; }
		/// Setters
		void setDigitsBeforeDecimal(int digitsBeforeDecimal) { this->digitsBeforeDecimal = digitsBeforeDecimal; }
		void setDigitsAfterDecimal(int digitsAfterDecimal) { this->digitsAfterDecimal = digitsAfterDecimal; }
	};

	class IntFormatter : public Formatter<int>
	{
	private:
		int digits = -1;
	public:
		IntFormatter(int digits = -1);
		std::unique_ptr<Formatter<int>> clone() const override
		{
			return std::make_unique<IntFormatter>(*this);
		}
		/// Takes a value and returns a formatted string
		std::string to_string(const int& val) override;
		/// Getters
		int getDigits() const { return digits; }
		/// Setters
		void setDigits(int digits) { this->digits = digits; }
	};

	class UnsignedFormatter : public Formatter<unsigned>
	{
	private:
		int digits = -1;
	public:
		UnsignedFormatter(int digits = -1);
		std::unique_ptr<Formatter<unsigned>> clone() const override
		{
			return std::make_unique<UnsignedFormatter>(*this);
		}
		/// Takes a value and returns a formatted string
		std::string to_string(const unsigned& val) override;
		/// Getters
		int getDigits() const { return digits; }
		/// Setters
		void setDigits(int digits) { this->digits = digits; }
	};

	template<typename T>
	inline std::string Formatter<T>::to_string(const T& val)
	{
		return SnackerEngine::to_string(val);
	}

	template<typename T>
	inline std::optional<T> Formatter<T>::from_string(std::string_view string_view)
	{
		return SnackerEngine::from_string(string_view);
	}

}