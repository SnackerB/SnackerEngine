#pragma once

#include "Graphics/Mesh.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

#include <vector>
#include <queue>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class MeshManager
	{
		/// This struct stores all the data that is necessary to store for each mesh
		struct MeshData
		{
			// Standard constructor
			MeshData() = default;
			/// Move Constructor
			MeshData(MeshData&& other) noexcept = default;
			/// Move assignment operator
			MeshData& operator=(MeshData&& other) noexcept = default;
			/// Deleted copy constructor and assignment operator
			MeshData(MeshData& other) noexcept = delete;
			MeshData& operator=(MeshData& other) noexcept = delete;
			/// The VertexArray object 
			VertexArray va{};
			/// The VertexBuffer object
			VertexBuffer vb{};
			/// The IndexBuffer object
			IndexBuffer ib{};
			// Number of times this mesh is referenced by other objects
			unsigned int referenceCount{};
			// If this is set to false, this mesh will get deleted if referenceCount reaches zero
			bool persistent{};
			// This is set to false by standard and is set to true if an actual mesh is stored!
			bool valid{};
			// Destructor. Deletes the mesh from the GPU if it was loaded.
			~MeshData();

			/// Binds this mesh by binding the VertexArray, VertexBuffer and IndexBuffer
			void bind() const;
			/// Finalizes this MeshData object. If vb is valid this tries to bind it to va.
			/// If va, vb and ib are valid this MeshData object is also set to valid
			void finalize();
		};
		/// We just use unsigned ints as meshIDs. The IDs are also indices into the meshDataArray!
		using MeshID = unsigned int;
		/// Vector that stores all MeshData structs. MeshIDs are indices into this vector
		inline static std::vector<MeshData> meshDataArray;
		/// Queue of available (unused) MeshIDs
		inline static std::queue<MeshID> availableMeshIDs;
		/// Current maximal amount of Meshes that we can have before we have to resize
		inline static MeshID maxMeshes;
		/// Current count of Meshes loaded
		inline static MeshID loadedMeshesCount;

		/// Delete the given mesh. Is called by decreaseReferenceCount, if the reference count reaches zero
		static void deleteMesh(const MeshID& meshID);
		/// Returns a new meshID. The MeshData corresponding to this id can then be used to create a new mesh.
		/// Potentially resizes the meshDataArray, in which case a warning will be printed
		static MeshID getNewMeshID();
	protected:
		friend class Mesh;
		friend class AssetManager;
		/// Initializes the meshManager
		static void initialize(const unsigned int& startingSize);
		/// Increases the reference count of the given mesh
		static void increaseReferenceCount(const Mesh& mesh);
		/// Decreases the refrenece count of the given mesh, and deletes its meshData struct if necessary
		static void decreaseReferenceCount(const Mesh& mesh);
		/// Binds a given mesh
		static void bind(const Mesh& mesh);
		/// Unbinds all meshes
		static void unbindAll();
		/// Terminates the meshManager
		static void terminate();

		/// Returns a const reference to the corresponding MeshData object
		static MeshData& getMeshData(const Mesh& mesh);
	public:
		/// Deleted constructor: this is a purely virtual class!
		MeshManager() = delete;
		/// Create a mesh from vertices and indices vectors!
		template<typename T>
		static Mesh createMesh(const VertexBufferLayout& layout, const std::vector<T>& vertices, const std::vector<unsigned int>& indices, const VertexBuffer::VertexBufferStorageType& storageType = VertexBuffer::VertexBufferStorageType::STATIC);
	};
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Mesh MeshManager::createMesh(const VertexBufferLayout& layout, const std::vector<T>& vertices, const std::vector<unsigned int>& indices, const VertexBuffer::VertexBufferStorageType& storageType)
	{
		MeshID meshID = getNewMeshID();
		MeshData& meshData = meshDataArray[meshID];
		meshData.vb.setStorageType(storageType);
		meshData.vb.setDataAndFinalize<T>(vertices);
		meshData.ib.setIndices(indices);
		meshData.va = VertexArray(layout, meshData.vb);
		meshData.finalize();
		return Mesh(meshID);
	}
	//------------------------------------------------------------------------------------------------------
}