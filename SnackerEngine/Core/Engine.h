#pragma once

#include "Core/Scene.h"
#include "Utility\Buffer.h"

#include <string>
#include <chrono>
#include <random>
#include <vector>
#include "External\nlohmann_json\json.hpp"

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
		inline static std::vector<std::string> resourcePaths{};
		/// Currently active scene
		inline static Scene* activeScene = nullptr;
		/// The last time the active scene was updated
		inline static std::chrono::high_resolution_clock::time_point lastUpdateTime{};
		/// if set to true, the mouse is constrained to the middle of the screen
		inline static bool constrainMouseToScreenCenter = false;
		/// Random Engine
		inline static std::default_random_engine randomEngine{};
		/// bool that decides if the engine is currently running
		inline static bool running;

		/// Determines the resource path. Prints info/error about the location of the path
		static bool determineResourcePath(const std::string& resourceFolderPath = "");
		/// Callback functiions
		static void callbackKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void callbackMouseButton(GLFWwindow* window, int button, int action, int mods);
		static void callbackMouseMotion(GLFWwindow* window, double xpos, double ypos);
		static void callbackWindowResize(GLFWwindow* window, int width, int height);
		static void callbackMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
		static void callbackCharacterInput(GLFWwindow* window, unsigned int codepoint);
	public:
		/// Initializes the engine and creates a window
		static bool initialize(const int& windowWidth, const int& windowHeight, const std::string& windowName, const std::string& resourceFolderPath = "");
		/// Terminates the engine
		static void terminate();
		/// Sets the active scene
		static void setActiveScene(Scene& scene);
		/// Starts the main loop
		static void startup();
		/// Constrains the mouse to the center of the screen after each call to mouseMovementCallback!
		/// Returns the current mouse position
		static Vec2i constrainMouseCenter(const bool& enable);
		/// Returns the x and y dpi of the monitor the engine is run on
		static Vec2<unsigned int> getDPI();
		/// Returns a random number generator that can be used for non-critical random numbers!
		static std::default_random_engine& getRandomEngine() { return randomEngine; }
		/// Stops the engine, can be called by scenes, eg. when a "Return to Desktop" button is clicked
		static void stopEngine();
		/// Adds a new resource folder path. Paths are relative to the executable
		static void addResourceFolderPath(const std::string& path);
		/// Checks if the given file exists relative to any of the known resource folder
		/// paths. If it does, the full path is returned (relative to the executable). If it does not, 
		/// an empty optional is returned instead
		static std::optional<std::string> getFullPath(const std::string& path);
		/// Returns the default resource path (the first of the known resource paths)
		static const std::string& getDefaultResourcePath();
		/// Loads a file relative to the resource path. Returns an empty optional if anything fails
		static std::optional<Buffer> loadFileRelativeToResourcePath(const std::string& path);
		/// Loads and parses a JSON file relative to the resource path. Returns an
		/// empty optional if anything fails
		static std::optional<nlohmann::json> loadJSONRelativeToResourcePath(const std::string& path);
		/// Saves JSON into a file relative to the resource path. Returns true on success
		static bool saveJSONRelativeToResourcePath(const nlohmann::json& json, const std::string& path);
		/// Deleted destructor: this is a static class!
		Engine() = delete;
	};
	//------------------------------------------------------------------------------------------------------
}