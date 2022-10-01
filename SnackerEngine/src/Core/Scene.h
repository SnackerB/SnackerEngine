#pragma once

#include "Math/Vec.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class Scene
	{
	protected:
		friend class Engine;
		/// Draws the scene to the screen. Called by the Engine class!
		virtual void draw() {};
	public:
		/// Constructor
		Scene() = default;
		/// Virtual destructor
		virtual ~Scene() = default;

		//==================================================================================================
	    // Callback functions implementation
		//==================================================================================================
		
		/// Initializes the scene (eg adds update functions, loads data, ...). This is called by the engine
		virtual void initialize() {};
		/// Callback function for keyboard input
		/// key:		The keyboard key that was pressed or released
		/// scancode:	The system-specific scancode of the key
		/// action:		GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
		/// mods:		Bit field describing which modifier keys were held down, eg. GLFW_MOD_SHIFT or GLFW_MOD_CONTROL
		virtual void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) {};
		/// Callback function for mouse button input
		/// button:		The mouse button that was pressed or released, eg. GLFW_MOUSE_BUTTON_LEFT
		/// action:		One of GLFW_PRESS or GLFW_RELEASE
		/// mods:		Bit field describing which modifier keys were held down, eg. GLFW_MOD_SHIFT or GLFW_MOD_CONTROL
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) {};
		/// Callback function for mouse motion
		/// position:	The new mouse position, relative to the top left corner
		virtual void callbackMouseMotion(const Vec2d& position) {};
		/// Callback function for resizing the window
		/// screenDims:	The new screen dimensions in pixels
		virtual void callbackWindowResize(const Vec2i& screenDims) {};
		/// Callback function for scrolling the mouse wheel
		/// offset:		scroll offset along the x- and y-axis
		virtual void callbackMouseScroll(const Vec2d& offset) {};
		/// Callback function for the input of unicode characters
		/// codepoint:	The Unicode code point of the character
		virtual void callbackCharacterInput(const unsigned int& codepoint) {};
		/// this function get calls regularly by the Engine class.
		/// dt:			The time that passed since the last update, in seconds.
		virtual void update(const double& dt) {};
	};
	//------------------------------------------------------------------------------------------------------
}