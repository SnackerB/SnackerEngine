#include "Vec.h"
#include "Utility/Json.h"
#include "Utility/Conversions.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Vec2i>(const nlohmann::json& json, JsonTag<Vec2i> tag)
	{
		return json.is_array() && json.size() == 2
			&& json[0].is_number_integer() && json[1].is_number_integer();
	}
	template<> bool isOfType<Vec3i>(const nlohmann::json& json, JsonTag<Vec3i> tag)
	{
		return json.is_array() && json.size() == 3
			&& json[0].is_number_integer() && json[1].is_number_integer() && json[2].is_number_integer();
	}
	template<> bool isOfType<Vec4i>(const nlohmann::json& json, JsonTag<Vec4i> tag)
	{
		return json.is_array() && json.size() == 4
			&& json[0].is_number_integer() && json[1].is_number_integer() && json[2].is_number_integer() && json[3].is_number_integer();
	}
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Vec2f>(const nlohmann::json& json, JsonTag<Vec2f> tag)
	{
		return json.is_array() && json.size() == 2
			&& json[0].is_number_float() && json[1].is_number_float();
	}
	template<> bool isOfType<Vec3f>(const nlohmann::json& json, JsonTag<Vec3f> tag)
	{
		return json.is_array() && json.size() == 3
			&& json[0].is_number_float() && json[1].is_number_float() && json[2].is_number_float();
	}
	template<> bool isOfType<Vec4f>(const nlohmann::json& json, JsonTag<Vec4f> tag)
	{
		return json.is_array() && json.size() == 4
			&& json[0].is_number_float() && json[1].is_number_float() && json[2].is_number_float() && json[3].is_number_float();
	}
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Vec2d>(const nlohmann::json& json, JsonTag<Vec2d> tag)
	{
		return json.is_array() && json.size() == 2
			&& json[0].is_number_float() && json[1].is_number_float();
	}
	template<> bool isOfType<Vec3d>(const nlohmann::json& json, JsonTag<Vec3d> tag)
	{
		return json.is_array() && json.size() == 3
			&& json[0].is_number_float() && json[1].is_number_float() && json[2].is_number_float();
	}
	template<> bool isOfType<Vec4d>(const nlohmann::json& json, JsonTag<Vec4d> tag)
	{
		return json.is_array() && json.size() == 4
			&& json[0].is_number_float() && json[1].is_number_float() && json[2].is_number_float() && json[3].is_number_float();
	}
	//------------------------------------------------------------------------------------------------------
	template<> Vec2i parseJSON(const nlohmann::json& json, JsonTag<Vec2i> tag)
	{
		return Vec2i(int(json[0]), int(json[1]));
	}
	template<> Vec3i parseJSON(const nlohmann::json& json, JsonTag<Vec3i> tag)
	{
		return Vec3i(int(json[0]), int(json[1]), int(json[2]));
	}
	template<> Vec4i parseJSON(const nlohmann::json& json, JsonTag<Vec4i> tag)
	{
		return Vec4i(int(json[0]), int(json[1]), int(json[2]), int(json[3]));
	}
	//------------------------------------------------------------------------------------------------------
	template<> Vec2f parseJSON(const nlohmann::json& json, JsonTag<Vec2f> tag)
	{
		return Vec2f(float(json[0]), float(json[1]));
	}
	template<> Vec3f parseJSON(const nlohmann::json& json, JsonTag<Vec3f> tag)
	{
		return Vec3f(float(json[0]), float(json[1]), float(json[2]));
	}
	template<> Vec4f parseJSON(const nlohmann::json& json, JsonTag<Vec4f> tag)
	{
		return Vec4f(float(json[0]), float(json[1]), float(json[2]), float(json[3]));
	}
	//------------------------------------------------------------------------------------------------------
	template<> Vec2d parseJSON(const nlohmann::json& json, JsonTag<Vec2d> tag)
	{
		return Vec2d(int(json[0]), int(json[1]));
	}
	template<> Vec3d parseJSON(const nlohmann::json& json, JsonTag<Vec3d> tag)
	{
		return Vec3d(int(json[0]), int(json[1]), int(json[2]));
	}
	template<> Vec4d parseJSON(const nlohmann::json& json, JsonTag<Vec4d> tag)
	{
		return Vec4d(int(json[0]), int(json[1]), int(json[2]), int(json[3]));
	}
	//------------------------------------------------------------------------------------------------------
	template<> Vec2i interpolate(const Vec2i& a, const Vec2i& b, double percentage)
	{
		return Vec2i(a.x + static_cast<int>(static_cast<double>(b.x - a.x) * percentage), a.y + static_cast<int>(static_cast<double>(b.y - a.y) * percentage));
	}
	template<> Vec3i interpolate(const Vec3i& a, const Vec3i& b, double percentage)
	{
		return Vec3i(a.x + static_cast<int>(static_cast<double>(b.x - a.x) * percentage), a.y + static_cast<int>(static_cast<double>(b.y - a.y) * percentage), a.z + static_cast<int>(static_cast<double>(b.z - a.z) * percentage));
	}
	template<> Vec4i interpolate(const Vec4i& a, const Vec4i& b, double percentage)
	{
		return Vec4i(a.x + static_cast<int>(static_cast<double>(b.x - a.x) * percentage), a.y + static_cast<int>(static_cast<double>(b.y - a.y) * percentage), a.z + static_cast<int>(static_cast<double>(b.z - a.z) * percentage), a.w + static_cast<int>(static_cast<double>(b.w - a.w) * percentage));
	}
	//------------------------------------------------------------------------------------------------------
	template<> Vec2f interpolate(const Vec2f& a, const Vec2f& b, double percentage)
	{
		return Vec2f(a.x + static_cast<float>(static_cast<double>(b.x - a.x) * percentage), a.y + static_cast<float>(static_cast<double>(b.y - a.y) * percentage));
	}
	template<> Vec3f interpolate(const Vec3f& a, const Vec3f& b, double percentage)
	{
		return Vec3f(a.x + static_cast<float>(static_cast<double>(b.x - a.x) * percentage), a.y + static_cast<float>(static_cast<double>(b.y - a.y) * percentage), a.z + static_cast<int>(static_cast<double>(b.z - a.z) * percentage));
	}
	template<> Vec4f interpolate(const Vec4f& a, const Vec4f& b, double percentage)
	{
		return Vec4f(a.x + static_cast<float>(static_cast<double>(b.x - a.x) * percentage), a.y + static_cast<float>(static_cast<double>(b.y - a.y) * percentage), a.z + static_cast<int>(static_cast<double>(b.z - a.z) * percentage), a.w + static_cast<int>(static_cast<double>(b.w - a.w) * percentage));
	}
	//------------------------------------------------------------------------------------------------------
	template<> Vec2d interpolate(const Vec2d& a, const Vec2d& b, double percentage)
	{
		return a + (b - a) * percentage;
	}
	template<> Vec3d interpolate(const Vec3d& a, const Vec3d& b, double percentage)
	{
		return a + (b - a) * percentage;
	}
	template<> Vec4d interpolate(const Vec4d& a, const Vec4d& b, double percentage)
	{
		return a + (b - a) * percentage;
	}
	//------------------------------------------------------------------------------------------------------
}