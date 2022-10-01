#pragma once

#include "Graphics/Material.h"
#include "AssetManager/MaterialData.h"
#include "Graphics/Shader.h"

#include <vector>
#include <queue>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class MaterialManager
	{
	private:
		/// We just use unsigned ints as materialIDs. The IDs are also indices into the materialDataPtrArray!
		using MaterialID = unsigned int;
		/// Vector that stores all pointers to MaterialData structs. MaterialIDs are indices into this vector
		/// We need to store pointers since there are multiple different materialData classes, all deriving from 
		/// the base class.
		inline static std::vector<std::unique_ptr<MaterialData>> materialDataPtrArray;
		/// Queue of available (unused) MaterialIDs
		inline static std::queue<MaterialID> availableMaterialIDs;
		/// Current maximal amount of Materials that we can have before we have to resize
		inline static MaterialID maxMaterials;
		/// Current count of Materials loaded
		inline static MaterialID loadedMaterialsCount;
		/// materialID of the default material
		static constexpr MaterialID defaultMaterial{ 0 };

		/// Delete the given material. Is called by decreaseReferenceCount, if the reference count reaches zero
		static void deleteMaterial(const MaterialID& materialID);
		/// Returns a new materialID. The MaterialData corresponding to this id can then be used to create a new material.
		/// Potentially resizes the materialDataPtrArray, in which case a warning will be printed
		static MaterialID getNewMaterialID();
	protected:
		friend class Material;
		friend class AssetManager;
		/// Initializes the materialManager
		static void initialize(const unsigned int& startingSize);
		/// Increases the reference count of the given material
		static void increaseReferenceCount(const Material& material);
		/// Decreases the refrenece count of the given material, and deletes its materialData struct if necessary
		static void decreaseReferenceCount(const Material& material);
		/// Binds a given material
		static void bind(const Material& material);
		/// Unbinds all materials
		static void unbindAll();
		/// Terminates the materialManager
		static void terminate();
		/// Returns a reference to the pointer to the MaterialData object
		static const std::unique_ptr<MaterialData>& getMaterialData(const Material& material);
	public:
		/// Deleted constructor: this is a purely virtual class!
		MaterialManager() = delete;
		/// Creates a material from a shader
		static Material createMaterial(const Shader& shader);
		/// Creates a material from a materialData object
		static Material createMaterial(std::unique_ptr<MaterialData>&& materialDataPtr);
	};
	//------------------------------------------------------------------------------------------------------
}