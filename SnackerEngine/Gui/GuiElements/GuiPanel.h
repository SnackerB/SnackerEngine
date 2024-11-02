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
		/// Static default Attributes
		static Color4f defaultBackgroundColor;
		static Shader defaultBackgroundShader;
	private:
		Color4f backgroundColor = defaultBackgroundColor;
		Mat4f modelMatrix = {};
		Shader shader = defaultBackgroundShader;
		/// Computes the modelMatrix
		void computeModelMatrix();
	protected:
		/// Returns a const reference to the model matrix
		const Mat4f& getModelMatrix() { return modelMatrix; };
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_PANEL";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiPanel(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(),
			const ResizeMode& resizeMode = ResizeMode::RESIZE_RANGE, const Color4f& backgroundColor = defaultBackgroundColor);
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
		const Color4f& getBackgroundColor() const { return backgroundColor; }
		const Shader& getPanelShader() const { return shader; }
	protected:
		/// Function that draws the panel relative to its parent element, but also rescales according
		/// to the given scaling matrix. This function can be called by child elements.
		/// Does not draw children.
		void draw(const Vec2i& worldPosition, const Mat4f& scale);
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

		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;

		//==============================================================================================
		// Animatables
		//==============================================================================================

		virtual std::unique_ptr<GuiElementAnimatable> animateBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);

	};
}