#pragma once

#include "External/nlohmann_json/json.hpp"

#include <typeinfo>
#include <optional>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

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
	/// Returns an instance of the given type, parsed from the JSON. If nothing could be parsed, an empty
	/// optional will be returned.
	template<typename T> std::optional<T> parseJson(const nlohmann::json& json)
	{
		if (isOfType(json, JsonTag<T>{})) return parseJSON(json, JsonTag<T>{});
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	/// Parses a value from the given JSON file and stores it in the variable "value". 
	/// If nothing could be parsed, the variable is not changed. Returns true on success.
	template<typename T> bool parseJson(T& value, const nlohmann::json& json)
	{
		if (isOfType(json, JsonTag<T>{})) {
			value = parseJSON(json, JsonTag<T>{});
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	/// Struct for keeping track of parameterNames, such that we can print a warning if not all parameters were parsed!
	struct ParameterNameSet {
		std::unordered_set<std::string> parameterNames;
		std::unordered_map<std::string, ParameterNameSet> nestedParameterNameSets;
		ParameterNameSet* getNestedParameterNameSet(const std::string& name) {
			auto it = nestedParameterNameSets.find(name);
			return it == nestedParameterNameSets.end() ? nullptr : &(it->second);
		}
	};
	//--------------------------------------------------------------------------------------------------
	/// Parses a value from the given JSON file and stores it in the variable "value". 
	/// If nothing could be parsed, the variable is not changed. Additionally, if a set of parameter names
	/// is given and a item of the given name was found, it is erased off the set (this can be used for keeping track
	/// if all values in the file were read!). Returns true on success
	template<typename T> bool parseJson(T& value, const std::string& attributeName, const nlohmann::json& json, ParameterNameSet* parameterNameSet = nullptr)
	{
		if (json.contains(attributeName)) {
			const nlohmann::json& attributeJson = json[attributeName];
			if (isOfType(attributeJson, JsonTag<T>{})) value = parseJSON(attributeJson, JsonTag<T>{});
			else return false;
			if (parameterNameSet) parameterNameSet->parameterNames.erase(attributeName);
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	/// Parses a value from the given JSON file and returns it as an optional.
	/// If nothing could be parsed, the variable is not changed. Additionally, if a set of parameter names
	/// is given and a item of the given name was found, it is erased off the set (this can be used for keeping track
	/// if all values in the file were read!)
	template<typename T> std::optional<T> parseJson(const std::string& attributeName, const nlohmann::json& json, ParameterNameSet* parameterNameSet = nullptr)
	{
		if (json.contains(attributeName)) {
			const nlohmann::json& attributeJson = json[attributeName];
			if (isOfType(attributeJson, JsonTag<T>{})) {
				if (parameterNameSet) parameterNameSet->parameterNames.erase(attributeName);
				return std::make_optional<T>(parseJSON(attributeJson, JsonTag<T>{}));
			}
			else return {};
		}
		return {};
	}
}