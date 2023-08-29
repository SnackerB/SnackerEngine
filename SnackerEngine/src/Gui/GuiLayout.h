#pragma once

#include "Gui/GuiElement.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiLayout : public GuiElement
	{
	public:
		/// Default constructor
		GuiLayout()
			: GuiElement(Vec2i(), Vec2i(), ResizeMode::SAME_AS_PARENT) {}
		/// Constructor from JSON
		GuiLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
			: GuiElement(json, data, parameterNames) 
		{
			setResizeMode(ResizeMode::SAME_AS_PARENT);
		}
		/// Removes the given child from this GuiElement object
		std::optional<unsigned> removeChild(GuiID guiElement)
		{
			std::optional<unsigned> result = GuiElement::removeChild(guiElement);
			if (result.has_value()) registerEnforceLayoutDown();
			return result;
		}
	};
	//--------------------------------------------------------------------------------------------------
}