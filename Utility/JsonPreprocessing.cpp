#include "JsonPreprocessing.h"
#include "SnackerEngine\Core\Log.h"

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

	std::set<std::string> preprocessJsonReplacePlaceholders(nlohmann::json& input, const std::vector<std::reference_wrapper<const nlohmann::json>>& lookupSources, bool logUnreadPlaceholders)
	{
		std::set<std::string> unreadPlaceholders{};
		if (input.is_array()) {
			for (auto& item : input) {
				std::set<std::string> tempUnreadPlaceholders = preprocessJsonReplacePlaceholders(item, lookupSources, logUnreadPlaceholders);
				if (logUnreadPlaceholders) unreadPlaceholders.merge(tempUnreadPlaceholders);
			}
		}
		else if (input.is_object()) {
			for (auto& [key, value] : input.items()) {
				if (value.is_string()) {
					const std::string& placeholder = value.get_ref<const std::string&>();
					if (placeholder.starts_with('$') && placeholder.ends_with('$') && placeholder.size() > 2) {
						auto lookupKey = placeholder.substr(1, placeholder.size() - 2);
						bool foundPlaceholder = false;
						for (const auto& ref : lookupSources) {
							const nlohmann::json& lookupSource = ref.get();
							if (lookupSource.is_object() && lookupSource.contains(lookupKey)) {
								value = lookupSource.at(lookupKey);
								foundPlaceholder = true;
								break;
							}
						}
						if (!foundPlaceholder && logUnreadPlaceholders) unreadPlaceholders.insert(lookupKey);
					}
				}
				else preprocessJsonReplacePlaceholders(value, lookupSources);
			}
		}
		return unreadPlaceholders;
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

	std::set<std::string> preprocessJsonImportObjectTemplates(nlohmann::json& input, const std::vector<std::reference_wrapper<const nlohmann::json>>& lookupSources, bool logUnreadTemplates)
	{
		std::set<std::string> unreadTemplates{};
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
						bool foundTemplate = false;
						for (const auto& ref : lookupSources) {
							const nlohmann::json& lookupSource = ref.get();
							if (lookupSource.is_object() && lookupSource.contains(templateName)) {
								auto& templateObject = lookupSource.at(templateName);
								if (templateObject.is_object()) {
									// We found the template. Now, import all elements from the template
									deepRecursiveMerge(input, templateObject);
									// Delete the "objectTemplate" name
									input.erase("objectTemplate");
									foundTemplate = true;
									break;
								}	
							}
						}
						if (!foundTemplate && logUnreadTemplates) {
							unreadTemplates.insert(std::string(templateName));
						}
					}
				}
			}
			for (auto& item : input) preprocessJsonImportObjectTemplates(item, lookupSources);
		}
		return unreadTemplates;
	}
}
