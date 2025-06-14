#include "Renderer.h"
#include "Core/Log.h"
#include "Core/Assert.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	bool Renderer::initialize(const int& windowWidth, const int& windowHeight, const std::string& windowName)
	{
		// Initialize the library 
		if (!glfwInit())
		{
			errorLogger << LOGGER::BEGIN << "GLFW could not be initialized!" << LOGGER::ENDL;
			return false;
		}
		// Clip window to native screen dimensions if it is too large
		screenDims = getNativeScreenDimensions();
		if (screenDims.x > windowWidth)
			screenDims.x = windowWidth;
		if (screenDims.y > windowHeight)
			screenDims.y = windowHeight;
		// Multisampling Antialiasing
		glfwWindowHint(GLFW_SAMPLES, 4);
		activeWindow = glfwCreateWindow(screenDims.x, screenDims.y, windowName.c_str(), NULL, NULL);
		// Enable Multisampling
		glEnable(GL_MULTISAMPLE); 
		// Check if window was created successfully
		if (!activeWindow)
		{
			errorLogger << LOGGER::BEGIN << "Window could not be created!" << LOGGER::ENDL;
			glfwTerminate();
			return false;
		}
		// Make the window's context current
		glfwMakeContextCurrent(activeWindow);

		// Initialize glew
		if (glewInit() != GLEW_OK)
		{
			errorLogger << LOGGER::BEGIN << "Glew could not be initialized!" << LOGGER::ENDL;
			glfwTerminate();
			return false;
		}

		// Print out version
		infoLogger << LOGGER::BEGIN << "Running on GL version " << glGetString(GL_VERSION) << LOGGER::ENDL;

		// Enable depth testing
		GLCall(glEnable(GL_DEPTH_TEST));

		// Make cubemaps seemless
		GLCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

		// Enable blending
		enableBlending();

		// Successfully initialized!
		return true;
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::terminate()
	{
		glfwTerminate();
	}
	//------------------------------------------------------------------------------------------------------
	Vec2i Renderer::getNativeScreenDimensions()
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return Vec2i(mode->width, mode->height);
	}
	//------------------------------------------------------------------------------------------------------
	Vec2i Renderer::getScreenDimensions()
	{
		return screenDims;
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::changeViewPort(const Vec2i& viewPort)
	{
		currentViewPort = viewPort;
		GLCall(glViewport(GLint(0), GLint(0), static_cast<GLint>(viewPort.x), static_cast<GLint>(viewPort.y)));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::hideCursor()
	{
		GLCall(glfwSetInputMode(activeWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::unhideCursor()
	{
		glfwSetInputMode(activeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::clear()
	{
		GLCall(glClearColor(static_cast<GLclampf>(clearColor.r), static_cast<GLclampf>(clearColor.g), static_cast<GLclampf>(clearColor.b), 1.0f));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //  | GL_STENCIL_BUFFER_BIT); // TODO: Stencil buffer
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::setClearColor(const Color3f& clearColor)
	{
		Renderer::clearColor = clearColor;
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::swapBuffers()
	{
		GLCall(glfwSwapBuffers(activeWindow));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::enableDepthTesting()
	{
		GLCall(glEnable(GL_DEPTH_TEST));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::disableDepthTesting()
	{
		GLCall(glDisable(GL_DEPTH_TEST));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::enableBlending()
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // TODO: Make this variable!
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::disableBlending()
	{
		GLCall(glDisable(GL_BLEND));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::enableScissorTest(const Vec2i& position, const Vec2i& size)
	{
		GLCall(glEnable(GL_SCISSOR_TEST));
		GLCall(glScissor(position.x, position.y, size.x, size.y));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::enableScissorTest(const Vec4i& clippingBox)
	{
		GLCall(glEnable(GL_SCISSOR_TEST));
		GLCall(glScissor(clippingBox.x, clippingBox.y, clippingBox.z, clippingBox.w));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::disableScissorTest()
	{
		GLCall(glDisable(GL_SCISSOR_TEST));
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::draw(const Model& model, const Material& material)
	{
		material.bind();
		for (const auto& mesh : model.getMeshes()) {
			mesh.getVertexArray().bind();
			const IndexBuffer& ib = mesh.getIndexBuffer();
			ib.bind();
			// TODO: maybe add other draw modes than Triangles!
			GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::draw(const Model& model)
	{
		for (const auto& mesh : model.getMeshes()) {
			mesh.getVertexArray().bind();
			const IndexBuffer& ib = mesh.getIndexBuffer();
			ib.bind();
			// TODO: maybe add other draw modes than Triangles!
			GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Renderer::setCursorShape(const CursorShape& cursorShape)
	{
		switch (cursorShape)
		{
		case CursorShape::DEFAULT:
		{
			GLCall(glfwSetCursor(activeWindow, NULL));
			return;
		}
		case CursorShape::ARROW:
		{
			GLCall(currentCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
			break;
		}
		case CursorShape::IBEAM:
		{
			GLCall(currentCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR));
			break;
		}
		case CursorShape::CROSSHAIR:
		{
			GLCall(currentCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
			break;
		}
		case CursorShape::HAND:
		{
			GLCall(currentCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR));
			break;
		}
		case CursorShape::HRESIZE:
		{
			GLCall(currentCursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
			break;
		}
		case CursorShape::VRESIZE:
		{
			GLCall(currentCursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
			break;
		}
		default:
			break;
		}
		GLCall(glfwSetCursor(activeWindow, currentCursor));
	}
	//------------------------------------------------------------------------------------------------------
}
