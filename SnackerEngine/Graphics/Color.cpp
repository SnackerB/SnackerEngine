#include "Graphics/Color.h"
#include "Utility/Json.h"
#include "Utility\Conversions.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType(const nlohmann::json& json, JsonTag<Color3f> tag)
	{
		return json.is_array() && json.size() == 3 &&
			json[0].is_number() && json[1].is_number() && json[2].is_number();
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<Color4f> tag)
	{
		return json.is_array() && json.size() == 4 &&
			json[0].is_number() && json[1].is_number() && json[2].is_number() && json[3].is_number();
	}
	//------------------------------------------------------------------------------------------------------
	template<> Color3f parseJSON(const nlohmann::json& json, JsonTag<Color3f> tag)
	{
		Color3f result{};
		if (json[0].is_number_integer()) result.r = static_cast<float>(int(json[0])) / 255.0f;
		else result.r = json[0];
		if (json[1].is_number_integer()) result.g = static_cast<float>(int(json[1])) / 255.0f;
		else result.g = json[1];
		if (json[2].is_number_integer()) result.b = static_cast<float>(int(json[2])) / 255.0f;
		else result.b = json[2];
		return result;
	}
	template<> Color4f parseJSON(const nlohmann::json& json, JsonTag<Color4f> tag)
	{
		Color4f result{};
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
	template<> Color3f interpolate(const Color3f& a, const Color3f& b, double percentage)
	{
		return Color3f(a.r + ((b.r - a.r) * static_cast<float>(percentage)), a.g + ((b.g - a.g) * static_cast<float>(percentage)), a.b + ((b.b - a.b) * static_cast<float>(percentage)));
	}
	template<> Color4f interpolate(const Color4f& a, const Color4f& b, double percentage)
	{
		return Color4f(a.r + ((b.r - a.r) * static_cast<float>(percentage)), a.g + ((b.g - a.g) * static_cast<float>(percentage)), a.b + ((b.b - a.b) * static_cast<float>(percentage)), a.alpha + ((b.alpha - a.alpha) * static_cast<float>(percentage)));
	}
	//------------------------------------------------------------------------------------------------------
}