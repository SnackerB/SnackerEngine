#pragma once

#include "Gui/GuiElement.h"
#include "Gui/Text/Text.h"
#include "Graphics/Material.h"
#include "Graphics/Color.h"
#include "Core/Timer.h"
#include "Gui/GuiEventHandles/GuiEventHandle.h"

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
			FORCE_SIZE,					/// The textBox has the given size, no matter
										/// the actual text size. Default mode. Part
										/// of the text may not be visible
			SHRINK_HEIGHT_TO_FIT,		/// Computes the size of the text with a given width
										/// and adjusts the height accordingly. size.x sets the
										/// maximum textwidth
			SHRINK_WIDTH_TO_FIT,		/// computes the size of the text with textWidth set to 0.0
										/// and adjusts the width accordingly. size.y sets the height
										/// of the textBox
			SHRINK_TO_FIT,				/// computes the size of the text with textWidth set to 0.0
										/// and adjusts the size and width accordingly afterwards. 
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
		/// Wether the text should be placed in a single line
		bool singleLine;
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
		/// Protected constructor that can be used to create a TextBox with differen types of GuiText, eg.
		/// GuiDynamicText or GuiEditableText
		GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, std::unique_ptr<DynamicText>&& text, const Font& font, 
			const Color4f& textColor, const Color4f& backgroundColor, const TextBoxMode& textBoxMode, const double& singleLine);
		
	public:
		/// Constructor
		GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font,
			const double& fontSize, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE, const double& singleLine = false);
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
	//--------------------------------------------------------------------------------------------------
	class GuiEditTextBox : public GuiDynamicTextBox
	{
		/// If this is set to true, the text box is currently being edited. This also means that the
		/// cursor should be blinking.
		bool active;
		/// If this is set to true, the cursor is currently visible. This bool is switched
		/// on and off while the cursor is blinking
		bool cursorIsVisible;
		/// The timer controlling the blinking time
		Timer cursorBlinkingTimer;
		/// The model matrix of the cursor
		Mat4f modelMatrixCursor;
		/// The event that happens when the text was edited (isActive is set to true and enter
		/// or escape is pressed or the user clicks outside the textBox)
		GuiEventHandle* eventHandleTextWasEdited;
		/// Computes the model matrix of the cursor
		void computeModelMatrixCursor();
	protected:
		/// Draws this guiInteractable object relative to its parent element
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function gets called when the position changes. Not called by the constructor!
		virtual void onPositionChange() override;
		/// This function gets called when the size changes. Not called by the constructor!
		virtual void onSizeChange() override;
		/// This function is called by the guiManager after registering this guiInteractable object.
		/// When this function is called, the guiManager pointer is set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		// callback function for keyboard input. Parameters the same as in Scene.h
		virtual void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) override;
		/// Callback function for the input of unicode characters. Parameter the same as in Scene.h
		virtual void callbackCharacterInput(const unsigned int& codepoint) override;
		/// Callback function for mouse button input on this guiInteractable object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Update function
		virtual void update(const double& dt) override;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// This function is called by the guiManager after registering this guiElement.
		/// When this function is called, the guiManager pointer, the guiID, and the parent element id are set.
		/// This function can e.g. be used for registering callbacks at the guiManager
	public:
		/// Constructor
		GuiEditTextBox(const Vec2i& position = {}, const Vec2i& size = {}, const std::string& text = "", const Font& font = Font(),
			const double& fontSize = 0, const double& cursorWidth = 0, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE, 
			const double& singleLine = false, const double& cursorBlinkTime = 0.5);
		/// Constructors using GuiStyle
		GuiEditTextBox(const std::string& text, const GuiStyle& style);
		GuiEditTextBox(const std::string& text, const double& fontSize, const GuiStyle& style);
		/// Copy and Move constructors
		GuiEditTextBox(const GuiEditTextBox& other) noexcept;
		GuiEditTextBox(GuiEditTextBox&& other) noexcept;
		/// Copy and Move assignment operators
		GuiEditTextBox& operator=(const GuiEditTextBox& other) noexcept;
		GuiEditTextBox& operator=(GuiEditTextBox&& other) noexcept;
		/// Returns true if the text is currently being edited
		bool isActive() const;
		/// Sets the event handle for the event that happens when the text was edited 
		/// (isActive is set to true and enter or escape is pressed or the user clicks 
		/// outside the textBox). Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setEventHandleTextWasEdited(GuiEventHandle& eventHandle);
	};
}