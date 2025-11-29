#pragma once

#include "External/nlohmann_json/json.hpp"

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
	/// Tries to save a JSON file from the given filepath. If an error occurs, a runtime exception is thrown
	/// and needs to be catched
	void saveJSON(const nlohmann::json& json, const std::string& filePath);
	//--------------------------------------------------------------------------------------------------
	// Struct used only for implementation, making the template specialization easier.
	// With the tag one can simply use function overloading
	template<class T>
	struct JsonTag {};
	//--------------------------------------------------------------------------------------------------
	/// Returns true if the given JSON is of the template type
	template<typename T> bool isOfType(const nlohmann::json& json, JsonTag<T> tag);
	//--------------------------------------------------------------------------------------------------
	/// Returns an instance of the given type, parsed from the JSON. Is guaranteed to not throw
	/// an exception if isOfType<T>(json) returned true.
	template<typename T> T parseJSON(const nlohmann::json& json, JsonTag<T> tag);
	//--------------------------------------------------------------------------------------------------
	// Gives a JSON representation, that will be successfully parsed when calling parseJSON.
	template<typename T> nlohmann::json toJson(const T& value);
	//--------------------------------------------------------------------------------------------------
	/// Returns an instance of the given type, parsed from the JSON or read from the data (if json has
	/// a string "name" as value, data["name"] is returned). If nothing could be parsed, an empty
	/// optional will be returned.
	template<typename T> std::optional<T> parseJsonOrReadFromData(const nlohmann::json& json, const nlohmann::json* data = nullptr)
	{
		if (isOfType(json, JsonTag<T>{})) return parseJSON(json, JsonTag<T>{});
		if (json.is_string() && data && data->contains(json) && isOfType((*data)[json], JsonTag<T>{})) return parseJSON((*data)[json], JsonTag<T>{});
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	/// Parses a value from the given JSON file or reads it from the data (if json has
	/// a string "name" as value, data["name"] is read) and stores it in the variable "value". 
	/// If nothing could be parsed, the variable is not changed. Returns true on success.
	template<typename T> bool parseJsonOrReadFromData(T& value, const nlohmann::json& json, const nlohmann::json* data = nullptr)
	{
		if (isOfType(json, JsonTag<T>{})) value = parseJSON(json, JsonTag<T>{});
		else if (json.is_string() && data && data->contains(json) && isOfType((*data)[json], JsonTag<T>{})) value = parseJSON((*data)[json], JsonTag<T>{});
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
			if (isOfType(attributeJson, JsonTag<T>{})) value = parseJSON(attributeJson, JsonTag<T>{});
			else if (attributeJson.is_string() && data && data->contains(attributeJson) && isOfType((*data)[attributeJson], JsonTag<T>{})) value = parseJSON((*data)[attributeJson], JsonTag<T>{});
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
			if (isOfType(attributeJson, JsonTag<T>{})) {
				if (parameterNames) parameterNames->erase(attributeName);
				return std::make_optional<T>(parseJSON(attributeJson, JsonTag<T>{}));
			}
			else if (attributeJson.is_string() && data && data->contains(attributeJson) && isOfType((*data)[attributeJson], JsonTag<T>{})) {
				if (parameterNames) parameterNames->erase(attributeName);
				return std::make_optional<T>(parseJSON((*data)[attributeJson], JsonTag<T>{}));
			}
			else return {};
			if (parameterNames) parameterNames->erase(attributeName);
		}
		return {};
	}
}