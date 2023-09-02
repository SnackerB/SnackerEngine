#pragma once

#include "Gui/GuiLayout.h"
#include "Utility/Alignment.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiVerticalLayout : public GuiLayout
	{
	public:
		/// Enum that controls the mode of this layout
		enum class VerticalLayoutMode
		{
			VARIABLE_WIDTH = 0,		/// This layout will set its preferred, min and max width according to child elements
			FORCED_WIDTH = 1,		/// This layout will not change its width, the width of child elements is set according
									/// to their min, max and preferred width
			FORCE_CHILD_WIDTH = 2,	/// This layout will not change its width, the width of child elements will be set to the layouts
									/// width if possible
		};
	public:
		/// Static default Attributes
		static unsigned defaultHorizontalBorder;
	private:
		/// The mode of this layout
		VerticalLayoutMode verticalLayoutMode = VerticalLayoutMode::VARIABLE_WIDTH;
		/// The default alignment
		AlignmentHorizontal defaultAlignmentHorizontal = AlignmentHorizontal::LEFT;
		/// Vector storing the alignments of all children
		std::vector<AlignmentHorizontal> alignmentsHorizontal{};
		/// border between the left/right and the children
		unsigned horizontalBorder = defaultHorizontalBorder;
		/// Helper function that computes the position of a child
		int computeChildPositionX(int childWidth, AlignmentHorizontal alignmentHorizontal) const;
		/// Helper function that gets called when a child is registered
		void onRegisterChild(AlignmentHorizontal alignment);
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_VERTICAL_LAYOUT";
		/// Constructor
		GuiVerticalLayout() {};
		/// Constructor from JSON
		GuiVerticalLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiVerticalLayout() {};
		/// Copy constructor and assignment operator
		GuiVerticalLayout(const GuiVerticalLayout& other) noexcept;
		GuiVerticalLayout& operator=(const GuiVerticalLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiVerticalLayout(GuiVerticalLayout&& other) noexcept;
		GuiVerticalLayout& operator=(GuiVerticalLayout&& other) noexcept;
		/// Adds a child to this guiElement. Returns true on success
		virtual bool registerChild(GuiElement& guiElement, AlignmentHorizontal alignment);
		/// Registers a child, using the default alignment
		virtual bool registerChild(GuiElement& guiElement) override;
		/// Adds a child to this guiElement, with options given in JSON
		virtual bool registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames) override;
		/// Sets the mode of this layout
		void setVerticalLayoutMode(VerticalLayoutMode verticalLayoutMode) { if (this->verticalLayoutMode != verticalLayoutMode) { this->verticalLayoutMode = verticalLayoutMode; registerEnforceLayoutDown(); } }
		/// Sets the default alignment
		void setDefaultAlignmentHorizontal(AlignmentHorizontal defaultAlignmentHorizontal) { this->defaultAlignmentHorizontal = defaultAlignmentHorizontal; }
		/// Sets the alignment of the given element, if it is a child of this layout
		void setAlignmentHorizontal(GuiID childID, AlignmentHorizontal alignment);
		/// Getters
		VerticalLayoutMode getMode() const { return verticalLayoutMode; }
		AlignmentHorizontal getDefaultAlignmentHorizontal() const { return defaultAlignmentHorizontal; }
		std::optional<AlignmentHorizontal> getAlignmentHorizontal(GuiID childID);
	protected:
		/// Removes the given child from this GuiElement object
		virtual std::optional<unsigned> removeChild(GuiID guiElement) override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May register children and/or parent for
		/// enforcing layouts as well
		virtual void enforceLayout() override;
	};
	//--------------------------------------------------------------------------------------------------
}