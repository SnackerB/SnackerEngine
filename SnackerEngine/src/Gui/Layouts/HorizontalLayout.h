#pragma once

#include "Gui/GuiLayout.h"
#include "Utility/Alignment.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiHorizontalLayout : public GuiLayout
	{
	public:
		/// Enum that controls the mode of this layout
		enum class HorizontalLayoutMode
		{
			CHILD_HEIGHT_RANGE = 0,				/// The height of child elements is set according to their min, max and preferred height
			CHILD_HEIGHT_TO_LAYOUT_HEIGHT = 1,	/// The height of child elements will be set to the layouts height if possible
		};
	public:
		/// Static default Attributes
		static unsigned defaultVerticalBorder;
	private:
		/// The mode of this layout
		HorizontalLayoutMode horizontalLayoutMode = HorizontalLayoutMode::CHILD_HEIGHT_RANGE;
		/// The default alignment
		AlignmentVertical defaultAlignmentVertical = AlignmentVertical::TOP;
		/// Vector storing the alignments of all children
		std::vector<AlignmentVertical> alignmentsVertical{};
		/// border between the top/bottom and the children
		unsigned verticalBorder = defaultVerticalBorder;
		/// Helper function that computes the position of a child
		int computeChildPositionY(int childHeight, AlignmentVertical alignmentVertical) const;
		/// Helper function that gets called when a child is registered
		void onRegisterChild(AlignmentVertical alignmentVertical);
		/// Computes the minHeight, preferredHeight and m maxHeight of this layout from the size hints of its children
		void computeHeightHintsFromChildren();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_HORIZONTAL_LAYOUT";
		/// Constructor
		GuiHorizontalLayout(HorizontalLayoutMode horizontalLayoutMode = HorizontalLayoutMode::CHILD_HEIGHT_RANGE)
			: horizontalLayoutMode(horizontalLayoutMode) {}
		/// Constructor from JSON
		GuiHorizontalLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiHorizontalLayout() {};
		/// Copy constructor and assignment operator
		GuiHorizontalLayout(const GuiHorizontalLayout& other) noexcept;
		GuiHorizontalLayout& operator=(const GuiHorizontalLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiHorizontalLayout(GuiHorizontalLayout&& other) noexcept;
		GuiHorizontalLayout& operator=(GuiHorizontalLayout&& other) noexcept;
		/// Adds a child to this guiElement. Returns true on success
		virtual bool registerChild(GuiElement& guiElement, AlignmentVertical alignment);
		/// Registers a child, using the default alignment
		virtual bool registerChild(GuiElement& guiElement) override;
		/// Adds a child to this guiElement, with options given in JSON
		virtual bool registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames) override;
		/// Sets the mode of this layout
		void setHorizontalLayoutMode(HorizontalLayoutMode horizontalLayoutMode) { if (this->horizontalLayoutMode != horizontalLayoutMode) { this->horizontalLayoutMode = horizontalLayoutMode; registerEnforceLayoutDown(); } }
		/// Sets the default alignment
		void setDefaultAlignmentVertical(AlignmentVertical defaultAlignmentVertical) { this->defaultAlignmentVertical = defaultAlignmentVertical; }
		/// Sets the alignment of the given element, if it is a child of this layout
		void setAlignmentVertical(GuiID childID, AlignmentVertical alignmentVertical);
		/// Getters
		HorizontalLayoutMode getMode() const { return horizontalLayoutMode; }
		AlignmentVertical getDefaultAlignmentVertical() const { return defaultAlignmentVertical; }
		std::optional<AlignmentVertical> getAlignmentVertical(GuiID childID);
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