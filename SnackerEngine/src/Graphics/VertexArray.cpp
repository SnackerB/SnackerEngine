#include "Graphics/VertexArray.h"
#include "Core/Assert.h"
#include "Core/Log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	VertexArray::VertexArray()
		: valid(false), GPU_ID(0), layout{} {}
	//------------------------------------------------------------------------------------------------------
	VertexArray::VertexArray(const VertexBufferLayout& layout)
		: valid(false), GPU_ID(0), layout(layout) {}
	//------------------------------------------------------------------------------------------------------
	VertexArray::VertexArray(const VertexBufferLayout& layout, const VertexBuffer& vb)
		: valid(false), GPU_ID(0), layout(layout)
	{
		bindBuffer(vb);
	}
	//------------------------------------------------------------------------------------------------------
	VertexArray::VertexArray(VertexArray&& va) noexcept
		: valid(va.valid), GPU_ID(va.GPU_ID), layout(va.layout)
	{
		// Reset the other va
		va.valid = false;
		va.GPU_ID = 0;
		va.layout = {};
	}
	//------------------------------------------------------------------------------------------------------
	VertexArray& VertexArray::operator=(VertexArray&& va) noexcept
	{
		this->~VertexArray();
		valid = va.valid;
		GPU_ID = va.GPU_ID;
		layout = va.layout;
		// Reset the other va
		va.valid = false;
		va.GPU_ID = 0;
		va.layout = {};
		// Return reference to self
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	VertexArray::~VertexArray()
	{
		if (valid) {
			GLCall(glDeleteVertexArrays(1, &GPU_ID));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void VertexArray::bind() const
	{
		if (valid) {
			GLCall(glBindVertexArray(GPU_ID));
		}
		else {
			warningLogger << LOGGER::BEGIN << "Tried to bind Vertex Array that was invalid!" << LOGGER::ENDL;
		}
	}
	//------------------------------------------------------------------------------------------------------
	void VertexArray::unbind()
	{
		GLCall(glBindVertexArray(0));
	}
	//------------------------------------------------------------------------------------------------------
	void VertexArray::bindBuffer(const VertexBuffer& buffer)
	{
		if (valid) {
			warningLogger << LOGGER::BEGIN << "Tried to bind buffer to a vertexArray that was already initialized!" << LOGGER::ENDL;
			return;
		}
		// Generate new vertexArray
		GLCall(glGenVertexArrays(1, &GPU_ID));
		// Bind it
		GLCall(glBindVertexArray(GPU_ID));
		// go through layout and call VertexAttribPointer with the correct parameters
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			GLCall(glEnableVertexAttribArray(i));
			GLCall(glVertexAttribPointer(i, element.count, element.type,
				element.normalized, layout.GetStride(), reinterpret_cast<const void*>(offset)));
			offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		}
		GLCall(glEnableVertexAttribArray(0));
		valid = true;
	}
	//------------------------------------------------------------------------------------------------------
	void VertexArray::bindBuffer(const VertexBuffer& buffer, VertexBufferLayout layout)
	{
		this->layout = layout;
		bindBuffer(buffer);
	}
	//------------------------------------------------------------------------------------------------------
}