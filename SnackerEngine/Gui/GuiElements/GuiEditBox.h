#pragma once

#include "Gui\GuiElements\GuiTextBox.h"
#include "Utility\Timer.h"
#include "Utility\Handles\EventHandle.h"

namespace SnackerEngine 
{

	class GuiEditBox : public GuiTextBox
	{
	public:
		/// Static default Attributes
		static Color4f defaultSelectionBoxColor;
		static Color4f defaultBackgroundColor;
		static float defaultCursorWidth;
		static double defaultCursorBlinkingTime;
		static SizeHintModes defaultSizeHintModes;
	private:
		/// Color of the selection Box
		Color4f selectionBoxColor = defaultSelectionBoxColor;
		/// The width of the cursor
		float cursorWidth = defaultCursorWidth;
		/// If this is set to true, the text box is currently being edited. This also means that the
		/// cursor should be blinking.
		bool active = false;
		/// If this is set to true, the cursor is currently visible. This bool is switched
		/// on and off while the cursor is blinking
		bool cursorIsVisible = false;
		/// If this is set to true, the text is repositioned such that the cursor is always visible
		bool repositionTextToFitCursor = true;
		/// text position, relative to the background
		Vec2f textPosition{};
		/// text offset relative to the normal textPosition
		Vec2f textOffset{};
		/// offset of the cursor, relative to the position of the text
		Vec2f cursorOffsetFromText{};
		/// offset of the cursor, relative to the background
		Vec2f cursorPosition{};
		/// The timer controlling the blinking time
		Timer cursorBlinkingTimer = Timer(defaultCursorBlinkingTime);
		/// The model matrix of the cursor
		Mat4f modelMatrixCursor{};
		/// The model matrices of the selection boxes
		std::vector<Mat4f> modelMatricesSelectionBoxes{};
		/// The event that happens when the text was edited (isActive is set to true and enter
		/// or escape is pressed or the user clicks outside the textBox)
		EventHandle::Observable eventTextWasEdited;
		/// The event that happens when the isActive was set to true and enter is pressed
		EventHandle::Observable eventEnterWasPressed;
		/// Helper functions for computing textOffsrt and cursorOffset
		void computeTextOffsetAndCursorOffset(); // Full recompute
		void computeTextOffsetAndCursorOffsetKeepTextOffset(); // Try to keep textOffset
		void computeTextOffsetAndCursorOffsetKeepCursorOffset(); // Try to keep cursorOffset
		void computeTextOffsetAndCursorOffsetAdvanceCursor(); // Try to advance cursor while keeping textOffset constant
		/// Helper function that computes the model matrix of the cursor
		void computeModelMatrixCursor();
		/// Helper function that Computes the model matrices of the selection boxes
		void computeModelMatricesSelectionBoxes();
		/// Helper function that returns mouse offset to the (0, 0) point of the text object in pt
		Vec2d getMouseOffsetToText();
	protected:
		/// Gets called when, while in editing mode, the left mouse button is pressed outside the editbox
		/// or enter is pressed in SINGLE_LINE mode. Can be overwritten by derived GuiElements.
		virtual void OnTextEdit();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_EDIT_BOX";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiEditBox(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& text = "", const Font& font = defaultFont, const double& fontSize = defaultFontSizeNormal, std::optional<double> lineHeight = std::nullopt, const Color4f& backgroundColor = defaultBackgroundColor);
		/// Constructor from JSON
		GuiEditBox(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiEditBox() {};
		/// Copy constructor and assignment operator
		GuiEditBox(const GuiEditBox& other) noexcept;
		GuiEditBox& operator=(const GuiEditBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiEditBox(GuiEditBox&& other) noexcept;
		GuiEditBox& operator=(GuiEditBox&& other) noexcept;
		/// Getters
		const Color4f& getSelectionBoxColor() const { return selectionBoxColor; }
		bool isActive() const { return active; }
		float getCursorWidth() const { return cursorWidth; };
		double getCursorBlinkTime() const { return cursorBlinkingTimer.getTimeStep(); }
		/// Setters
		void setSelectionBoxColor(const Color4f& selectionBoxColor) { this->selectionBoxColor = selectionBoxColor; }
		void setCursorWidth(float cursorWidth);
		void setCursorBlinkTime(double cursorBlinkTime) { cursorBlinkingTimer.setTimeStep(cursorBlinkTime); }
		/// Subscribes to the event that happens when the text was edited 
		/// (isActive is set to true and enter or escape is pressed or the user clicks 
		/// outside the textBox)
		void subscribeEventTextWasEdited(EventHandle& eventHandle);
		/// Subscribes to the event that happens when isActive was set to true and 
		/// enter is pressed.
		void setEventHandleEnterWasPressed(EventHandle& eventHandle);
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this guiInteractable object.
		/// When this function is called, the guiManager pointer is set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function gets called when the size changes. Not called by the constructor!
		virtual void onSizeChange() override;
		
		//==============================================================================================
		// Collisions
		//==============================================================================================

	public:
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;

		//==============================================================================================
		// Animatables
		//==============================================================================================

		std::unique_ptr<GuiElementAnimatable> animateSelectionBoxColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateCursorWidth(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateCursorBlinkTime(const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
	
	protected:

		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position);
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
		/// Update function
		virtual void update(const double& dt) override;

	};

}