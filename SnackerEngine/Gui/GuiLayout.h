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
			if (!json.contains("resizeMode") && !json.contains("position")
				&& !json.contains("positionX") && !json.contains("positionY")
				&& !json.contains("size") && !json.contains("width")
				&& !json.contains("height")) setResizeMode(ResizeMode::SAME_AS_PARENT);
		}
		/// Removes the given child from this GuiElement object
		std::optional<unsigned> removeChild(GuiID guiElement)
		{
			std::optional<unsigned> result = GuiElement::removeChild(guiElement);
			if (result.has_value()) registerEnforceLayoutDown();
			return result;
		}
	protected:
		/// Helper function that distributes remaining size between children.
		/// This assumes that the sum of the childrens minSizes is smaller than the layouts size,
		/// but the sum of the childrens preferredSizes is larger than the layouts size.
		/// The remaining size is the difference of the layouts size to the sum of the childrens minSizes.
		static std::vector<int> distributeSizeBetweenChildren(const std::vector<int>& minSizes, const std::vector<int>& preferredSizes, int remainingSize);
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;
	};
	//--------------------------------------------------------------------------------------------------
}