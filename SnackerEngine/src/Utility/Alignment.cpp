#include "Utility/Alignment.h"
#include "Utility/Json.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType<AlignmentHorizontal>(const nlohmann::json& json)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "AlignmentHorizontal has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "LEFT" ||
			json == "CENTER" ||
			json == "RIGHT") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid AlignmentHorizontal." << LOGGER::ENDL;
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	template<> AlignmentHorizontal parseJSON(const nlohmann::json& json)
	{
		if (json == "LEFT") return AlignmentHorizontal::LEFT;
		if (json == "CENTER") return AlignmentHorizontal::CENTER;
		if (json == "RIGHT") return AlignmentHorizontal::RIGHT;
		return AlignmentHorizontal::CENTER;
	}
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType<AlignmentVertical>(const nlohmann::json& json)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "AlignmentVertical has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "TOP" ||
			json == "CENTER" ||
			json == "BOTTOM") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid AlignmentVertical." << LOGGER::ENDL;
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	template<> AlignmentVertical parseJSON(const nlohmann::json& json)
	{
		if (json == "TOP") return AlignmentVertical::TOP;
		if (json == "CENTER") return AlignmentVertical::CENTER;
		if (json == "BOTTOM") return AlignmentVertical::BOTTOM;
		return AlignmentVertical::CENTER;
	}
	//--------------------------------------------------------------------------------------------------
}