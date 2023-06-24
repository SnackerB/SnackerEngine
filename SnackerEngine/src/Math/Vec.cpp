#include "Math/Vec.h"
#include "Utility/Json.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Vec2i>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 2
			&& json[0].is_number_integer() && json[1].is_number_integer();
	}
	template<> bool isOfType<Vec3i>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 3
			&& json[0].is_number_integer() && json[1].is_number_integer() && json[2].is_number_integer();
	}
	template<> bool isOfType<Vec4i>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 4
			&& json[0].is_number_integer() && json[1].is_number_integer() && json[2].is_number_integer() && json[3].is_number_integer();
	}
	template<> bool isOfType<Vec2f>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 2
			&& json[0].is_number_float() && json[1].is_number_float();
	}
	template<> bool isOfType<Vec3f>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 3
			&& json[0].is_number_float() && json[1].is_number_float() && json[2].is_number_float();
	}
	template<> bool isOfType<Vec4f>(const nlohmann::json& json)
	{
		return json.is_array() && json.size() == 4
			&& json[0].is_number_float() && json[1].is_number_float() && json[2].is_number_float() && json[3].is_number_float();
	}
	//------------------------------------------------------------------------------------------------------
	template<> Vec2i parseJSON(const nlohmann::json& json)
	{
		return Vec2i(int(json[0]), int(json[1]));
	}
	template<> Vec3i parseJSON(const nlohmann::json& json)
	{
		return Vec3i(int(json[0]), int(json[1]), int(json[2]));
	}
	template<> Vec4i parseJSON(const nlohmann::json& json)
	{
		return Vec4i(int(json[0]), int(json[1]), int(json[2]), int(json[3]));
	}
	template<> Vec2f parseJSON(const nlohmann::json& json)
	{
		return Vec2f(float(json[0]), float(json[1]));
	}
	template<> Vec3f parseJSON(const nlohmann::json& json)
	{
		return Vec3f(float(json[0]), float(json[1]), float(json[2]));
	}
	template<> Vec4f parseJSON(const nlohmann::json& json)
	{
		return Vec4f(float(json[0]), float(json[1]), float(json[2]), float(json[3]));
	}
	//------------------------------------------------------------------------------------------------------
}