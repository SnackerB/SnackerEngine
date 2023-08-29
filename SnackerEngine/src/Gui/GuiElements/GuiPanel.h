#pragma once

#include "Gui/GuiElement.h"
#include "Graphics/Color.h"
#include "Graphics/Shader.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiPanel : public GuiElement
	{
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_PANEL";
		/// Default constructor
		GuiPanel(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(),
			const ResizeMode& resizeMode = ResizeMode::RESIZE_RANGE, const Color4f& backgroundColor = Color4f()); // NOTE: All parameters must have default values!
		/// Constructor from JSON. 
		GuiPanel(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiPanel() {};
		/// Copy constructor and assignment operator
		GuiPanel(const GuiPanel& other) noexcept;
		GuiPanel& operator=(const GuiPanel& other) noexcept;
		/// Move constructor and assignment operator
		GuiPanel(GuiPanel&& other) noexcept;
		GuiPanel& operator=(GuiPanel&& other) noexcept;
		/// Setters and Getters
		virtual void setBackgroundColor(const Color4f& backgroundColor);
		Color4f getBackgroundColor() const;
		const Shader& getPanelShader() const;
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition);
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the position changes. Can eg. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onPositionChange();
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange();

		//==============================================================================================
		// Collisions
		//==============================================================================================

	public:
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const;

	private:
		Color4f backgroundColor = Color4f(0.0f, 1.0f);
		Mat4f modelMatrix = {};
		Shader shader = Shader("shaders/gui/simpleAlphaColor.shader");
		/// Computes the modelMatrix
		void computeModelMatrix();
	};
}