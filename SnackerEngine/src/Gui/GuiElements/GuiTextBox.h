#pragma once

#include "Gui/GuiElement.h"
#include "Gui/Text/Text.h"
#include "Gui/Text/Text2.h"
#include "Graphics/Material.h"
#include "Graphics/Color.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiStaticTextBox : public GuiElement
	{
	private:
		/// The text object
		DynamicText2 text;
		/// Material, containing font, textColor and backgroundColor.
		/// MaterialData is of type SimpleTextMaterialData
		Material material;
		/// Background color
		Color4f backgroundColor;
		/// Shader for drawing the background
		Shader backgroundShader;
		/// Model matrices of the text and the background box
		Mat4f modelMatrixText;
		Mat4f modelMatrixBackground;
		/// Computes the modelMatrix of the text and the background box
		void computeModelMatrices();
	protected:
		/// Draws this GuiTextBox
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function gets called when the position changes. Not called by the constructor!
		virtual void onPositionChange() override;
		/// This function gets called when the size changes. Not called by the constructor!
		virtual void onSizeChange() override;
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// This function is called by the guiManager after registering this guiElement.
		/// When this function is called, the guiManager pointer, the guiID, and the parent element id are set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
	public:
		/// Constructor
		GuiStaticTextBox(const Vec2i& position, const Vec2i& size, const std::u8string& text, const Font& font,
			const double& fontSize, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText2::ParseMode& parseMode = StaticText2::ParseMode::WORD_BY_WORD, const StaticText2::Alignment& alignment = StaticText2::Alignment::LEFT);
		/// Setter and Getters
		void setText(const std::u8string& text);
		void setFont(const Font& font);
		void setFontSize(const double& fontSize);
		void setTextColor(const Color4f& textColor);
		void setBackgroundColor(const Color4f& backgroundColor);
		void setTextParseMode(const TextParseMode& parseMode);
		void setAlignment(const Alignment& alignment);

	};
	//--------------------------------------------------------------------------------------------------
}