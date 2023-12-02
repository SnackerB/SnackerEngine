#include "Engine.h"
#include "AssetManager/AssetManager.h"
#include "Graphics/Renderer.h"
#include "Core/Log.h"
#include "Utility\Timer.h"
#include "Core/Assert.h"
#include "Gui/GuiManager.h"
#include "Utility\Random.h"

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
			resourcePaths = { resourceFolderPath };
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"" << resourceFolderPath << "\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("resources"))
		{
			resourcePaths = { "resources/" };
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"./resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../resources"))
		{
			resourcePaths = { "../resources/" };
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../../resources"))
		{
			resourcePaths = { "../../resources/" };
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../../resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../SnackerEngine/resources"))
		{
			resourcePaths = { "../SnackerEngine/resources/" };
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../SnackerEngine/resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../../SnackerEngine/resources"))
		{
			resourcePaths = { "../../SnackerEngine/resources/" };
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../../SnackerEngine/resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../../../SnackerEngine/resources"))
		{
			resourcePaths = { "../../../SnackerEngine/resources/" };
			infoLogger << LOGGER::BEGIN << "Resource path relative to executable: \"../../../SnackerEngine/resources/\"" << LOGGER::ENDL;
		}
		else if (std::filesystem::exists("../../../SnackerEngine/SnackerEngine/resources"))
		{
			resourcePaths = { "../../../SnackerEngine/SnackerEngine/resources/" };
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
		// Seed RNG
		initializeRNG();
		// Initialize Renderer class and create window
		if (!Renderer::initialize(windowWidth, windowHeight, windowName))
			return false;
		// Search for resource path
		if (!determineResourcePath(resourceFolderPath))
			return false;
		// Initialize AssetManager class
		AssetManager::initialize();
		/// Initialize GuiManager
		GuiManager::initialize();

		// Register GLFW callbacks
		GLCall(glfwSetKeyCallback(Renderer::activeWindow, &callbackKeyboard));
		GLCall(glfwSetMouseButtonCallback(Renderer::activeWindow, &callbackMouseButton));
		GLCall(glfwSetCursorPosCallback(Renderer::activeWindow, &callbackMouseMotion));
		GLCall(glfwSetWindowSizeCallback(Renderer::activeWindow, &callbackWindowResize));
		GLCall(glfwSetScrollCallback(Renderer::activeWindow, &callbackMouseScroll));
		GLCall(glfwSetCharCallback(Renderer::activeWindow, &callbackCharacterInput));

		// Initialize random engine
		auto rd = std::random_device{};
		Engine::randomEngine = std::default_random_engine{ rd() };

		return true;
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::terminate()
	{
		GuiManager::terminate();
		AssetManager::terminate();
		//NetworkManager::cleanup();
		Renderer::terminate();
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
		running = true;
		while (!glfwWindowShouldClose(Renderer::activeWindow) && running)
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
	void Engine::stopEngine()
	{
		running = false;
	}
	//------------------------------------------------------------------------------------------------------
	void Engine::addResourceFolderPath(const std::string& path)
	{
		resourcePaths.push_back(path);
	}
	//------------------------------------------------------------------------------------------------------
	std::optional<std::string> Engine::getFullPath(const std::string& path)
	{
		for (const auto& resourcePath : resourcePaths) {
			std::string fullPath = resourcePath + path;
			if (std::filesystem::exists(fullPath))
				return fullPath;
		}
		if (std::filesystem::exists(path))
			return path;
		return {};
	}
	//------------------------------------------------------------------------------------------------------
	const std::string& Engine::getDefaultResourcePath()
	{
		return Engine::resourcePaths.empty() ? "" : Engine::resourcePaths[0];
	}
	//------------------------------------------------------------------------------------------------------
	std::optional<Buffer> Engine::loadFileRelativeToResourcePath(const std::string& path)
	{
		for (const std::string& resourcePath : resourcePaths) {
			std::string fullPath = resourcePath + path;
			if (std::filesystem::exists(fullPath)) {
				return Buffer::loadFromFile(fullPath);
			}
		}
		warningLogger << LOGGER::BEGIN << "Could not find file \"" << path << "\" relative to resource folders." << LOGGER::ENDL;
		return std::nullopt;
	}
	//------------------------------------------------------------------------------------------------------
	std::optional<nlohmann::json> Engine::loadJSONRelativeToResourcePath(const std::string& path)
	{
		for (const std::string& resourcePath : resourcePaths) {
			std::string fullPath = resourcePath + path;
			if (std::filesystem::exists(fullPath)) {
				try
				{
					return loadJSON(fullPath);
				}
				catch (const std::exception& e)
				{
					warningLogger << LOGGER::BEGIN << e.what() << LOGGER::ENDL;
					return std::nullopt;
				}
			}
		}
		warningLogger << LOGGER::BEGIN << "Could not find file \"" << path << "\" relative to resource folders." << LOGGER::ENDL;
		return std::nullopt;
	}
	//------------------------------------------------------------------------------------------------------
	bool Engine::saveJSONRelativeToResourcePath(const nlohmann::json& json, const std::string& path)
	{
		for (const std::string& resourcePath : resourcePaths) {
			std::string fullPath = resourcePath + path;
			if (std::filesystem::exists(fullPath)) {
				try
				{
					saveJSON(json, fullPath);
					return true;
				}
				catch (const std::exception&) 
				{
					return false;
				}
			}
		}
		if (!resourcePaths.empty()) {
			std::string fullPath = resourcePaths.front() + path;
			try
			{
				saveJSON(json, fullPath);
				return true;
			}
			catch (const std::exception&) 
			{
				return false;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------
}

