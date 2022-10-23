#pragma once

#include "Gui/GuiElement.h"
#include "Graphics/Color.h"
#include "Graphics/Shader.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiPanel : public GuiElement
	{
	private:
		Color3f backgroundColor;
		Mat4f modelMatrix;
		Shader shader;
		/// Computes the modelMatrix
		void computeModelMatrix();
	protected:
		/// Draws this GuiPanel
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function gets called when the position changes. Not called by the constructor!
		virtual void onPositionChange() override;
		/// This function gets called when the size changes. Not called by the constructor!
		virtual void onSizeChange() override;
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Returns a constant reference to the shader
		const Shader& getPanelShader() const;
	public:
		/// Constructor
		GuiPanel(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Color3f& backgroundColor = Color3f());
		/// Setter and Getter for the background color
		void setBackgroundColor(const Color3f& backgroundColor);
		Color3f getBackgroundColor() const;
		/// Copy constructor
		GuiPanel(const GuiPanel& other) noexcept;
		/// Copy assignment operator
		GuiPanel& operator=(const GuiPanel& other) noexcept;
		/// Move constructor
		GuiPanel(GuiPanel&& other) noexcept;
		/// Move assignment operator
		GuiPanel& operator=(GuiPanel&& other) noexcept;
	};
	//--------------------------------------------------------------------------------------------------
}