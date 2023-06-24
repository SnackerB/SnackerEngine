#include "Utility/Json.h"
#include "Core/Engine.h"

namespace SnackerEngine
{

	std::optional<nlohmann::json> loadJSON(const std::string& path, bool relativeToResourceFolder)
	{
		if (relativeToResourceFolder) {
			std::string fullPath = Engine::getResourcePath();
			fullPath.append(path);
			std::ifstream file(fullPath);
			if (!file.is_open()) {
				SnackerEngine::warningLogger << SnackerEngine::LOGGER::BEGIN << "Could not locate file at " << fullPath << SnackerEngine::LOGGER::ENDL;
				return {};
			}
			nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
			if (data.is_discarded())
			{
				SnackerEngine::warningLogger << SnackerEngine::LOGGER::BEGIN << "Could not parse json file at " << fullPath << SnackerEngine::LOGGER::ENDL;
				return {};
			}
			return data;
		}
		else
		{
			std::ifstream file(path);
			if (!file.is_open()) {
				SnackerEngine::warningLogger << SnackerEngine::LOGGER::BEGIN << "Could not locate file at " << path << SnackerEngine::LOGGER::ENDL;
				return {};
			}
			nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
			if (data.is_discarded())
			{
				SnackerEngine::warningLogger << SnackerEngine::LOGGER::BEGIN << "Could not parse json file at " << path << SnackerEngine::LOGGER::ENDL;
				return {};
			}
			return data;
		}
	}

	template<> bool isOfType<std::string>(const nlohmann::json& json)
	{
		return json.is_string();
	}
	template<> bool isOfType<int>(const nlohmann::json& json)
	{
		return json.is_number_integer();
	}
	template<> bool isOfType<unsigned>(const nlohmann::json& json)
	{
		return json.is_number_unsigned();
	}
	template<> bool isOfType<float>(const nlohmann::json& json)
	{
		return json.is_number_float();
	}
	template<> bool isOfType<double>(const nlohmann::json& json)
	{
		return json.is_number_float();
	}
	template<> bool isOfType<bool>(const nlohmann::json& json)
	{
		return json.is_boolean();
	}

	template<> std::string parseJSON(const nlohmann::json& json)
	{
		return std::string(json);
	}
	template<> int parseJSON(const nlohmann::json& json)
	{
		return int(json);
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