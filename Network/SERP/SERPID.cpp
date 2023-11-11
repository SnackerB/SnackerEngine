#include "SERP\SERPID.h"
#include <random>

namespace SnackerEngine
{

	SERPID getRandomSerpID()
	{
		return (std::rand() % 9999) + 1;
	}
	
	// TODO: Find solution that doesn't conflict with formatting unsigned ints ...
	/*
	std::string to_string(SERPID serpID)
	{
		std::string result = std::to_string(serpID);
		while (result.size() < 4) {
			result = "0" + result;
		}
		return result;
	}
	*/
	
	std::optional<SERPID> parseSERPID(const std::string& string)
	{
		if (string.length() != 4) return {};
		if (std::all_of(string.begin(), string.end(), std::isdigit)) return static_cast<SERPID>(std::stoul(string));
		return {};
	}

}