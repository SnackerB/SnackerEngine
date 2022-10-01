#pragma once

#include <vector>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	struct VertexBufferElement
	{
		/// The type of this vertexBufferElement, can be GL_FLOAT, GL_UNSIGNED_INT, GL_UNSIGNED_BYTE
		unsigned int type;
		/// The number of elements of the above type
		unsigned int count;
		// if GL_TRUE, this maps eg. integers to the range [-1, 1] (or [0, 1] if they're unsigned).
		// for now this is hardcoded to false. TODO: maybe change this in the future!
		unsigned char normalized;

		/// Helper function that can be used to determine the size of a given type
		static unsigned int GetSizeOfType(const unsigned int& type);
	};
	//------------------------------------------------------------------------------------------------------
	class VertexBufferLayout
	{
	private:
		/// Vector of VertexBufferElement objects
		std::vector<VertexBufferElement> elements;
		/// The Stride is the offset of each vertex!
		unsigned int stride;
	public:
		VertexBufferLayout();

		// New data types can be defined in the .cpp file, if this is necessary!
		// Currently supported types: 
		//		float, unsigned int, unsigned char, Vec2<unsigned int>,
		//		Vec3<unsigned int>, Vec2f, Vec3f
		template<typename T>
		void push(unsigned int count);

		const inline std::vector<VertexBufferElement> GetElements() const { return elements; }
		const inline unsigned int GetStride() const { return stride; }

	};
	//------------------------------------------------------------------------------------------------------
}