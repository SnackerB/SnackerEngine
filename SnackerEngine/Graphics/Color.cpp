#include "Graphics/Color.h"
#include "Utility/Json.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Color3f>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 3 &&
			json[0].is_number() && json[1].is_number() && json[2].is_number();
	}
	template<> bool isOfType<Color4f>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 4 &&
			json[0].is_number() && json[1].is_number() && json[2].is_number() && json[3].is_number();
	}
	//------------------------------------------------------------------------------------------------------
	template<> Color3f parseJSON(const nlohmann::json& json)
	{
		Color3f result;
		if (json[0].is_number_integer()) result.r = static_cast<float>(int(json[0])) / 255.0f;
		else result.r = json[0];
		if (json[1].is_number_integer()) result.g = static_cast<float>(int(json[1])) / 255.0f;
		else result.g = json[1];
		if (json[2].is_number_integer()) result.b = static_cast<float>(int(json[2])) / 255.0f;
		else result.b = json[2];
		return result;
	}
	template<> Color4f parseJSON(const nlohmann::json& json)
	{
		Color4f result;
		if (json[0].is_number_integer()) result.r = static_cast<float>(int(json[0])) / 255.0f;
		else result.r = json[0];
		if (json[1].is_number_integer()) result.g = static_cast<float>(int(json[1])) / 255.0f;
		else result.g = json[1];
		if (json[2].is_number_integer()) result.b = static_cast<float>(int(json[2])) / 255.0f;
		else result.b = json[2];
		if (json[3].is_number_integer()) result.alpha = static_cast<float>(int(json[3])) / 255.0f;
		else result.alpha = json[3];
		return result;
	}
	//------------------------------------------------------------------------------------------------------
}