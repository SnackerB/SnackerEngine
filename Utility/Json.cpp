#include "Json.h"

#include <exception>

namespace SnackerEngine
{

	nlohmann::json loadJSON(const std::string& filePath)
	{
		std::ifstream file(filePath);
		if (!file.is_open()) {
			throw std::runtime_error(std::string("Could not locate file at ") + filePath);
		}
		nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
		if (data.is_discarded())
		{
			throw std::runtime_error(std::string("Could not parse json file at ") + filePath);
		}
		return data;
	}

	void saveJSON(const nlohmann::json& json, const std::string& filePath)
	{
		std::ofstream file(filePath);
		if (!file.is_open()) {
			throw std::runtime_error(std::string("Could not open file for writing at ") + filePath);
		}
		file << json.dump();
	}

	template<> bool isOfType(const nlohmann::json& json, JsonTag<std::string> tag)
	{
		return json.is_string();
	}
	template<> bool isOfType<std::size_t>(const nlohmann::json& json, JsonTag<std::size_t> tag)
	{
		return json.is_number_integer();
	}
	template<> bool isOfType<int>(const nlohmann::json& json, JsonTag<int> tag)
	{
		return json.is_number_integer();
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<long long int> tag)
	{
		return json.is_number_integer();
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<unsigned> tag)
	{
		return json.is_number_unsigned();
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<float> tag)
	{
		return json.is_number();
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<double> tag)
	{
		return json.is_number();
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<bool> tag)
	{
		return json.is_boolean();
	}

	template<> std::string parseJSON(const nlohmann::json& json, JsonTag<std::string> tag)
	{
		return static_cast<std::string>(json);
	}
	template<> std::size_t parseJSON(const nlohmann::json& json, JsonTag<std::size_t> tag)
	{
		return static_cast<std::size_t>(json);
	}
	template<> int parseJSON(const nlohmann::json& json, JsonTag<int> tag)
	{
		return static_cast<int>(json);
	}
	template<> long long int parseJSON(const nlohmann::json& json, JsonTag<long long int> tag)
	{
		return static_cast<long long int>(json);
	}
	template<> unsigned parseJSON(const nlohmann::json& json, JsonTag<unsigned> tag)
	{
		return static_cast<unsigned>(json);
	}
	template<> float parseJSON(const nlohmann::json& json, JsonTag<float> tag)
	{
		return static_cast<float>(json);
	}
	template<> double parseJSON(const nlohmann::json& json, JsonTag<double> tag)
	{
		return static_cast<double>(json);
	}
	template<> bool parseJSON(const nlohmann::json& json, JsonTag<bool> tag)
	{
		return static_cast<bool>(json);
	}

}