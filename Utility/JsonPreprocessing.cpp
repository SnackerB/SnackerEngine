#include "JsonPreprocessing.h"

namespace SnackerEngine 
{

	void preprocessJsonScaleNumericalValues(nlohmann::json& input, double scaleFactor)
	{
		if (input.is_number()) {
			if (input.is_number_integer()) input = static_cast<int>(std::floor(static_cast<double>(input) * scaleFactor));
			else input = static_cast<double>(input) * scaleFactor;
		}
		else if (input.is_array() || input.is_object()) {
			for (auto& item : input) preprocessJsonScaleNumericalValues(item, scaleFactor);
		}
	}

	void preprocessJsonReplacePlaceholders(nlohmann::json& input, const std::vector<std::reference_wrapper<const nlohmann::json>>& lookupSources)
	{
		if (input.is_array()) {
			for (auto& item : input) preprocessJsonReplacePlaceholders(item, lookupSources);
		}
		else if (input.is_object()) {
			for (auto& [key, value] : input.items()) {
				if (value.is_string()) {
					const std::string& placeholder = value.get_ref<const std::string&>();
					if (placeholder.starts_with('$') && placeholder.ends_with('$') && placeholder.size() > 2) {
						auto lookupKey = placeholder.substr(1, placeholder.size() - 2);
						for (const auto& ref : lookupSources) {
							const nlohmann::json& lookupSource = ref.get();
							if (lookupSource.is_object() && lookupSource.contains(lookupKey)) {
								value = lookupSource.at(lookupKey);
								break;
							}
						}
					}
				}
				else preprocessJsonReplacePlaceholders(value, lookupSources);
			}
		}
	}

	// Helper function that merges templateJson into input recursively. This does not overwrite keys that are alreay present in input.
	static void deepRecursiveMerge(nlohmann::json& input, const nlohmann::json& templateObject) {
		if (!input.is_object() || !templateObject.is_object()) {
			return;
		}
		for (auto& [templateKey, templateValue] : templateObject.items()) {
			if (!input.contains(templateKey)) {
				input[templateKey] = templateValue;
			}
			else {
				auto& inputValue = input.at(templateKey);
				if (templateValue.is_object() && inputValue.is_object()) {
					deepRecursiveMerge(inputValue, templateValue);
				}
			}
		}
	}

	void preprocessJsonImportObjectTemplates(nlohmann::json& input, const std::vector<std::reference_wrapper<const nlohmann::json>>& lookupSources)
	{
		if (input.is_array()) {
			for (auto& item : input) preprocessJsonImportObjectTemplates(item, lookupSources);
		}
		else if (input.is_object()) {
			if (input.contains("objectTemplate")) {
				auto& value = input.at("objectTemplate");
				if (value.is_string()) {
					const std::string& placeholder = value.get_ref<const std::string&>();
					if (placeholder.starts_with('$') && placeholder.ends_with('$') && placeholder.size() > 2) {
						auto templateName = placeholder.substr(1, placeholder.size() - 2);
						for (const auto& ref : lookupSources) {
							const nlohmann::json& lookupSource = ref.get();
							if (lookupSource.is_object() && lookupSource.contains(templateName)) {
								auto& templateObject = lookupSource.at(templateName);
								if (templateObject.is_object()) {
									// We found the template. Now, import all elements from the template
									deepRecursiveMerge(input, templateObject);
									// Delete the "objectTemplate" name
									input.erase("objectTemplate");
								}	
								break;
							}
						}
					}
				}
			}
			for (auto& item : input) preprocessJsonImportObjectTemplates(item, lookupSources);
		}
	}
}
