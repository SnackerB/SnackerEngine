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
			CHILD_HEIGHT_RANGE,				/// The height of child elements is set according to their min, max and preferred height
			CHILD_HEIGHT_TO_LAYOUT_HEIGHT,	/// The height of child elements will be set to the layouts height if possible
			LARGEST_PREFERRED_HEIGHT,		/// The height of child elements will be set to the largest child preferredHeight, if possible
		};
	private:
		/// The mode of this layout
		HorizontalLayoutMode horizontalLayoutMode = HorizontalLayoutMode::CHILD_HEIGHT_RANGE;
		/// The default alignment
		AlignmentVertical defaultAlignmentVertical = AlignmentVertical::TOP;
		/// Vector storing the alignments of all children
		std::vector<AlignmentVertical> alignmentsVertical{};
		/// border between the top/bottom and the children
		unsigned verticalBorder = 0;
		/// If this is set to true, the layouts minHeight and preferredHeight are set according
		/// to child elements
		bool shrinkHeightToChildren = false;
		/// Helper function that computes the position of a child
		int computeChildPositionY(int childHeight, AlignmentVertical alignmentVertical) const;
		/// Helper function that gets called when a child is registered
		void onRegisterChild(AlignmentVertical alignmentVertical);
		/// Computes the minHeight, preferredHeight and maxHeight of this layout from the size hints of its children
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
		/// Setters
		void setHorizontalLayoutMode(HorizontalLayoutMode horizontalLayoutMode) { if (this->horizontalLayoutMode != horizontalLayoutMode) { this->horizontalLayoutMode = horizontalLayoutMode; registerEnforceLayoutDown(); } }
		void setVerticalBorder(unsigned verticalBorder) { if (this->verticalBorder != verticalBorder) { this->verticalBorder = verticalBorder; registerEnforceLayoutDown(); } }
		void setShrinkHeightToChildren(bool shrinkHeightToChildren);
		/// Sets the default alignment
		void setDefaultAlignmentVertical(AlignmentVertical defaultAlignmentVertical) { this->defaultAlignmentVertical = defaultAlignmentVertical; }
		/// Sets the alignment of the given element, if it is a child of this layout
		void setAlignmentVertical(GuiID childID, AlignmentVertical alignmentVertical);
		/// Getters
		HorizontalLayoutMode getMode() const { return horizontalLayoutMode; }
		unsigned getVerticalBorder() const { return verticalBorder; }
		bool isShrinkHeightToChildren() const { return shrinkHeightToChildren; }
		AlignmentVertical getDefaultAlignmentVertical() const { return defaultAlignmentVertical; }
		std::optional<AlignmentVertical> getAlignmentVertical(GuiID childID);
		//==============================================================================================
		// Animatables
		//==============================================================================================
		void animateVerticalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
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