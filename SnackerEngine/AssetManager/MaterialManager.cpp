#include "AssetManager/MaterialManager.h"
#include "Graphics/Texture.h"

namespace SnackerEngine
{
	//======================================================================================================
	// MaterialManager implementation
	//======================================================================================================
	void MaterialManager::deleteMaterial(const MaterialID& materialID)
	{
#ifdef _DEBUG
		if (materialID > maxMaterials) {
			warningLogger << LOGGER::BEGIN << "tried to delete material that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!materialDataPtrArray[materialID] || !materialDataPtrArray[materialID]->valid) {
			warningLogger << LOGGER::BEGIN << "tried to delete material that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		// Reset and unload material
		materialDataPtrArray[materialID] = nullptr;
		loadedMaterialsCount--;
		availableMaterialIDs.push(materialID);
	}
	//------------------------------------------------------------------------------------------------------
	MaterialManager::MaterialID MaterialManager::getNewMaterialID()
	{
		if (loadedMaterialsCount >= maxMaterials)
		{
			// Resize vector and add new available materialID slots accordingly. For now: double size everytime this happens and send warning!
			materialDataPtrArray.resize(maxMaterials * 2 + 1);
			for (MaterialID id = maxMaterials + 1; id <= 2 * maxMaterials; ++id)
			{
				availableMaterialIDs.push(id);
			}
			warningLogger << LOGGER::BEGIN << "maximum amount of materials exceeded. Resizing materialManager to be able to store "
				<< 2 * maxMaterials << " materials!" << LOGGER::ENDL;
			maxMaterials *= 2;
		}
		// Take ID from the front of the queue
		MaterialID id = availableMaterialIDs.front();
		availableMaterialIDs.pop();
		++loadedMaterialsCount;
		return id;
	}
	//------------------------------------------------------------------------------------------------------
	void MaterialManager::initialize(const unsigned int& startingSize)
	{
		// Initializes queue with all possible materialIDs. materialID = 0 is reserved for invalid materials.
		for (MaterialID id = 1; id <= startingSize; ++id)
		{
			availableMaterialIDs.push(id);
		}
		// materialDataArray[0] stores the default material
		materialDataPtrArray.resize(startingSize + 1);
		maxMaterials = startingSize;
		// Load default material!
		materialDataPtrArray[0] = std::make_unique<MaterialData>();
		MaterialData& materialData = *materialDataPtrArray[0];
		materialData.persistent = true;
		materialData.referenceCount = 0;
		materialData.shader = Shader();
		materialData.valid = materialData.shader.isValid();
	}
	//------------------------------------------------------------------------------------------------------
	void MaterialManager::increaseReferenceCount(const Material& material)
	{
		if (material.materialID == 0)
			return;
#ifdef _DEBUG
		if (material.materialID > maxMaterials) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of material that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!materialDataPtrArray[material.materialID] || !materialDataPtrArray[material.materialID]->valid) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of material that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		materialDataPtrArray[material.materialID]->referenceCount++;
	}
	//------------------------------------------------------------------------------------------------------
	void MaterialManager::decreaseReferenceCount(const Material& material)
	{
		if (material.materialID == 0)
			return;
#ifdef _DEBUG
		if (material.materialID > maxMaterials) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of material that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!materialDataPtrArray[material.materialID] || !materialDataPtrArray[material.materialID]->valid) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of material that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		materialDataPtrArray[material.materialID]->referenceCount--;
		if (materialDataPtrArray[material.materialID]->referenceCount == 0 && !materialDataPtrArray[material.materialID]->persistent) {
			deleteMaterial(material.materialID);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void MaterialManager::bind(const Material& material)
	{
#ifdef _DEBUG
		if (material.materialID > maxMaterials) {
			warningLogger << LOGGER::BEGIN << "tried to bind material that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!materialDataPtrArray[material.materialID] || !materialDataPtrArray[material.materialID]->valid) {
			warningLogger << LOGGER::BEGIN << "tried to bind material that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		materialDataPtrArray[material.materialID]->bind();
	}
	//------------------------------------------------------------------------------------------------------
	void MaterialManager::unbindAll()
	{
		Shader::unBind();
		Texture::unBind();
	}
	//------------------------------------------------------------------------------------------------------
	void MaterialManager::terminate()
	{
		/// Clears the materialDataArray, deleting all materials from the GPU!
		materialDataPtrArray.clear();
	}
	//------------------------------------------------------------------------------------------------------
	const std::unique_ptr<MaterialData>& MaterialManager::getMaterialData(const Material& material)
	{
		return materialDataPtrArray[material.materialID];
	}
	//------------------------------------------------------------------------------------------------------
	Material MaterialManager::createMaterial(const Shader& shader)
	{
		MaterialID materialID = getNewMaterialID();
		materialDataPtrArray[materialID] = std::make_unique<MaterialData>(shader);
		return Material(materialID);
	}
	//------------------------------------------------------------------------------------------------------
	Material MaterialManager::createMaterial(std::unique_ptr<MaterialData>&& materialDataPtr)
	{
		MaterialID materialID = getNewMaterialID();
		materialDataPtrArray[materialID] = std::move(materialDataPtr);
		return Material(materialID);
	}
	//------------------------------------------------------------------------------------------------------
}