#pragma once

#include "Gui/GuiLayout.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiPositioningLayout : public GuiLayout
	{
	public:
		/// Layout mode, decides where the child is positioned
		enum class Mode
		{
			CENTER,
			LEFT,
			TOP_LEFT,
			TOP,
			TOP_RIGHT,
			RIGHT,
			BOTTOM_RIGHT,
			BOTTOM,
			BOTTOM_LEFT,
		};
	private:
		Mode mode;
		/// Helper function that computes the sizeHints of this layout based on the sizeHints of its children
		void computeSizeHintsFromChildren();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_POSITIONING_LAYOUT";
		/// Default constructor
		GuiPositioningLayout(Mode mode = Mode::CENTER);
		/// Constructor from JSON
		GuiPositioningLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiPositioningLayout() {};
		/// Copy constructor and assignment operator
		GuiPositioningLayout(const GuiPositioningLayout& other) noexcept;
		GuiPositioningLayout& operator=(const GuiPositioningLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiPositioningLayout(GuiPositioningLayout&& other) noexcept;
		GuiPositioningLayout& operator=(GuiPositioningLayout&& other) noexcept;
	protected:
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;
	};
	//--------------------------------------------------------------------------------------------------
}