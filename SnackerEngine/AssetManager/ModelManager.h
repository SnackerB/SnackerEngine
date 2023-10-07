#pragma once

#include "Graphics/Model.h"
#include "Graphics/Mesh.h"

#include <vector>
#include <queue>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class ModelManager
	{
		/// This struct stores all the data that is necessary to store for each model
		struct ModelData
		{
			/// Standard constructor
			ModelData() = default;
			/// Move Constructor
			ModelData(ModelData&& other) noexcept = default;
			/// Move assignment operator
			ModelData& operator=(ModelData&& other) noexcept = default;
			/// Deleted copy constructor and assignment operator
			ModelData(ModelData& other) noexcept = delete;
			ModelData& operator=(ModelData& other) noexcept = delete;
			/// Number of times this model is referenced by other objects
			unsigned int referenceCount{};
			/// If this is set to false, this model will get deleted if referenceCount reaches zero
			bool persistent{};
			/// This is set to false by standard and is set to true if an actual model is stored!
			bool valid{};
			/// Vector of meshes
			std::vector<Mesh> meshes{};
			/// Destructor. Deletes the model from the GPU if it was loaded.
			~ModelData();

			/// Inserts a new mesh into the mesh vector
			void insertMesh(const Mesh& mesh);
		};
		/// We just use unsigned ints as modelIDs. The IDs are also indices into the modelDataArray!
		using ModelID = unsigned int;
		/// Vector that stores all ModelData structs. ModelIDs are indices into this vector
		inline static std::vector<ModelData> modelDataArray;
		/// Queue of available (unused) ModelIDs
		inline static std::queue<ModelID> availableModelIDs;
		/// Current maximal amount of Models that we can have before we have to resize
		inline static ModelID maxModels;
		/// Current count of Modesl loaded
		inline static ModelID loadedModelsCount;

		/// Delete the given model. Is called by decreaseReferenceCount, if the reference count reaches zero
		static void deleteModel(const ModelID& modelID);
		/// Returns a new modelID. The ModelData corresponding to this id can then be used to create a new model.
		/// Potentially resizes the modelDataArray, in which case a warning will be printed
		static ModelID getNewModelID();
	protected:
		friend class Model;
		friend class AssetManager;
		/// Initializes the modelManager
		static void initialize(const unsigned int& startingSize);
		/// Increases the reference count of the given model
		static void increaseReferenceCount(const Model& model);
		/// Decreases the refrenece count of the given model, and deletes its modelData struct if necessary
		static void decreaseReferenceCount(const Model& model);
		/// Terminates the modelManager
		static void terminate();

		/// Returns a const reference to the corresponding ModelData object
		static const ModelData& getModelData(const Model& model);
		/// Loads model from .obj file. Path is relative to resource directory
		static Model loadModelOBJ(const std::string& path);
		/// Creates model from array of meshes
		static Model createModel(const std::vector<Mesh>& meshes);
		/// Creates model from a single mesh
		static Model createModel(const Mesh& mesh);
	public:
		/// Deleted constructor: this is a purely virtual class!
		ModelManager() = delete;
	};
	//------------------------------------------------------------------------------------------------------
}