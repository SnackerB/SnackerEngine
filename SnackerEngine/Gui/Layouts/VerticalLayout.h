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
			CHILD_WIDTH_RANGE,				/// The width of child elements is set according to their min, max and preferred width
			CHILD_WIDTH_TO_LAYOUT_WIDTH,	/// The width of child elements will be set to the layouts width if possible
			LARGEST_PREFERRED_WIDTH,		/// The width of child elements will be set to the largest child preferredWidth, if possible
		};
	private:
		/// The mode of this layout
		VerticalLayoutMode verticalLayoutMode = VerticalLayoutMode::CHILD_WIDTH_RANGE;
		/// The default alignment
		AlignmentHorizontal defaultAlignmentHorizontal = AlignmentHorizontal::LEFT;
		/// Vector storing the alignments of all children
		std::vector<AlignmentHorizontal> alignmentsHorizontal{};
		/// border between the left/bottom and the children
		unsigned horizontalBorder = 0;
		/// If this is set to true, the layouts minWidth and preferredWidth are set according
		/// to child elements
		bool shrinkWidthToChildren = false;
		/// Helper function that computes the position of a child
		int computeChildPositionX(int childWidth, AlignmentHorizontal alignmentHorizontal) const;
		/// Helper function that gets called when a child is registered
		void onRegisterChild(AlignmentHorizontal alignmentHorizontal);
		/// Computes the minWidth, preferredWidth and maxWidth of this layout from the size hints of its children
		void computeWidthHintsFromChildren();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_VERTICAL_LAYOUT";
		/// Constructor
		GuiVerticalLayout(VerticalLayoutMode verticalLayoutMode = VerticalLayoutMode::CHILD_WIDTH_RANGE)
			: verticalLayoutMode(verticalLayoutMode) {}
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
		/// Setters
		void setVerticalLayoutMode(VerticalLayoutMode verticalLayoutMode) { if (this->verticalLayoutMode != verticalLayoutMode) { this->verticalLayoutMode = verticalLayoutMode; registerEnforceLayoutDown(); } }
		void setHorizontalBorder(unsigned horizontalBorder) { if (this->horizontalBorder != horizontalBorder) { this->horizontalBorder = horizontalBorder; registerEnforceLayoutDown(); } }
		void setShrinkWidthToChildren(bool shrinkWidthToChildren);
		/// Sets the default alignment
		void setDefaultAlignmentHorizontal(AlignmentHorizontal defaultAlignmentHorizontal) { this->defaultAlignmentHorizontal = defaultAlignmentHorizontal; }
		/// Sets the alignment of the given element, if it is a child of this layout
		void setAlignmentHorizontal(GuiID childID, AlignmentHorizontal alignmentHorizontal);
		/// Getters
		VerticalLayoutMode getMode() const { return verticalLayoutMode; }
		unsigned getHorizontalBorder() const { return horizontalBorder; }
		bool isShrinkWidthToChildren() const { return shrinkWidthToChildren; }
		AlignmentHorizontal getDefaultAlignmentHorizontal() const { return defaultAlignmentHorizontal; }
		std::optional<AlignmentHorizontal> getAlignmentHorizontal(GuiID childID);
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateHorizontalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
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