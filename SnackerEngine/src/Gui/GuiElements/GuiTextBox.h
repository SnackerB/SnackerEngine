#pragma once

#include "Gui/GuiElement.h"
#include "Gui/Text/Text.h"
#include "Graphics/Material.h"
#include "Graphics/Color.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declaration of GuiStyle
	struct GuiStyle;
	//--------------------------------------------------------------------------------------------------
	class GuiDynamicTextBox : public GuiElement
	{
	public:
		/// Controls how the textbox changes its size for different texts
		enum class TextBoxMode
		{
			FORCE_SIZE,				/// The textBox has the given size, no matter
									/// the actual text size. Default mode.
			SHRINK_HEIGHT_TO_FIT,	/// Computes the size of the text with a given width
									/// and adjusts the height accordingly
		};
	protected:
		/// The text object. This needs to be a pointer such that the Edit text class can save an 
		/// EditableText object in this variable instead (thus avoiding code duplication)
		std::unique_ptr<DynamicText> text;
		/// Material, containing font, textColor and backgroundColor.
		/// MaterialData is of type SimpleTextMaterialData
		Material material;
		/// Background color
		Color4f backgroundColor;
		/// Text color
		Color4f textColor;
		/// Shader for drawing the background
		Shader backgroundShader;
		/// Model matrices of the text and the background box
		Mat4f modelMatrixText;
		Mat4f modelMatrixBackground;
		/// The textBoxMode
		TextBoxMode textBoxMode;
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
		/// Protected constructor that is used by the derived class EditTextBox. Creates an
		/// EditableText object instead of a DynamicText object for the "text" member variable
		
	public:
		/// Constructor
		GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font,
			const double& fontSize, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE);
		/// Constructors using GuiStyle
		GuiDynamicTextBox(const std::string& text, const GuiStyle& style);
		GuiDynamicTextBox(const std::string& text, const double& fontSize, const GuiStyle& style);
		/// Copy and Move constructors
		GuiDynamicTextBox(const GuiDynamicTextBox& other) noexcept;
		GuiDynamicTextBox(GuiDynamicTextBox&& other) noexcept;
		/// Copy and Move assignment operators
		GuiDynamicTextBox& operator=(const GuiDynamicTextBox& other) noexcept;
		GuiDynamicTextBox& operator=(GuiDynamicTextBox&& other) noexcept;
		/// Setter and Getters
		void setText(const std::string& text);
		void setFont(const Font& font);
		void setFontSize(const double& fontSize);
		void setTextColor(const Color4f& textColor);
		void setBackgroundColor(const Color4f& backgroundColor);
		void setTextParseMode(const StaticText::ParseMode& parseMode);
		void setAlignment(const StaticText::Alignment& alignment);
		void setTextBoxMode(const TextBoxMode& textBoxMode);
		const std::string& getText() { return text->getText(); }
		const Font& getFont() const { return text->getFont(); }
		const double& getFontSize() const { return text->getFontSize(); }
		const Color4f& getTextColor() const { return textColor; }
		const Color4f& getBackgroundColor() const { return backgroundColor; };
		const StaticText::ParseMode& getParseMode() const { return text->getParseMode(); }
		const StaticText::Alignment& getAlignment() const { return text->getAlignment(); }
		const TextBoxMode& getTextBoxMode() const { return textBoxMode; }
	};
	/*
	//--------------------------------------------------------------------------------------------------
	class GuiEditTextBox : public GuiDynamicTextBox
	{
		/// Constructor
		GuiEditTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font,
			const double& fontSize, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE);
		/// Constructors using GuiStyle
		GuiEditTextBox(const std::string& text, const GuiStyle& style);
		GuiEditTextBox(const std::string& text, const double& fontSize, const GuiStyle& style);
		/// Copy and Move constructors
		GuiEditTextBox(const GuiDynamicTextBox& other) noexcept;
		GuiEditTextBox(GuiDynamicTextBox&& other) noexcept;
		/// Copy and Move assignment operators
		GuiEditTextBox& operator=(const GuiEditTextBox& other) noexcept;
		GuiEditTextBox& operator=(GuiEditTextBox&& other) noexcept;
	};
	*/
}