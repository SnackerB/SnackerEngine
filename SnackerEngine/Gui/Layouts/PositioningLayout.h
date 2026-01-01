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
		int leftBorder = 0;
		int rightBorder = 0;
		int topBorder = 0;
		int bottomBorder = 0;
		/// If set to true, the layout sets its width hints as the childs width hints plus the optional left/right borders.
		/// For this, the smallest maxWidth and the largest preferredWidth and maxWidth are chosen out of the children.
		bool shrinkWidthToChildren = false;
		/// If set to true, the layout sets its height hints as the childs height hints plus the optional top/bottom borders.
		/// For this, the smallest maxHeight and the largest preferredHeight and maxheight are chosen out of the children.
		bool shrinkHeightToChildren = false;
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_POSITIONING_LAYOUT";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiPositioningLayout(Mode mode = Mode::CENTER);
		/// Constructor from JSON
		GuiPositioningLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		// Copy constructor and assignment operator
		GuiPositioningLayout(const GuiPositioningLayout& other) = default;
		GuiPositioningLayout& operator=(const GuiPositioningLayout& other) = default;
		// Move constructor and assignment operator
		GuiPositioningLayout(GuiPositioningLayout&& other) = default;
		GuiPositioningLayout& operator=(GuiPositioningLayout&& other) = default;
		/// Destructor
		virtual ~GuiPositioningLayout() {};
		/// Getters
		Mode getMode() const { return mode; }
		int getLeftBorder() const { return leftBorder; }
		int getRightBorder() const { return rightBorder; }
		int getTopBorder() const { return topBorder; }
		int getBottomBorder() const { return bottomBorder; }
		bool isShrinkWidthToChildren() const { return shrinkWidthToChildren; }
		bool isShrinkHeightToChildren() const { return shrinkHeightToChildren; }
		/// Setters
		void setLeftBorder(int leftBorder);
		void setRightBorder(int rightBorder);
		void setTopBorder(int topBorder);
		void setBottomBorder(int bottomBorder);
		void setMode(Mode mode);
		void setShrinkWidthToChildren(bool shrinkWidthToChildren);
		void setShrinkHeightToChildren(bool shrinkHeightToChildren);
	protected:
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;
	};
	//--------------------------------------------------------------------------------------------------
}