#pragma once

#include "graphics/vertexBufferLayout.h"
#include "graphics/vertexBuffer.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class VertexArray {
		/// This is set to true if this VertexArray object is valid and corresponds to data on the GPU
		bool valid;
		/// GPU ID of this VertexArray object
		unsigned int GPU_ID;
		/// The layout of this VertexArray object
		VertexBufferLayout layout;
	public:
		/// Default constructor. A buffer and layout should be bound to finalize the VertexArray object before using it
		VertexArray();
		/// Contructor using a layout. A buffer should be bound to finalize the VertexArray object before using it
		VertexArray(const VertexBufferLayout& layout);
		/// Constructor using a layout and a buffer. Will directly be valid and usable!
		VertexArray(const VertexBufferLayout& layout, const VertexBuffer& vb);
		/// Move constructor
		VertexArray(VertexArray&& va) noexcept;
		/// Deleted copy constructor
		VertexArray(VertexArray& va) = delete;
		/// Move assignment operator
		VertexArray& operator=(VertexArray&& va) noexcept;
		/// Deleted copy assignment operator
		VertexArray& operator=(VertexArray& va) = delete; 
		/// Destructor
		~VertexArray();
		/// Binds this VertexArray object
		void bind() const;
		/// Unbinds all VertexArray objects
		static void unbind();
		/// Binds a buffer to this VertexArray object and finalizes it. A layout should already be specified
		void bindBuffer(const VertexBuffer& buffer);
		/// Specifies a layout and binds a buffer, finalizing this VertexArray object
		void bindBuffer(const VertexBuffer& buffer, VertexBufferLayout layout);

		bool isValid() { return valid; };
		const VertexBufferLayout& getLayout() { return layout; };
	};
	//------------------------------------------------------------------------------------------------------
}