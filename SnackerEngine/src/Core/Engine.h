#pragma once

#include "Core/Scene.h"

#include <string>
#include <chrono>

//----------------------------------------------------------------------------------------------------------
/// Forward declaration of GLFWindow class
struct GLFWwindow;
//----------------------------------------------------------------------------------------------------------
namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// This is the main class for controlling the engine!
	/// Note that this is a purely static class
	class Engine
	{
		/// Path to resource folder
		inline static std::string resourcePath = "";
		/// Currently active scene
		inline static Scene* activeScene = nullptr;
		/// The last time the active scene was updated
		inline static std::chrono::high_resolution_clock::time_point lastUpdateTime{};
		/// if set to true, the mouse is constrained to the middle of the screen
		inline static bool constrainMouseToScreenCenter = false;

		/// Determines the resource path. Prints info/warning about the location of the path
		static void determineResourcePath();
		/// Callback functiions
		static void callbackKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void callbackMouseButton(GLFWwindow* window, int button, int action, int mods);
		static void callbackMouseMotion(GLFWwindow* window, double xpos, double ypos);
		static void callbackWindowResize(GLFWwindow* window, int width, int height);
		static void callbackMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
		static void callbackCharacterInput(GLFWwindow* window, unsigned int codepoint);
	public:
		/// Initializes the engine and creates a window
		static bool initialize(const int& windowWidth, const int& windowHeight, const std::string& windowName);
		/// Terminates the engine
		static void terminate();
		/// Obtains resource path
		static const std::string& getResourcePath();
		/// Sets resource path
		static void setResourcePath(const std::string& path);
		/// Sets the active scene
		static void setActiveScene(Scene& scene);
		/// Starts the main loop
		static void startup();
		/// Constrains the mouse to the center of the screen after each call to mouseMovementCallback!
		/// Returns the current mouse position
		static Vec2i constrainMouseCenter(const bool& enable);
		/// Returns the x and y dpi of the monitor the engine is run on
		static Vec2<unsigned int> getDPI();
		/// Deleted destructor: this is a static class!
		Engine() = delete;
	};
	//------------------------------------------------------------------------------------------------------
}