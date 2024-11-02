#include "Graphics/VertexBuffer.h"
#include "Core/Log.h"
#include "Core/Assert.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	void VertexBuffer::setDataAndFinalize(void* data, const unsigned int& size)
	{
		// Generate one buffer and set the ID to m_RendererID.
		GLCall(glGenBuffers(1, &GPU_ID));
		// Bind the buffer. GL_ARRAY_BUFFER is used for vertex data!
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, GPU_ID));
		// glBufferData creates a new data store for the buffer object currently bound to target. 
		// Any pre-existing data store is deleted.
		// - usage is a hint to the GL implementation as to how a buffer object's data store will be accessed. 
		// This enables the GL implementation to make more intelligent decisions that may significantly 
		// impact buffer object performance. It does not, however, constrain the actual usage of the data store. 
		// usage can be broken down into two parts: first, the frequency of access (modification and usage), 
		// and second, the nature of that access. The frequency of access may be one of these:
		// STREAM : The data store contents will be modified once and used at most a few times.
		// STATIC : The data store contents will be modified once and used many times.
		// DYNAMIC : The data store contents will be modified repeatedly and used many times.
		// The nature of access must be:
		// DRAW : The data store contents are modified by the application, 
		// and used as the source for GL drawing and image specification commands.
		// TODO: add more options (not just static and dynamic draw!)
		if (storageType == VertexBufferStorageType::STATIC)
		{
			GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
		}
		else if (storageType == VertexBufferStorageType::DYNAMIC)
		{
			GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
		}
		else
		{
			warningLogger << LOGGER::BEGIN << "Used unknown storage type in VertexBuffer creation" << LOGGER::ENDL;
			return;
		}
		valid = true;
		sizeInBytes = size;
	}
	//------------------------------------------------------------------------------------------------------
	void VertexBuffer::changeSubData(void* data, const unsigned int& size, const unsigned int& offset)
	{
		if (valid) {
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, GPU_ID));
			GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
		}
		else {
			warningLogger << LOGGER::BEGIN << "Tried to bind vertex buffer which has no data on the GPU!" << LOGGER::ENDL;
		}
#ifdef _DEBUG
		unBind();
#endif // _DEBUG
	}
	//------------------------------------------------------------------------------------------------------
	VertexBuffer::VertexBuffer(const VertexBufferStorageType storageType)
		: valid(false), GPU_ID(0), storageType(storageType), sizeInBytes(0) {}
	//------------------------------------------------------------------------------------------------------
	VertexBuffer::VertexBuffer()
		: valid(false), GPU_ID(0), storageType(VertexBufferStorageType::DYNAMIC), sizeInBytes(0) {}
	//------------------------------------------------------------------------------------------------------
	VertexBuffer::VertexBuffer(VertexBuffer&& vb) noexcept
		: valid(vb.valid), GPU_ID(vb.GPU_ID), storageType(vb.storageType), sizeInBytes(vb.sizeInBytes)
	{
		// Reset the other vb
		vb.valid = false;
		vb.GPU_ID = 0;
		vb.storageType = VertexBufferStorageType::DYNAMIC;
		vb.sizeInBytes = 0;
	}
	//------------------------------------------------------------------------------------------------------
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vb) noexcept
	{
		this->~VertexBuffer();
		valid = vb.valid;
		GPU_ID = vb.GPU_ID;
		storageType = vb.storageType;
		sizeInBytes = vb.sizeInBytes;
		// Reset the other vb
		vb.valid = false;
		vb.GPU_ID = 0;
		vb.storageType = VertexBufferStorageType::DYNAMIC;
		vb.sizeInBytes = 0;
		// Return reference to self
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	VertexBuffer::~VertexBuffer()
	{
		// glDeleteBuffers deletes n buffer objects named by the elements of the array buffers. 
		// After a buffer object is deleted, it has no contents, and its name is free for reuse 
		// (for example by glGenBuffers). 
		// If a buffer object that is currently bound is deleted, the binding reverts to 0 
		// (the absence of any buffer object, which reverts to client memory usage).
		// glDeleteBuffers silently ignores 0's and names that do not correspond to existing buffer objects.
		if (valid) {
			GLCall(glDeleteBuffers(1, &GPU_ID));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void VertexBuffer::bind()
	{
		if (valid) {
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, GPU_ID));
		}
		else {
			warningLogger << LOGGER::BEGIN << "Tried to bind vertex buffer which has no data on the GPU!" << LOGGER::ENDL;
		}
	}
	//------------------------------------------------------------------------------------------------------
	void VertexBuffer::unBind()
	{
		// Unbind all Buffers!
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	//------------------------------------------------------------------------------------------------------
	void VertexBuffer::setStorageType(VertexBufferStorageType storageType)
	{
		if (valid) {
			warningLogger << LOGGER::BEGIN << "Tried to change storage type of vertex buffer whis already has data on GPU!" << LOGGER::ENDL;
		}
		else {
			this->storageType = storageType;
		}
	}
	//------------------------------------------------------------------------------------------------------
}