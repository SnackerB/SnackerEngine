#pragma once

#include "Math/Vec.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Graphics/Color.h"

#include <string>

//----------------------------------------------------------------------------------------------------------
/// Forward declaration of GLFWindow class
struct GLFWwindow;
//----------------------------------------------------------------------------------------------------------
namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// This is the main class for rendering everything.
	/// Note that it is a fully static class that cannot be constructed
	class Renderer
	{
		friend class Engine;
		/// The current screen dimensions
		inline static Vec2i screenDims{};
		/// The current viewPort
		inline static Vec2i currentViewPort{};
		/// Pointer to the active GLFWwindow
		inline static GLFWwindow* activeWindow = nullptr;
		/// Clear color
		inline static Color3f clearColor{};
	protected:
		/// Initialize the Renderer and creates the window. Returns true on success
		static bool initialize(const int& windowWidth, const int& windowHeight, const std::string& windowName);
		/// Terminates the Renderer and the window
		static void terminate();
	public:
		/// Returns the screen dimensions of the screen that the engine was launched on
		static Vec2i getNativeScreenDimensions();
		/// Returns the current window dimensions
		static Vec2i getScreenDimensions();
		/// Changes the viewport
		static void changeViewPort(const Vec2i& viewPort);
		/// Hides the cursor
		static void hideCursor();
		/// Unhides the cursor
		static void unhideCursor();
		/// Clears the screen
		static void clear();
		/// Sets clear color
		static void setClearColor(const Color3f& clearColor);
		/// swaps renderBuffers. Should be called everytime after Rendering!
		static void swapBuffers();
		/// Enables depth testing
		static void enableDepthTesting();
		/// Disables depth testing
		static void disableDepthTesting();
		/// Enables blending
		static void enableBlending();
		/// Disables blending
		static void disableBlending();
		/// Draws a given model with the given material
		static void draw(const Model& model, const Material& material);
		/// Draws a given model, with shaders, textures etc. already bound
		static void draw(const Model& model);
		/// Deleted constructor: this is a static class!
		Renderer() = delete;
	};
	//------------------------------------------------------------------------------------------------------
}