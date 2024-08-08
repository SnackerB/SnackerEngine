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

	template<> bool isOfType<std::string>(const nlohmann::json& json)
	{
		return json.is_string();
	}
	template<> bool isOfType<std::size_t>(const nlohmann::json& json)
	{
		return json.is_number_integer();
	}
	template<> bool isOfType<int>(const nlohmann::json& json)
	{
		return json.is_number_integer();
	}
	template<> bool isOfType<long long int>(const nlohmann::json& json)
	{
		return json.is_number_integer();
	}
	template<> bool isOfType<unsigned>(const nlohmann::json& json)
	{
		return json.is_number_unsigned();
	}
	template<> bool isOfType<float>(const nlohmann::json& json)
	{
		return json.is_number();
	}
	template<> bool isOfType<double>(const nlohmann::json& json)
	{
		return json.is_number();
	}
	template<> bool isOfType<bool>(const nlohmann::json& json)
	{
		return json.is_boolean();
	}

	template<> std::string parseJSON(const nlohmann::json& json)
	{
		return std::string(json);
	}
	template<> std::size_t parseJSON(const nlohmann::json& json)
	{
		return std::size_t(json);
	}
	template<> int parseJSON(const nlohmann::json& json)
	{
		return int(json);
	}
	template<> long long int parseJSON(const nlohmann::json& json)
	{
		return long long int(json);
	}
	template<> unsigned parseJSON(const nlohmann::json& json)
	{
		return unsigned(json);
	}
	template<> float parseJSON(const nlohmann::json& json)
	{
		return float(json);
	}
	template<> double parseJSON(const nlohmann::json& json)
	{
		return double(json);
	}
	template<> bool parseJSON(const nlohmann::json& json)
	{
		return bool(json);
	}

}