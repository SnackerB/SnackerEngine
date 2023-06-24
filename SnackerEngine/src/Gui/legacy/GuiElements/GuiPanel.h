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
		Color4f backgroundColor;
		Mat4f modelMatrix;
		Shader shader;
		/// Computes the modelMatrix
		void computeModelMatrix();
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called when the position changes. Can eg. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onPositionChange() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) override;
		/// Returns a constant reference to the shader
		const Shader& getPanelShader() const;
	public:
		/// Constructor
		GuiPanel(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(),
			const ResizeMode& resizeMode = ResizeMode::RESIZE_RANGE, const Color4f& backgroundColor = Color4f());
		/// Setter and Getter for the background color
		void setBackgroundColor(const Color4f& backgroundColor);
		Color4f getBackgroundColor() const;
		/// Copy constructor and assignment operator
		GuiPanel(const GuiPanel& other) noexcept;
		GuiPanel& operator=(const GuiPanel& other) noexcept;
		/// Move constructor and assignment operator
		GuiPanel(GuiPanel&& other) noexcept;
		GuiPanel& operator=(GuiPanel&& other) noexcept;
		/// Destructor
		virtual ~GuiPanel() {};
	};
	//--------------------------------------------------------------------------------------------------

}