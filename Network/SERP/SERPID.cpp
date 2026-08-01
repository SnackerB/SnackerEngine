#include "SERPID.h"
#include "Utility/Formatting.h"
#include "Utility/Json.h"

#include <random>

namespace SnackerEngine
{

	const SERPID SERPID::SERVER_ID = static_cast<unsigned int>(0);

	SERPID getRandomSerpID()
	{
		return SERPID(static_cast<unsigned int>((std::rand() % 9999) + 1));
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
	bool isOfType(const nlohmann::json& json, JsonTag<SERPID> tag)
	{
		return json.is_number_integer() && static_cast<int>(json) >= 0 && static_cast<int>(json) < 10000;
	}

	template<> 
	SERPID parseJSON(const nlohmann::json& json, JsonTag<SERPID> tag)
	{
		return static_cast<unsigned int>(json);
	}
	
	template<>
	std::optional<SERPID> from_string(std::string_view string_view)
	{
		if (string_view.length() != 4) return {};
		for (const auto& c : string_view) {
			if (!std::isdigit(c)) return {};
		}
		return static_cast<unsigned int>(std::stoul(string_view.data()));
	}

}