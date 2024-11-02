#include "Alignment.h"
#include "Json.h"

#include "External\nlohmann_json\json.hpp"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType(const nlohmann::json& json, JsonTag<AlignmentHorizontal> tag)
	{
		if (!json.is_string()) return false;
		if (json == "LEFT" ||
			json == "CENTER" ||
			json == "RIGHT") return true;
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	template<> AlignmentHorizontal parseJSON(const nlohmann::json& json, JsonTag<AlignmentHorizontal> tag)
	{
		if (json == "LEFT") return AlignmentHorizontal::LEFT;
		if (json == "CENTER") return AlignmentHorizontal::CENTER;
		if (json == "RIGHT") return AlignmentHorizontal::RIGHT;
		return AlignmentHorizontal::CENTER;
	}
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType(const nlohmann::json& json, JsonTag<AlignmentVertical> tag)
	{
		if (!json.is_string()) return false;
		if (json == "TOP" ||
			json == "CENTER" ||
			json == "BOTTOM") return true;
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	template<> AlignmentVertical parseJSON(const nlohmann::json& json, JsonTag<AlignmentVertical> tag)
	{
		if (json == "TOP") return AlignmentVertical::TOP;
		if (json == "CENTER") return AlignmentVertical::CENTER;
		if (json == "BOTTOM") return AlignmentVertical::BOTTOM;
		return AlignmentVertical::CENTER;
	}
	//--------------------------------------------------------------------------------------------------
}