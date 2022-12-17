#pragma once

#include "Gui/GuiElement.h"
#include "Graphics/Color.h"
#include "Math/Mat.h"
#include "Gui/Text/Text.h"
#include "Graphics/Material.h"
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
			FORCE_SIZE_SCALE_TEXT_UP,	/// The textBox has the given size and the text will
										/// be scaled up to try to fit the box better.
										/// Works best in the single line mode.
			FORCE_SIZE_SCALE_TEXT_DOWN, /// The textBox has the given size and the text will
										/// be scaled down to try to fit the text box.
										/// Works best in the single line mode
			FORCE_SIZE_SCALE_TEXT,		/// The textBox has the given size and the text will
										/// be scaled up and down to try to fit the text box.
										/// Works best in the single line mode
			FORCE_SIZE_RECOMPUTE_SCALE_DOWN, 
										/// The textBox has the given size. If it becomes to
										/// small the text is recomputed with a different font
										/// size to try to fit the text better. May lead to bad
										/// performance
			FORCE_SIZE_RECOMPUTE_SCALE, /// The textBox has the given size. The text is 
										/// recomputed with different font sizes
										/// to try to fit the text better. May lead to bad
										/// performance
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
		/// text box mode
		TextBoxMode textBoxMode;
		/// If this is set to true, the text is displayed in a single line
		bool singleLine;
		/// Computes the modelMatrix of the text and the background box.
		/// Depending on the textBoxMode the text is scaled appropriately.
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
		/// Helper function that recomputes the text and corresponding model matrices.
		/// This function should be called when the text needs to be recomputed, eg.
		/// when the font or fontSize changes.
		/// May also change the size of the textBox, depending on the textBoxMode
		void recomputeText();
		/// Helper function that computes the the text position
		Vec2f computeTextPosition();
		/// Protected constructor that can be used to create a TextBox with differen types of GuiText, eg.
		/// GuiDynamicText or GuiEditableText
		GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, std::unique_ptr<DynamicText>&& text,
			const Color4f& textColor, const Color4f& backgroundColor, const TextBoxMode& textBoxMode, const double& singleLine);
	public:
		/// Constructor
		GuiDynamicTextBox(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode,
			const std::string& text, const Font& font, const double& fontSize, 
			Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
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
		virtual void setText(const std::string& text);
		virtual void setFont(const Font& font);
		virtual void setFontSize(const double& fontSize);
		virtual void setTextColor(const Color4f& textColor);
		virtual void setBackgroundColor(const Color4f& backgroundColor);
		virtual void setTextParseMode(const StaticText::ParseMode& parseMode);
		virtual void setAlignment(const StaticText::Alignment& alignment);
		virtual void setTextBoxMode(const TextBoxMode& textBoxMode);
		virtual void setSingleLine(const bool& singleLine);
		const std::string& getText() { return text->getText(); }
		const Font& getFont() const { return text->getFont(); }
		const double& getFontSize() const { return text->getFontSize(); }
		const Color4f& getTextColor() const { return textColor; }
		const Vec2i getTextSize() const;
		const Color4f& getBackgroundColor() const { return backgroundColor; };
		const StaticText::ParseMode& getParseMode() const { return text->getParseMode(); }
		const StaticText::Alignment& getAlignment() const { return text->getAlignment(); }
		const TextBoxMode& getTextBoxMode() const { return textBoxMode; }
		const bool& isSingleLine() const { return singleLine; };
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
		/// The model matrices of the selection boxes
		std::vector<Mat4f> modelMatricesSelectionBoxes;
		/// The event that happens when the text was edited (isActive is set to true and enter
		/// or escape is pressed or the user clicks outside the textBox)
		GuiEventHandle* eventHandleTextWasEdited;
		/// Computes the model matrix of the cursor
		void computeModelMatrixCursor();
		/// Computes the model matrices of the selection boxes
		void computeModelMatricesSelectionBoxes();
	protected:
		/// Draws this guiInteractable object relative to its parent element
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function gets called when the position changes. Not called by the constructor!
		virtual void onPositionChange() override;
		/// This function gets called when the size changes. Not called by the constructor!
		virtual void onSizeChange() override;
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
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
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		/// Update function
		virtual void update(const double& dt) override;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// Returns mouse offset to the (0, 0) point of the text object in pt
		Vec2d getMouseOffsetToText();
	public:
		/// Constructor
		GuiEditTextBox(const Vec2i& position = {}, const Vec2i& size = {}, const ResizeMode& resizeMode = ResizeMode::DO_NOT_RESIZE,
			const std::string& text = "", const Font& font = Font(),
			const double& fontSize = 0, const double& cursorWidth = 0, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE,
			const double& singleLine = true, const double& cursorBlinkTime = 0.5);
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
		bool isActive() const { return active; }
		/// Sets the event handle for the event that happens when the text was edited 
		/// (isActive is set to true and enter or escape is pressed or the user clicks 
		/// outside the textBox). Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setEventHandleTextWasEdited(GuiEventHandle& eventHandle);
		/// Setters
		void setCursorWidth(const float& cursorWidth);
		void setCursorBlinkTime(const double& cursorBlinkTime);
		void setText(const std::string& text) override;
		void setFont(const Font& font) override;
		void setFontSize(const double& fontSize) override;
		void setTextParseMode(const StaticText::ParseMode& parseMode) override;
		void setAlignment(const StaticText::Alignment& alignment) override;
		void setTextBoxMode(const TextBoxMode& textBoxMode) override;
		void setSingleLine(const bool& singleLine) override;
	};
}