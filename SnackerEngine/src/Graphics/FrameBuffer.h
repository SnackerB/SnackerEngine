#pragma once

#include "Math/Vec.h"
#include "Graphics/Texture.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Frame buffers can be used as render targets, in which case everything is rendered to a texture.
	/// This texture can later be read from.
	class FrameBuffer
	{
	private:
		unsigned int GPU_ID;
		unsigned int renderBufferID;
		Vec2i dimensions;
		Texture texture; // TODO: Add support for using multiple textures!
		bool valid;
		bool doDepthTesting;
		bool doStencilTesting;
	public:
		/// Default Constructor
		FrameBuffer();
		/// Constructor specifying dimensions of the FrameBuffer object and optionally specifying if depth and stencil testing are done
		FrameBuffer(const Vec2i& dimensions, const bool& doDepthTesting = true, const bool& doStencilTesting = false);
		/// Setters (should only be used before calling finalize())
		void setDoDepthTesting(const bool& doDepthTesting);
		void setDoStencilTesting(const bool& doStencilTesting);
		void setScreenDimensions(const Vec2i& dimensions);
		void attachTexture(const Texture& texture);
		/// Call this function after setting up all flags and textures
		void finalize();
		/// Destructor
		~FrameBuffer();
		//==================================================================================================
		/// The following functions should only be used after finalizing the framebuffer.
		//==================================================================================================
		/// Binds the frameBuffer as active render target
		void bindAsTarget();
		/// Unbinds all frameBuffers, rendering is done to the default frameBuffer
		void unbindAllTargets();
		/// Binds the FrameBuffer texture to the given slot
		void bindTexture(const unsigned int& slot = 0);
		/// Returns the FrameBuffer texture
		const Texture& getTexture() { return texture; }
		/// Resizes the FrameBuffer
		void resize(const Vec2i& dimensions);
	};
	//--------------------------------------------------------------------------------------------------
}