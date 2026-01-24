#pragma once

#include "Gui/GuiLayout.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiWrapperLayout : public GuiLayout
	{
	private:
		int leftBorder = 0;
		int rightBorder = 0;
		int topBorder = 0;
		int bottomBorder = 0;
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_WRAPPER_LAYOUT";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiWrapperLayout(int border);
		GuiWrapperLayout(int leftBorder, int rightBorder, int topBorder, int bottomBorder);
		/// Constructor from JSON
		GuiWrapperLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		// Copy constructor and assignment operator
		GuiWrapperLayout(const GuiWrapperLayout& other) = default;
		GuiWrapperLayout& operator=(const GuiWrapperLayout& other) = default;
		// Move constructor and assignment operator
		GuiWrapperLayout(GuiWrapperLayout&& other) = default;
		GuiWrapperLayout& operator=(GuiWrapperLayout&& other) = default;
		/// Destructor
		virtual ~GuiWrapperLayout() {};
		/// Getters
		int getLeftBorder() const { return leftBorder; }
		int getRightBorder() const { return rightBorder; }
		int getTopBorder() const { return topBorder; }
		int getBottomBorder() const { return bottomBorder; }
		/// Setters
		void setBorder(int border);
		void setLeftBorder(int leftBorder);
		void setRightBorder(int rightBorder);
		void setTopBorder(int topBorder);
		void setBottomBorder(int bottomBorder);
	protected:
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;
	};
	//--------------------------------------------------------------------------------------------------
}