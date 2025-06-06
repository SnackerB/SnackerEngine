#pragma once

#include "Gui/Layouts/VerticalLayout.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiVerticalWeightedLayout : public GuiVerticalLayout
	{
	private:
		/// Total weight
		double totalWeight = 0.0;
		/// Vector of stored weights
		std::vector<double> weights = {};
		/// Vector of stored width percentages
		std::vector<double> percentages = {};
		/// Border between elements
		int verticalBorder = 0;
		/// Border between elements and the top/bottom border of the layout
		int outerVerticalBorder = 0;
		/// Vertical alignment
		AlignmentVertical alignmentVertical = AlignmentVertical::TOP;
		/// The height of the resize area (in pixels)
		int resizeAreaHeight = defaultBorderNormal;
		/// The offset of the mouse to the border (used for resizing the layout)
		int mouseOffset = 0;
		/// The border that is currently being resized. Counting from the top, starting at 0.
		int resizeBorder = 0;
		/// If this is set to true, moving the border with the mouse is possible.
		bool allowMoveBorders = false;
		/// This is set to true if we are currently resizing a border
		bool isResizing = false;
		/// Helper function that stops the resizing process
		void stopResizing();
		/// Helper function that computes percentages from weights
		void computePercentagesFromWeights();
		/// Helper function that computes weights from percentages
		void computeWeightsFromPercentages();
		/// Helper function that get called when a child is registered
		void onRegisterChild(double weight = 1.0);
		/// Helper function that computes the minHeight of the layout from its childElements.
		void computeHeightHintsFromChildren();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_VERTICAL_WEIGHTED_LAYOUT";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiVerticalWeightedLayout() = default;
		/// Constructor from JSON
		GuiVerticalWeightedLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiVerticalWeightedLayout() {};
		/// Copy constructor and assignment operator
		GuiVerticalWeightedLayout(const GuiVerticalWeightedLayout& other) noexcept = default;
		GuiVerticalWeightedLayout& operator=(const GuiVerticalWeightedLayout& other) = default;
		/// Move constructor and assignment operator
		GuiVerticalWeightedLayout(GuiVerticalWeightedLayout&& other) noexcept = default;
		GuiVerticalWeightedLayout& operator=(GuiVerticalWeightedLayout&& other) noexcept = default;
		/// Adds a child to this guiElement. Returns true on success
		virtual bool registerChild(GuiElement& guiElement, double weight);
		/// Registers a child, using weight = 1.0.
		virtual bool registerChild(GuiElement& guiElement) override;
		/// Adds a child to this guiElement, with options given in JSON
		virtual bool registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames) override;
		/// Getters
		double getTotalWeight() const { return totalWeight; }
		std::optional<double> getWeight(GuiID childID) const;
		std::optional<double> getPercentage(GuiID childID) const;
		unsigned getVerticalBorder() const { return verticalBorder; }
		unsigned getOuterVerticalBorder() const { return outerVerticalBorder; }
		AlignmentVertical getAlignmentVertical() const { return alignmentVertical; }
		unsigned getResizeAreaHeight() const { return resizeAreaHeight; }
		bool isAllowMoveBorders() const { return allowMoveBorders; }
		/// Setters
		void setWeight(GuiID childID, double weight);
		void setPercentage(GuiID childID, double percentage);
		void setVerticalBorder(int horizontalBorder);
		void setOuterVerticalBorder(int outerHorizontalBorder);
		void setAlignmentVertical(AlignmentVertical alignmentVertical);
		void setResizeAreaHeight(unsigned resizeAreaHeight) { this->resizeAreaHeight = resizeAreaHeight; }
		void setAllowMoveBorders(bool allowMoveBorders);
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateWeight(GuiID childID, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animatePercentage(GuiID childID, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateVerticalBorder(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateOuterVerticalBorder(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateResizeAreaHeight(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
	protected:
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Removes the given child from this GuiElement object
		virtual std::optional<unsigned> removeChild(GuiID guiElement) override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;

		//==============================================================================================
		// Collisions
		//==============================================================================================

		/// Helper function that returns the border and offset, if a collision occured.
		/// position: position of mouse relative to the parent elements (0, 0) coordinate
		std::optional<std::pair<unsigned int, int>> getCollidingBorderAndOffset(const Vec2i& offset) const;
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;

		//==============================================================================================
		// Events
		//==============================================================================================

		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
	};
	//--------------------------------------------------------------------------------------------------
}