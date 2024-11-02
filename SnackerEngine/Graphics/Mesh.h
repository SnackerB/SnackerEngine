#pragma once

#include "Graphics/VertexArray.h"
#include "Graphics/IndexBuffer.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class Mesh
	{
	private:
		using MeshID = unsigned int;
		friend class MeshManager;
		MeshID meshID;
		Mesh(MeshID meshID);
	public:
		Mesh();
		// Copy constructor and assignment operator
		Mesh& operator=(const Mesh& other) noexcept;
		explicit Mesh(const Mesh& other) noexcept;
		/// Returns the true if this Mesh object is valid and corresponds to data stored on the GPU
		bool isValid() const;
		/// Returns the VertexArray object of this mesh
		const VertexArray& getVertexArray() const;
		/// Returns the IndexBuffer object of this mesh
		const IndexBuffer& getIndexBuffer() const;
		/// Returns the VertexBuffer object of this mesh
		VertexBuffer& getVertexBuffer() const;
		// Destructor
		~Mesh();
	};
	//------------------------------------------------------------------------------------------------------
}