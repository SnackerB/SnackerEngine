#include "Alignment.h"
#include "Json.h"

#include "External/nlohmann_json/json.hpp"

namespace SnackerEngine
{
	template<> bool isOfType(const nlohmann::json& json, JsonTag<AlignmentHorizontal> tag)
	{
		if (!json.is_string()) return false;
		if (json == "LEFT" ||
			json == "CENTER" ||
			json == "RIGHT") return true;
		return false;
	}

	template<> AlignmentHorizontal parseJson(const nlohmann::json& json, JsonTag<AlignmentHorizontal> tag)
	{
		if (json == "LEFT") return AlignmentHorizontal::LEFT;
		if (json == "CENTER") return AlignmentHorizontal::CENTER;
		if (json == "RIGHT") return AlignmentHorizontal::RIGHT;
		return AlignmentHorizontal::CENTER;
	}

	template<> nlohmann::json toJson(const AlignmentHorizontal& value)
	{
		switch (value) {
			case AlignmentHorizontal::LEFT: return nlohmann::json("LEFT");
			case AlignmentHorizontal::CENTER: return nlohmann::json("CENTER");
			case AlignmentHorizontal::RIGHT: return nlohmann::json("RIGHT");
		}
	}

	template<> bool isOfType(const nlohmann::json& json, JsonTag<AlignmentVertical> tag)
	{
		if (!json.is_string()) return false;
		if (json == "TOP" ||
			json == "CENTER" ||
			json == "BOTTOM") return true;
		return false;
	}

	template<> AlignmentVertical parseJson(const nlohmann::json& json, JsonTag<AlignmentVertical> tag)
	{
		if (json == "TOP") return AlignmentVertical::TOP;
		if (json == "CENTER") return AlignmentVertical::CENTER;
		if (json == "BOTTOM") return AlignmentVertical::BOTTOM;
		return AlignmentVertical::CENTER;
		return nlohmann::json();
	}

	template<> nlohmann::json toJson(const AlignmentVertical& value)
	{
		switch (value) {
		case AlignmentVertical::TOP: return nlohmann::json("TOP");
		case AlignmentVertical::CENTER: return nlohmann::json("CENTER");
		case AlignmentVertical::BOTTOM: return nlohmann::json("BOTTOM");
		}
		return nlohmann::json();
	}
}