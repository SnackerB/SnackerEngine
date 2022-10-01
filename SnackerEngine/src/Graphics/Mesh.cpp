#include "Graphics/Mesh.h"
#include "AssetManager/MeshManager.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	Mesh::Mesh(MeshID meshID)
		: meshID(meshID) 
	{
		MeshManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Mesh::Mesh()
		: meshID(0) {}
	//------------------------------------------------------------------------------------------------------
	Mesh& Mesh::operator=(const Mesh& other) noexcept
	{
		MeshManager::decreaseReferenceCount(*this);
		meshID = other.meshID;
		MeshManager::increaseReferenceCount(*this);
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	Mesh::Mesh(const Mesh& other) noexcept
		: meshID(other.meshID)
	{
		MeshManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	bool Mesh::isValid() const
	{
		return MeshManager::getMeshData(*this).valid;
	}
	//------------------------------------------------------------------------------------------------------
	const VertexArray& Mesh::getVertexArray() const
	{
		return MeshManager::getMeshData(*this).va;
	}
	//------------------------------------------------------------------------------------------------------
	const IndexBuffer& Mesh::getIndexBuffer() const
	{
		return MeshManager::getMeshData(*this).ib;
	}

	VertexBuffer& Mesh::getVertexBuffer() const
	{
		return MeshManager::getMeshData(*this).vb;
	}
	//------------------------------------------------------------------------------------------------------
	Mesh::~Mesh()
	{
		MeshManager::decreaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
}