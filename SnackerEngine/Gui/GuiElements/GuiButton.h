#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Utility\Handles\EventHandle.h"

namespace SnackerEngine
{
	class GuiButton : public GuiTextBox
	{
	public:
		/// Static default Attributes
		static Color4f defaultDefaultColor;
		static Color4f defaultHoverColor;
		static Color4f defaultPressedColor;
		static Color4f defaultPressedHoverColor;
		static Color4f defaultLockedColor;
	private:
		/// The event that happens when the button is pressed
		EventHandle::Observable observableButtonPressed;
		/// Different colors of the button for the different stages
		Color4f defaultColor = defaultDefaultColor;
		Color4f hoverColor = defaultHoverColor;
		Color4f pressedColor = defaultPressedColor;
		Color4f pressedHoverColor = defaultPressedHoverColor;
		Color4f lockedColor = defaultLockedColor;
		/// Set to true if the button was pressed and the mouse is held down currently
		bool isBeingPressed = false;
		/// Set to true if the mouse is currently inside the button
		bool isBeingHovered = false;
		/// Set to true if the button is currently locked
		bool locked = false;
	protected:
		/// Helper function that is called when the button is pressed.
		/// Can be overwritten by child elements
		virtual void onButtonPress();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_BUTTON";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiButton(const Vec2i& position = Vec2i{}, const Vec2i& size = Vec2i{}, const std::string& text = "");
		GuiButton(const std::string& label);
		/// Constructor from JSON
		GuiButton(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiButton();
		/// Copy constructor and assignment operator
		GuiButton(const GuiButton& other) noexcept;
		GuiButton& operator=(const GuiButton& other) noexcept;
		/// Move constructor and assignment operator
		GuiButton(GuiButton&& other) noexcept;
		GuiButton& operator=(GuiButton&& other) noexcept;
		/// Locks this button
		void lock();
		/// Unlocks this button
		void unlock();
		/// Setters
		virtual void setDefaultColor(const Color4f& defaultColor);
		virtual void setHoverColor(const Color4f& hoverColor);
		virtual void setPressedColor(const Color4f& pressedColor);
		virtual void setPressedHoverColor(const Color4f& pressedHoverColor);
		virtual void setLockedColor(const Color4f& lockedColor);
		/// Getters
		const Color4f& getDefaultColorColor() const { return defaultColor; }
		const Color4f& getHoverColor() const { return hoverColor; }
		const Color4f& getPressedColor() const { return pressedColor; }
		const Color4f& getpressedHoverColor() const { return pressedHoverColor; }
		const Color4f& getLockedColor() const { return lockedColor; }
		bool isLocked() const { return locked; }

		//==============================================================================================
		// Animatables
		//==============================================================================================

		std::unique_ptr<GuiElementAnimatable> animateDefaultColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateHoverColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animatePressedColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animatePressedHoverColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateLockedColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);

	protected:

		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;

		//==============================================================================================
		// EventHandles
		//==============================================================================================

	public:
		/// Subscribes an event handle
		void subscribeToEventButtonPress(EventHandle& eventHandle);

		//==============================================================================================
		// Collisions
		//==============================================================================================

	public:
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;

		//==============================================================================================
		// Events
		//==============================================================================================

	protected:
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
	};
}