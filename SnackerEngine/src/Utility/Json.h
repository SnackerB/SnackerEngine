#pragma once

#include "External/json.hpp"
#include "Core/Log.h"

#include <typeinfo>
#include <optional>
#include <fstream>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Tries to load a JSON file
	std::optional<nlohmann::json> loadJSON(const std::string& path, bool relativeToResourceFolder = true);
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
	/// If nothing could be parsed, the variable is not changed.
	template<typename T> void parseJsonOrReadFromData(T& value, const nlohmann::json& json, const nlohmann::json* data = nullptr)
	{
		if (isOfType<T>(json)) value = parseJSON<T>(json);
		else if (json.is_string() && data && data->contains(json) && isOfType<T>((*data)[json])) value = parseJSON<T>((*data)[json]);
	}
}