#include "Graphics/FrameBuffer.h"
#include "Core/Log.h"
#include "Core/Assert.h"
#include "AssetManager/TextureManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	FrameBuffer::FrameBuffer()
		: GPU_ID(0), renderBufferID(0), dimensions(0), texture(Texture()), valid(false), doDepthTesting(true), doStencilTesting(false) {}
	//--------------------------------------------------------------------------------------------------
	FrameBuffer::FrameBuffer(const Vec2i& dimensions, const bool& doDepthTesting, const bool& doStencilTesting)
		: GPU_ID(0), renderBufferID(0), dimensions(dimensions), texture(Texture()), valid(false), doDepthTesting(doDepthTesting), doStencilTesting(doStencilTesting) {}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::setDoDepthTesting(const bool& doDepthTesting)
	{
#ifdef _DEBUG
		if (valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't change framebuffer options when already initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		this->doDepthTesting = doDepthTesting;
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::setDoStencilTesting(const bool& doStencilTesting)
	{
#ifdef _DEBUG
		if (valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't change framebuffer options when already initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		this->doStencilTesting = doStencilTesting;
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::setScreenDimensions(const Vec2i& dimensions)
	{
#ifdef _DEBUG
		if (valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't change framebuffer options when already initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		this->dimensions = dimensions;
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::attachTexture(const Texture& texture)
	{
#ifdef _DEBUG
		if (valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't change framebuffer options when already initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		this->texture = texture;
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::finalize()
	{
#ifdef _DEBUG
		if (valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't finalize framebuffer when already initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		if (!texture.isValid())
		{
			warningLogger << LOGGER::BEGIN << "textures have to be initialized before using with a framebuffer!" << LOGGER::ENDL;
			return;
		}
		// First, generate the frame buffer
		GLCall(glGenFramebuffers(1, &GPU_ID));
		// bind it
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, GPU_ID));
		//Create RenderBuffer Object for Depth and/or Stencil Testing
		if (doDepthTesting || doStencilTesting)
		{
			GLCall(glGenRenderbuffers(1, &renderBufferID));
			GLCall(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID));
			if (doDepthTesting && !doStencilTesting)
			{
				GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions.x, dimensions.y));
				GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferID));
			}
			else if (doDepthTesting && doStencilTesting)
			{
				GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dimensions.x, dimensions.y));
				GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID));
			}
			else if (!doDepthTesting && doStencilTesting)
			{
				GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, dimensions.x, dimensions.y));
				GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID));
			}
		}
		// Bind texture
		switch (TextureManager::getTextureData(texture).textureType)
		{
		case Texture::TextureType::CUBEMAP:
		{
			for (unsigned int i = 0; i < 6; ++i)
			{
				GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TextureManager::getTextureData(texture).GPU_ID, 0));
			}
			break;
		}
		default:
		{
			GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureManager::getTextureData(texture).GPU_ID, 0));
			break;
		}
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			warningLogger << LOGGER::BEGIN << "Framebuffer is not complete!" << LOGGER::ENDL;
		}
		else
		{
			valid = true;
		}
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	//--------------------------------------------------------------------------------------------------
	FrameBuffer::~FrameBuffer()
	{
		if (valid)
		{
			if (doDepthTesting || doStencilTesting)
			{
				GLCall(glDeleteRenderbuffers(1, &renderBufferID));
			}
			GLCall(glDeleteFramebuffers(1, &GPU_ID));
		}
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::bindAsTarget()
	{
#ifdef _DEBUG
		if (!valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't use framebuffer when not initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, GPU_ID));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID));
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::unbindAllTargets()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::bindTexture(const unsigned int& slot)
	{
#ifdef _DEBUG
		if (!valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't use framebuffer when not initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		texture.bind(slot);
	}
	//--------------------------------------------------------------------------------------------------
	void FrameBuffer::resize(const Vec2i& dimensions)
	{
#ifdef _DEBUG
		if (!valid)
		{
			warningLogger << LOGGER::BEGIN << "Can't use framebuffer when not initialized!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		this->dimensions = dimensions;
		// Delete old frame- and renderBuffer
		if (doDepthTesting || doStencilTesting)
		{
			GLCall(glDeleteRenderbuffers(1, &renderBufferID));
		}
		GLCall(glDeleteFramebuffers(1, &GPU_ID));
		// And generate new one
		finalize();
	}
	//--------------------------------------------------------------------------------------------------
}