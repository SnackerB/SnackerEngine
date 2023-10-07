#include "Graphics/VertexBufferLayout.h"
#include "Math/Mat.h"
#include "Core/Assert.h"

#include <GL/glew.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	unsigned int VertexBufferElement::GetSizeOfType(const unsigned int& type)
	{
		switch (type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		default:
			ASSERT(false);
			break;
		}
		return 0;
	}
	//------------------------------------------------------------------------------------------------------
	VertexBufferLayout::VertexBufferLayout()
		: stride(0) {}
	//------------------------------------------------------------------------------------------------------
	template<>
	void VertexBufferLayout::push<float>(unsigned int count)
	{
		elements.push_back({ GL_FLOAT, count, GL_FALSE });
		stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void VertexBufferLayout::push<unsigned int>(unsigned int count)
	{
		elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	// unsigned chars are bytes!
	void VertexBufferLayout::push<unsigned char>(unsigned int count)
	{
		elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	// unsigned chars are bytes!
	void VertexBufferLayout::push<Vec2<unsigned int>>(unsigned int count)
	{
		elements.push_back({ GL_UNSIGNED_INT, count * 2, GL_TRUE });
		stride += count * 2 * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	// unsigned chars are bytes!
	void VertexBufferLayout::push<Vec3<unsigned int>>(unsigned int count)
	{
		elements.push_back({ GL_UNSIGNED_INT, count * 3, GL_TRUE });
		stride += count * 3 * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	// unsigned chars are bytes!
	void VertexBufferLayout::push<Vec2f>(unsigned int count)
	{
		elements.push_back({ GL_FLOAT, count * 2, GL_TRUE });
		stride += count * 2 * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	// unsigned chars are bytes!
	void VertexBufferLayout::push<Vec3f>(unsigned int count)
	{
		elements.push_back({ GL_FLOAT, count * 3, GL_TRUE });
		stride += count * 3 * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	//------------------------------------------------------------------------------------------------------
}