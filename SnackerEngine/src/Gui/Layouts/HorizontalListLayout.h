#pragma once

#include "Gui/Layouts/HorizontalLayout.h"
#include "Graphics/Color.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"
#include "Gui\Group.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiHorizontalListLayout : public GuiHorizontalLayout
	{
	public:
		/// Static default Attributes
		static Color4f defaultBackgroundColor;
	private:
		/// Class that is used for grouping multiple horizontal layouts together
		class HorizontalLayoutGroup : public GuiGroup
		{
		public:
			/// The group type, necessary for efficient casting
			std::string getGroupType() const override { return "HORIZONTAL_LAYOUT_GROUP"; }
		private:
			std::vector<int> widths{};
			bool upToDate = false;
		protected:
			/// This function gets called when a new element is added to the group
			void onElementJoin(GuiElement::GuiID element, std::size_t position) override;
			/// This function gets called when a new element leaves the group
			void onElementLeave(GuiElement::GuiID element, std::size_t position) override;
		public:
			/// Constructor
			HorizontalLayoutGroup(const std::string& name = "")
				: GuiGroup(name) {}
			/// If upToDate is set to false, the widths vector is recomputed and
			/// upToDate is set to true.
			void recomputeWidths();
			/// Getters
			bool isUpToDate() const { return upToDate; }
			const std::vector<int>& getWidths() const { return widths; }
			/// Setters
			void markForRecompute() { if (upToDate) onElementJoin(-1, -1); };
			void setShrinkWidthToChildren(bool shrinkWidthToChildren);
		};
		friend class HorizontalLayoutGroup;
		/// Horizontal alignment of the layout
		AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::LEFT;
		/// Border between different elements and between left/right edge of layout and elements, in pixels
		unsigned horizontalBorder = defaultBorderNormal;
		/// Border between left/right edge of layout and elements in pixels
		unsigned outerHorizontalBorder = defaultBorderNormal;
		/// Background color
		Color4f backgroundColor = defaultBackgroundColor;
		/// model matrix of the background
		Mat4f modelMatrixBackground{};
		/// Shader for drawing the background
		Shader backgroundShader = Shader("shaders/gui/simpleAlphaColor.shader");
		/// The ID of the HorizontalLayoutGroup this element is in
		GuiGroupID groupID = -1;
		/// The name of the HorizontalLayoutGroup this element is in
		std::string groupName = "";
		/// If this is set to true, the layouts minWidth and preferredWidth are set according
		/// to child elements
		bool shrinkWidthToChildren = true;
		/// Helper function for enforcing the layout
		static std::vector<int> computeChildWidths(const std::vector<int>& minWidths, const std::vector<int>& preferredWidths, const std::vector<int>& maxWidths, int width);
	protected:
		/// Helper function that computes the minWidth of the layout from its childElements.
		void computeWidthHintsFromChildren();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_HORIZONTAL_LIST_LAYOUT";
		/// Default constructor
		GuiHorizontalListLayout(Color4f backgroundColor = Color4f(1.0f, 0.0f));
		/// Constructor from JSON.
		GuiHorizontalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiHorizontalListLayout();
		/// Copy constructor and assignment operator
		GuiHorizontalListLayout(const GuiHorizontalListLayout& other) noexcept;
		GuiHorizontalListLayout& operator=(const GuiHorizontalListLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiHorizontalListLayout(GuiHorizontalListLayout&& other) noexcept;
		GuiHorizontalListLayout& operator=(GuiHorizontalListLayout&& other) noexcept;
		/// Getters
		AlignmentHorizontal getAlignmentHorizontal() const { return alignmentHorizontal; }
		unsigned getHorizontalBorder() const { return horizontalBorder; }
		unsigned getOuterHorizontalBorder() const { return outerHorizontalBorder; }
		Color4f getBackgroundColor() const { return backgroundColor; }
		Shader& getBackgroundShader() { return backgroundShader; }
		std::optional<GuiGroupID> getHorizontalLayoutGroupID() const { return groupID == -1 ? std::optional<GuiGroupID>() : groupID; }
		const std::string* getHorizontalLayoutGroupName() const { return groupID == -1 ? nullptr : &groupName; }
		bool isShrinkWidthToChildren() const { return this->shrinkWidthToChildren; }
		/// Setters
		void setAlignmentHorizontal(AlignmentHorizontal alignmentHorizontal);
		void setHorizontalBorder(unsigned horizontalBorder);
		void setOuterHorizontalBorder(unsigned outerHorizontalBorder);
		void setBackgroundColor(Color4f backgroundColor) { this->backgroundColor = backgroundColor; };
		void setBackgroundShader(Shader backgroundShader) { this->backgroundShader = backgroundShader; };
		bool setHorizontalLayoutGroupID(GuiGroupID groupID);
		bool setHorizontalLayoutGroupName(const std::string& groupName);
		void setShrinkWidthToChildren(bool shrinkWidthToChildren);
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