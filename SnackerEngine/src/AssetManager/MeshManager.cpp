#include "AssetManager/MeshManager.h"
#include "Core/Log.h"
#include "Core/Assert.h"
#include "Graphics/Meshes/Cube.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	MeshManager::MeshData::~MeshData() {}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::MeshData::bind() const
	{
		va.bind();
		ib.bind();
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::MeshData::finalize()
	{
		if (valid) {
			warningLogger << LOGGER::BEGIN << "Tried to finalize a mesh that is already finalized!" << LOGGER::ENDL;
			return;
		}
		if (vb.isValid() && !va.isValid()) {
			va.bindBuffer(vb);
		}
		if (vb.isValid() && va.isValid() && ib.isValid()) valid = true;
		else {
			if (!vb.isValid()) {
				warningLogger << LOGGER::BEGIN << "Trid to finalize mesh with invalid vertexBuffer!" << LOGGER::ENDL;
			}
			if (!va.isValid()) {
				warningLogger << LOGGER::BEGIN << "Trid to finalize mesh with invalid vertexArray!" << LOGGER::ENDL;
			}
			if (!ib.isValid()) {
				warningLogger << LOGGER::BEGIN << "Trid to finalize mesh with invalid indexBuffer!" << LOGGER::ENDL;
			}
		}
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::deleteMesh(const MeshID& meshID)
	{
#ifdef _DEBUG
		if (meshID > maxMeshes) {
			warningLogger << LOGGER::BEGIN << "tried to delete mesh that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!meshDataArray[meshID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to delete mesh that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG

		// Reset and unload mesh
		meshDataArray[meshID] = std::move(MeshData());
		loadedMeshesCount--;
		availableMeshIDs.push(meshID);
	}
	//------------------------------------------------------------------------------------------------------
	MeshManager::MeshID MeshManager::getNewMeshID()
	{
		if (loadedMeshesCount >= maxMeshes)
		{
			// Resize vector and add new available meshID slots accordingly. For now: double size everytime this happens and send warning!
			meshDataArray.resize(maxMeshes * 2 + 1);
			for (MeshID id = maxMeshes + 1; id <= 2 * maxMeshes; ++id)
			{
				availableMeshIDs.push(id);
			}
			warningLogger << LOGGER::BEGIN << "maximum amount of meshes exceeded. Resizing meshManager to be able to store "
				<< 2 * maxMeshes << " meshes!" << LOGGER::ENDL;
			maxMeshes *= 2;
		}
		// Take ID from the front of the queue
		MeshID id = availableMeshIDs.front();
		availableMeshIDs.pop();
		++loadedMeshesCount;
		return id;
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::initialize(const unsigned int& startingSize)
	{
		// Initializes queue with all possible meshIDs. meshID = 0 is reserved for invalid meshes.
		for (MeshID id = 1; id <= startingSize; ++id)
		{
			availableMeshIDs.push(id);
		}
		// meshDataArray[0] stores the default/invalid mesh. TODO: Maybe add default mesh?
		meshDataArray.resize(startingSize + 1);
		maxMeshes = startingSize;
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::increaseReferenceCount(const Mesh& mesh)
	{
		if (mesh.meshID == 0)
			return;
#ifdef _DEBUG
		if (mesh.meshID > maxMeshes) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of mesh that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!meshDataArray[mesh.meshID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of mesh that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		meshDataArray[mesh.meshID].referenceCount++;
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::decreaseReferenceCount(const Mesh& mesh)
	{
		if (mesh.meshID == 0)
			return;
#ifdef _DEBUG
		if (mesh.meshID > maxMeshes) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of mesh that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!meshDataArray[mesh.meshID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of mesh that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		meshDataArray[mesh.meshID].referenceCount--;
		if (meshDataArray[mesh.meshID].referenceCount == 0 && !meshDataArray[mesh.meshID].persistent) {
			deleteMesh(mesh.meshID);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::bind(const Mesh& mesh)
	{
#ifdef _DEBUG
		if (mesh.meshID > maxMeshes) {
			warningLogger << LOGGER::BEGIN << "tried to bind mesh that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!meshDataArray[mesh.meshID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to bind mesh that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		meshDataArray[mesh.meshID].bind();
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::unbindAll()
	{
		VertexArray::unbind();
		IndexBuffer::unBind();
	}
	//------------------------------------------------------------------------------------------------------
	void MeshManager::terminate()
	{
		/// Clears the meshDataArray, deleting all meshes from the GPU!
		meshDataArray.clear();
	}
	//------------------------------------------------------------------------------------------------------
	MeshManager::MeshData& MeshManager::getMeshData(const Mesh& mesh)
	{
		return meshDataArray[mesh.meshID];
	}
	//------------------------------------------------------------------------------------------------------
}