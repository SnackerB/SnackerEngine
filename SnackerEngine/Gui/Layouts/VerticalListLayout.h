#pragma once

#include "Gui/Layouts/VerticalLayout.h"
#include "Graphics/Color.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"
#include "Gui\Group.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiVerticalListLayout : public GuiVerticalLayout
	{
	public:
		/// Static default Attributes
		static Color4f defaultBackgroundColor;
	private:
		/// Class that is used for grouping multiple horizontal layouts together
		class VerticalLayoutGroup : public GuiGroup
		{
		public:
			/// The group type, necessary for efficient casting
			std::string getGroupType() const override { return "VERTICAL_LAYOUT_GROUP"; }
		private:
			std::vector<int> heights{};
			bool upToDate = false;
		protected:
			/// This function gets called when a new element is added to the group
			void onElementJoin(GuiElement::GuiID element, std::size_t position) override;
			/// This function gets called when a new element leaves the group
			void onElementLeave(GuiElement::GuiID element, std::size_t position) override;
		public:
			/// Constructor
			VerticalLayoutGroup(const std::string& name = "")
				: GuiGroup(name) {}
			/// If upToDate is set to false, the heights vectors is recomputed and
			/// upToDate is set to true.
			void recomputeHeights();
			/// Getters
			bool isUpToDate() const { return upToDate; }
			const std::vector<int>& getHeights() const { return heights; }
			/// Setters
			void markForRecompute() { if (upToDate) onElementJoin(-1, -1); };
		};
		friend class VerticalLayoutGroup;
		/// Vertical alignment of the layout
		AlignmentVertical alignmentVertical = AlignmentVertical::TOP;
		/// Border between different elements and between top/bottom edge of layout and elements, in pixels
		unsigned verticalBorder = defaultBorderNormal;
		/// Border between top/bottom edge of layout and elements in pixels
		unsigned outerVerticalBorder = defaultBorderNormal;
		/// Background color
		Color4f backgroundColor = defaultBackgroundColor;
		/// model matrix of the background
		Mat4f modelMatrixBackground{};
		/// Shader for drawing the background
		Shader backgroundShader = Shader("shaders/gui/simpleAlphaColor.shader");
		/// The ID of the VerticalLayoutGroup this element is in
		GuiGroupID groupID = -1;
		/// The name of the VerticalLayoutGroup this element is in
		std::string groupName = "";
		/// If this is set to true, the layouts minHeight and preferredHeight are set according
		/// to child elements
		bool shrinkHeightToChildren = false;
		/// Helper function for enforcing the layout
		static std::vector<int> computeChildHeights(const std::vector<int>& minHeights, const std::vector<int>& preferredHeights, const std::vector<int>& maxHeights, int height);
	protected:
		/// Helper function that computes the minWidth of the layout from its childElements.
		void computeHeightHintsFromChildren();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_VERTICAL_LIST_LAYOUT";
		/// Default constructor
		GuiVerticalListLayout(Color4f backgroundColor = Color4f(1.0f, 0.0f));
		/// Constructor from JSON.
		GuiVerticalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiVerticalListLayout();
		/// Copy constructor and assignment operator
		GuiVerticalListLayout(const GuiVerticalListLayout& other) noexcept;
		GuiVerticalListLayout& operator=(const GuiVerticalListLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiVerticalListLayout(GuiVerticalListLayout&& other) noexcept;
		GuiVerticalListLayout& operator=(GuiVerticalListLayout&& other) noexcept;
		/// Getters
		AlignmentVertical getAlignmentVertical() const { return alignmentVertical; }
		unsigned getVerticalBorder() const { return verticalBorder; }
		unsigned getOuterVerticalBorder() const { return outerVerticalBorder; }
		Color4f getBackgroundColor() const { return backgroundColor; }
		Shader& getBackgroundShader() { return backgroundShader; }
		std::optional<GuiGroupID> getVerticalLayoutGroupID() const { return groupID == -1 ? std::optional<GuiGroupID>() : groupID; }
		const std::string* getVerticalLayoutGroupName() const { return groupID == -1 ? nullptr : &groupName; }
		bool isShrinkHeightToChildren() const { return this->shrinkHeightToChildren; }
		/// Setters
		void setAlignmentVertical(AlignmentVertical alignmentVertical);
		void setVerticalBorder(unsigned verticalBorder);
		void setOuterVerticalBorder(unsigned outerVerticalBorder);
		void setBackgroundColor(Color4f backgroundColor) { this->backgroundColor = backgroundColor; };
		void setBackgroundShader(Shader backgroundShader) { this->backgroundShader = backgroundShader; };
		bool setVerticalLayoutGroupID(GuiGroupID groupID);
		bool setVerticalLayoutGroupName(const std::string& groupName);
		void setShrinkHeightToChildren(bool shrinkHeightToChildren);
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateVerticalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateOuterVerticalBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
	protected:
		/// Computes the background model matrix
		void computeModelMatrix();
		/// Draws the background. Is called by draw()
		void drawBackground(const Vec2i& worldPosition);
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the size changes. Can eg. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;

		//==============================================================================================
		// Groups
		//==============================================================================================
	protected:
		/// Removes the element from all groups it is currently in. Must be overwritten if this element
		/// belongs to any groups
		void leaveAllGroups() override;
	};
	//--------------------------------------------------------------------------------------------------
}