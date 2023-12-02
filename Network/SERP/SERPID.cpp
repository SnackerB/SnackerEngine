#include "SERP\SERPID.h"
#include <random>
#include "Utility\Formatting.h"
#include "Utility\Json.h"

namespace SnackerEngine
{

	const SERPID SERPID::SERVER_ID = 0;

	SERPID getRandomSerpID()
	{
		return (std::rand() % 9999) + 1;
	}
	
	template<>
	std::string to_string(const SERPID& serpID)
	{
		std::string result = std::to_string(static_cast<unsigned int>(serpID));
		while (result.size() < 4) {
			result = "0" + result;
		}
		return result;
	}

	template<>
	bool isOfType<SERPID>(const nlohmann::json& json)
	{
		return json.is_number_integer() && static_cast<int>(json) >= 0 && static_cast<int>(json) < 10000;
	}

	template<> 
	SERPID parseJSON(const nlohmann::json& json)
	{
		return static_cast<unsigned int>(json);
	}
	
	template<>
	std::optional<SERPID> from_string(const std::string& string)
	{
		if (string.length() != 4) return {};
		for (const auto& c : string) {
			if (!std::isdigit(c)) return {};
		}
		return static_cast<unsigned int>(std::stoul(string));
	}

}