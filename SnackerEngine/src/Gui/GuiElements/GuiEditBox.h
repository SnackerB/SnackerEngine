#pragma once

#include "Gui\GuiElements\GuiTextBox.h"
#include "Core\Timer.h"

namespace SnackerEngine 
{

	class GuiEditBox : public GuiTextBox
	{
	private:
		/// Color of the selection Box
		Color4f selectionBoxColor = Color4f(1.0f, 0.0f, 0.0f, 0.5f);
		/// The width of the cursor
		float cursorWidth = 0.1f;
		/// If this is set to true, the text box is currently being edited. This also means that the
		/// cursor should be blinking.
		bool active = false;
		/// If this is set to true, the cursor is currently visible. This bool is switched
		/// on and off while the cursor is blinking
		bool cursorIsVisible = false;
		/// The timer controlling the blinking time
		Timer cursorBlinkingTimer = Timer(0.5);
		/// The model matrix of the cursor
		Mat4f modelMatrixCursor{};
		/// The model matrices of the selection boxes
		std::vector<Mat4f> modelMatricesSelectionBoxes{};
		/// The event that happens when the text was edited (isActive is set to true and enter
		/// or escape is pressed or the user clicks outside the textBox)
		GuiEventHandle* eventHandleTextWasEdited = nullptr;
		/// The event that happens when the isActive was set to true and enter is pressed
		GuiEventHandle* eventHandleEnterWasPressed = nullptr;
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
		/// Default constructor
		GuiEditBox(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& text = "", const Font& font = Font("fonts/arial.ttf"), const double& fontSize = 10);
		/// Constructor from JSON
		GuiEditBox(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiEditBox();
		/// Copy constructor and assignment operator
		GuiEditBox(const GuiEditBox& other) noexcept;
		GuiEditBox& operator=(const GuiEditBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiEditBox(GuiEditBox&& other) noexcept;
		GuiEditBox& operator=(GuiEditBox&& other) noexcept;
		/// Getters
		const Color4f& getSelectionBoxColor() const { return selectionBoxColor; }
		bool isActive() const { return active; }
		float getCursorWidth() { return cursorWidth; };
		double getCursorBlinkTime() const { return cursorBlinkingTimer.getTimeStep(); }
		/// Setters
		void setSelectionBoxColor(const Color4f& selectionBoxColor) { this->selectionBoxColor = selectionBoxColor; }
		void setCursorWidth(float cursorWidth);
		double setCursorBlinkTime(double cursorBlinkTime) { cursorBlinkingTimer.setTimeStep(cursorBlinkTime); }
		/// Sets the event handle for the event that happens when the text was edited 
		/// (isActive is set to true and enter or escape is pressed or the user clicks 
		/// outside the textBox). Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setEventHandleTextWasEdited(GuiEventHandle& eventHandle);
		/// Sets the event handle for the event that happens when isActive was set to true and 
		/// enter is pressed. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setEventHandleEnterWasPressed(GuiEventHandle& eventHandle);
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
		// GuiHandles
		//==============================================================================================
		
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		
		//==============================================================================================
		// Collisions
		//==============================================================================================

	public:
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const;
	
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