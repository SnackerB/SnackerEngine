#pragma once

#include "Utility/Json.h"

namespace SnackerEngine
{

	// Scales all numerical values stored in this JSON file by the given scale factor.
	// Will also recursively go through nested JSON objects.
	void preprocessJsonScaleNumericalValues(nlohmann::json& input, double scaleFactor);

	/// Looks through the json file and tries to replace all string values of the form $STRING$ with values that are stored
	/// with the key $STRING$ in the lookup sources. If multiple of the lookup sources contain STRING, the first 
	/// found value is used.
	void preprocessJsonReplacePlaceholders(nlohmann::json& input, const std::vector<std::reference_wrapper<const nlohmann::json>>& lookupSources);

	/// Looks through the json file to locate json objects containing the key-value-pair "objectTemplate" : "$STRING$".
	/// If found, all lookup sources are scanned for the key "STRING", having a json object as its value. All elements
	/// from this object are then imported into the original object from the input file. If the input file already contains
	/// a key for a given imported key, the original key is used instead ("overriding" the template). If multiple of the 
	/// lookup sources contain $STRING$, the first found is used.
	void preprocessJsonImportObjectTemplates(nlohmann::json& input, const std::vector<std::reference_wrapper<const nlohmann::json>>& lookupSources);

}