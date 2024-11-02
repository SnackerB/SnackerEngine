#include "Graphics/IndexBuffer.h"
#include "Core/Assert.h"
#include "Core/Log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	IndexBuffer::IndexBuffer()
		: valid(false), GPU_ID(0), indexCount(0) {}
	//------------------------------------------------------------------------------------------------------
	IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices)
		: IndexBuffer()
	{
		setIndices(indices);
	}
	//------------------------------------------------------------------------------------------------------
	IndexBuffer::IndexBuffer(IndexBuffer&& ib) noexcept
		: valid(ib.valid), GPU_ID(ib.GPU_ID), indexCount(ib.indexCount)
	{
		// Reset other ib
		ib.valid = false;
		ib.GPU_ID = 0;
		ib.indexCount = 0;
	}
	//------------------------------------------------------------------------------------------------------
	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ib) noexcept
	{
		this->~IndexBuffer();
		valid = ib.valid;
		GPU_ID = ib.GPU_ID;
		indexCount = ib.indexCount;
		// Reset other ib
		ib.valid = false;
		ib.GPU_ID = 0;
		ib.indexCount = 0;
		// Returns reference to self
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	IndexBuffer::~IndexBuffer()
	{
		if (valid) {
			GLCall(glDeleteBuffers(1, &GPU_ID));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void IndexBuffer::bind() const
	{
		if (valid) {
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GPU_ID));
		}
		else {
			warningLogger << LOGGER::BEGIN << "Tried to bind indexBuffer which was not initialized!" << LOGGER::ENDL;
		}
	}
	//------------------------------------------------------------------------------------------------------
	void IndexBuffer::unBind()
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	//------------------------------------------------------------------------------------------------------
	void IndexBuffer::setIndices(const std::vector<unsigned int>& indices)
	{
		if (valid) {
			warningLogger << LOGGER::BEGIN << "tried to set indices of already valid indexBuffer!" << LOGGER::ENDL;
			return;
		}
#ifdef _DEBUG
		ASSERT(sizeof(unsigned int) == sizeof(GLuint));
#endif // _DEBUG

		GLCall(glGenBuffers(1, &GPU_ID));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GPU_ID));
		// GL_ELEMENT_ARRAY_BUFFER for indices. STATIC_DRAW: see vertexBuffer header!
		// TODO: maybe add more allowed modes (like DYNAMIC_DRAW)
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
		valid = true;
		indexCount = static_cast<unsigned int>(indices.size());
	}
	//------------------------------------------------------------------------------------------------------
}