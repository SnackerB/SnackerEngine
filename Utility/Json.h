#pragma once

#include "External\nlohmann_json\json.hpp"

#include <typeinfo>
#include <optional>
#include <fstream>
#include <set>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Tries to load a JSON file from the given filepath. If the file is not found or the JSON file could not
	/// be parsed, a runtime exception is thrown and needs to be catched
	nlohmann::json loadJSON(const std::string& filePath);
	//--------------------------------------------------------------------------------------------------
	/// Returns true if the given JSON is of the template type
	template<typename T> bool isOfType(const nlohmann::json& json);
	//--------------------------------------------------------------------------------------------------
	/// Returns an instance of the given type, parsed from the JSON. Is guaranteed to not throw
	/// an exception if isOfType<T>(json) returned true.
	template<typename T> T parseJSON(const nlohmann::json& json);
	//--------------------------------------------------------------------------------------------------
	/// Returns an instance of the given type, parsed from the JSON or read from the data (if json has
	/// a string "name" as value, data["name"] is returned). If nothing could be parsed, an empty
	/// optional will be returned.
	template<typename T> std::optional<T> parseJsonOrReadFromData(const nlohmann::json& json, const nlohmann::json* data = nullptr)
	{
		if (isOfType<T>(json)) return parseJSON<T>(json);
		if (json.is_string() && data && data->contains(json) && isOfType<T>((*data)[json])) return parseJSON<T>((*data)[json]);
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	/// Parses a value from the given JSON file or reads it from the data (if json has
	/// a string "name" as value, data["name"] is read) and stores it in the variable "value". 
	/// If nothing could be parsed, the variable is not changed. Returns true on success.
	template<typename T> bool parseJsonOrReadFromData(T& value, const nlohmann::json& json, const nlohmann::json* data = nullptr)
	{
		if (isOfType<T>(json)) value = parseJSON<T>(json);
		else if (json.is_string() && data && data->contains(json) && isOfType<T>((*data)[json])) value = parseJSON<T>((*data)[json]);
		else return false;
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	/// Parses a value from the given JSON file or reads it from the data (if json has
	/// a string "name" as value, data["name"] is read) and stores it in the variable "value". 
	/// If nothing could be parsed, the variable is not changed. Additionally, if a set of parameter names
	/// is given and a item of the given name was found, it is erased off the set (this can be used for keeping track
	/// if all values in the file were read!). Returns true on success
	template<typename T> bool parseJsonOrReadFromData(T& value, const std::string& attributeName, const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
	{
		if (json.contains(attributeName)) {
			const nlohmann::json& attributeJson = json[attributeName];
			if (isOfType<T>(attributeJson)) value = parseJSON<T>(attributeJson);
			else if (attributeJson.is_string() && data && data->contains(attributeJson) && isOfType<T>((*data)[attributeJson])) value = parseJSON<T>((*data)[attributeJson]);
			else return false;
			if (parameterNames) parameterNames->erase(attributeName);
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	/// Parses a value from the given JSON file or reads it from the data (if json has
	/// a string "name" as value, data["name"] is read) and returns it as an optional.
	/// If nothing could be parsed, the variable is not changed. Additionally, if a set of parameter names
	/// is given and a item of the given name was found, it is erased off the set (this can be used for keeping track
	/// if all values in the file were read!)
	template<typename T> std::optional<T> parseJsonOrReadFromData(const std::string& attributeName, const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr)
	{
		if (json.contains(attributeName)) {
			const nlohmann::json& attributeJson = json[attributeName];
			if (isOfType<T>(attributeJson)) {
				if (parameterNames) parameterNames->erase(attributeName);
				return std::make_optional<T>(parseJSON<T>(attributeJson));
			}
			else if (attributeJson.is_string() && data && data->contains(attributeJson) && isOfType<T>((*data)[attributeJson])) {
				if (parameterNames) parameterNames->erase(attributeName);
				return std::make_optional<T>(parseJSON<T>((*data)[attributeJson]));
			}
			else return {};
			if (parameterNames) parameterNames->erase(attributeName);
		}
		return {};
	}
}