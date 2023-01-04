#include "Engine.h"
#include "AssetManager/AssetManager.h"
#include "Graphics/Renderer.h"
#include "Core/Log.h"
#include "Core/Timer.h"
#include "Core/Assert.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include <shellscalingapi.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	void Engine::callbackKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (activeScene)
		{
			activeScene->callbackKeyboard(key, scancode, action, mods);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::callbackMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		if (activeScene)
		{
			activeScene->callbackMouseButton(button, action, mods);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::callbackMouseMotion(GLFWwindow* window, double xpos, double ypos)
	{
		if (activeScene)
		{
			activeScene->callbackMouseMotion(Vec2d(xpos, ypos));
		}
		Vec2i screenDims = Renderer::getScreenDimensions();
		if (constrainMouseToScreenCenter) {
			GLCall(glfwSetCursorPos(window, screenDims.x / 2.0f, screenDims.y / 2.0f));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::callbackWindowResize(GLFWwindow* window, int width, int height)
	{
		Renderer::screenDims = Vec2i(width, height);
		Renderer::changeViewPort(Renderer::screenDims);
		if (activeScene)
		{
			activeScene->callbackWindowResize(Vec2i(width, height));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::callbackMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (activeScene)
		{
			activeScene->callbackMouseScroll(Vec2d(xoffset, yoffset));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::callbackCharacterInput(GLFWwindow* window, unsigned int codepoint)
	{
		if (activeScene)
		{
			activeScene->callbackCharacterInput(codepoint);
		}
	}
	//------------------------------------------------------------------------------------------------------
	bool Engine::determineResourcePath(const std::string& resourceFolderPath)
	{
		if (resourceFolderPath != "" && std::filesystem::exists(resourceFolderPath)) 
		{
			resourcePath = resourceFolderPath;
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"" << resourceFolderPath << "\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("resources/"))
		{
			resourcePath = "resources/";
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"./resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../resources"))
		{
			resourcePath = "../resources/";
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../../resources"))
		{
			resourcePath = "../../resources/";
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../../resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../SnackerEngine/resources"))
		{
			resourcePath = "../SnackerEngine/resources/";
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../SnackerEngine/resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../../SnackerEngine/resources"))
		{
			resourcePath = "../../SnackerEngine/resources/";
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../../SnackerEngine/resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../../../SnackerEngine/resources"))
		{
			resourcePath = "../../../SnackerEngine/resources/";
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../../../SnackerEngine/resources/\"" << LOGGER::ENDL;
		}
		else
		{
			errorLogger << LOGGER::BEGIN << "Could not find resource folder!" << LOGGER::ENDL;
			return false;
		}
		return true;
	}
	//------------------------------------------------------------------------------------------------------
	bool Engine::initialize(const int& windowWidth, const int& windowHeight, const std::string& windowName, const std::string& resourceFolderPath)
	{
		// Initialize Renderer class and create window
		if (!Renderer::initialize(windowWidth, windowHeight, windowName))
			return false;
		// Search for resource path
		if (!determineResourcePath(resourceFolderPath))
			return false;
		// Initialize AssetManager class
		AssetManager::initialize();

		// Register GLFW callbacks
		GLCall(glfwSetKeyCallback(Renderer::activeWindow, &callbackKeyboard));
		GLCall(glfwSetMouseButtonCallback(Renderer::activeWindow, &callbackMouseButton));
		GLCall(glfwSetCursorPosCallback(Renderer::activeWindow, &callbackMouseMotion));
		GLCall(glfwSetWindowSizeCallback(Renderer::activeWindow, &callbackWindowResize));
		GLCall(glfwSetScrollCallback(Renderer::activeWindow, &callbackMouseScroll));
		GLCall(glfwSetCharCallback(Renderer::activeWindow, &callbackCharacterInput));

		return true;
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::terminate()
	{
		AssetManager::terminate();
		Renderer::terminate();
	}
	//------------------------------------------------------------------------------------------------------
	const std::string& Engine::getResourcePath()
	{
		return resourcePath;
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::setResourcePath(const std::string& path)
	{
		resourcePath = path;
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::setActiveScene(Scene& scene)
	{
		activeScene = &scene;
		lastUpdateTime = std::chrono::high_resolution_clock::now();
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::startup()
	{
		// Create timer for capping draw fps TODO: Make this variable
		Timer drawTimer((unsigned int)0);
		lastUpdateTime = std::chrono::high_resolution_clock::now();
		// Main loop
		while (!glfwWindowShouldClose(Renderer::activeWindow))
		{
			// Compute time since last update
			auto now = std::chrono::high_resolution_clock::now();
			long long microseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastUpdateTime).count();
			double dt = (double(microseconds) / 1'000'000'000.0f);
			
			// Update active scene
			activeScene->update(dt);
			
			// Render scene!
			auto timerResult = drawTimer.tick(dt);
			if (timerResult.first) {
				Renderer::clear();
				activeScene->draw();
				Renderer::swapBuffers();
			}

			// Poll for and process events
			GLCall(glfwPollEvents());

			// Update the lastUpdateTime
			lastUpdateTime = now;
		}
	}
	//------------------------------------------------------------------------------------------------------
	Vec2i Engine::constrainMouseCenter(const bool& enable)
	{
		constrainMouseToScreenCenter = enable;
		Vec2i mousePos = Renderer::screenDims / 2;
		glfwSetCursorPos(Renderer::activeWindow, mousePos.x, mousePos.y);
		return mousePos;
	}
	//------------------------------------------------------------------------------------------------------
	Vec2<unsigned int> Engine::getDPI()
	{
		Vec2<unsigned int> result{};
		HRESULT temp = GetDpiForMonitor(
			MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY),
			MDT_EFFECTIVE_DPI,
			&result.x,
			&result.y
		);
		if (temp != S_OK) {
			warningLogger << LOGGER::BEGIN << "Was unable to determine monitor DPI. Using default DPI." << LOGGER::ENDL;
			return Vec2<unsigned int>(10, 10);
		}
		return result;
	}
	//------------------------------------------------------------------------------------------------------
}

