#pragma once

#include "Gui/Layouts/HorizontalLayout.h"
#include "Graphics/Color.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiHorizontalListLayout : public GuiHorizontalLayout
	{
	private:
		/// Horizontal alignment of the layout
		AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::LEFT;
		/// Border between different elements and between left/right edge of layout and elements, in pixels
		unsigned horizontalBorder = 10;
		/// Background color
		Color4f backgroundColor = Color4f(0.1f, 1.0f);
		/// model matrix of the background
		Mat4f modelMatrixBackground{};
		/// Shader for drawing the background
		Shader backgroundShader = Shader("shaders/gui/simpleAlphaColor.shader");
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_HORIZONTAL_LIST_LAYOUT";
		/// Default constructor
		GuiHorizontalListLayout(Color4f backgroundColor = Color4f(1.0f, 0.0f));
		/// Constructor from JSON.
		GuiHorizontalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiHorizontalListLayout() {};
		/// Copy constructor and assignment operator
		GuiHorizontalListLayout(const GuiHorizontalListLayout& other) noexcept = default;
		GuiHorizontalListLayout& operator=(const GuiHorizontalListLayout& other) noexcept = default;
		/// Move constructor and assignment operator
		GuiHorizontalListLayout(GuiHorizontalListLayout&& other) noexcept = default;
		GuiHorizontalListLayout& operator=(GuiHorizontalListLayout&& other) noexcept = default;
		/// Getters
		unsigned getHorizontalBorder() const { return horizontalBorder; }
		Color4f getBackgroundColor() const { return backgroundColor; }
		Shader& getBackgroundShader() { return backgroundShader; }
		/// Setters
		void setHorizontalBorder(unsigned horizontalBorder);
		void setBackgroundColor(Color4f backgroundColor) { this->backgroundColor = backgroundColor; };
		void setBackgroundShader(Shader backgroundShader) { this->backgroundShader = backgroundShader; };
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
	};
	//--------------------------------------------------------------------------------------------------
}