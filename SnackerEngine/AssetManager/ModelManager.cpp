#include "AssetManager/ModelManager.h"
#include "Core/Log.h"
#include "AssetManager/LoaderOBJ.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	ModelManager::ModelData::~ModelData() {}
	//------------------------------------------------------------------------------------------------------
	void ModelManager::ModelData::insertMesh(const Mesh& mesh)
	{
		meshes.push_back(mesh);
	}
	//------------------------------------------------------------------------------------------------------
	void ModelManager::deleteModel(const ModelID& modelID)
	{
#ifdef _DEBUG
		if (modelID > maxModels) {
			warningLogger << LOGGER::BEGIN << "tried to delete model that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!modelDataArray[modelID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to delete model that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG

		// Reset and unload model
		modelDataArray[modelID] = std::move(ModelData());
		loadedModelsCount--;
		availableModelIDs.push(modelID);
	}
	//------------------------------------------------------------------------------------------------------
	ModelManager::ModelID ModelManager::getNewModelID()
	{
		if (loadedModelsCount >= maxModels)
		{
			// Resize vector and add new available modelID slots accordingly. For now: double size everytime this happens and send warning!
			modelDataArray.resize(maxModels * 2 + 1);
			for (ModelID id = maxModels + 1; id <= 2 * maxModels; ++id)
			{
				availableModelIDs.push(id);
			}
			warningLogger << LOGGER::BEGIN << "maximum amount of models exceeded. Resizing modelManager to be able to store "
				<< 2 * maxModels << " models!" << LOGGER::ENDL;
			maxModels *= 2;
		}
		// Take ID from the front of the queue
		ModelID id = availableModelIDs.front();
		availableModelIDs.pop();
		++loadedModelsCount;
		return id;
	}
	//------------------------------------------------------------------------------------------------------
	void ModelManager::initialize(const unsigned int& startingSize)
	{
		// Initializes queue with all possible modelIDs. modelID = 0 is reserved for invalid modeles.
		for (ModelID id = 1; id <= startingSize; ++id)
		{
			availableModelIDs.push(id);
		}
		// modelDataArray[0] stores the default/invalid model. TODO: Maybe add default model?
		modelDataArray.resize(startingSize + 1);
		maxModels = startingSize;
	}
	//------------------------------------------------------------------------------------------------------
	void ModelManager::increaseReferenceCount(const Model& model)
	{
		if (model.modelID == 0)
			return;
#ifdef _DEBUG
		if (model.modelID > maxModels) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of model that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!modelDataArray[model.modelID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of model that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		modelDataArray[model.modelID].referenceCount++;
	}
	//------------------------------------------------------------------------------------------------------
	void ModelManager::decreaseReferenceCount(const Model& model)
	{
		if (model.modelID == 0)
			return;
#ifdef _DEBUG
		if (model.modelID > maxModels) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of model that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!modelDataArray[model.modelID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of model that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		modelDataArray[model.modelID].referenceCount--;
		if (modelDataArray[model.modelID].referenceCount == 0 && !modelDataArray[model.modelID].persistent) {
			deleteModel(model.modelID);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void ModelManager::terminate()
	{
		/// Clears the modelDataArray, deleting all models!
		modelDataArray.clear();
	}
	//------------------------------------------------------------------------------------------------------
	const ModelManager::ModelData& ModelManager::getModelData(const Model& model)
	{
		return modelDataArray[model.modelID];
	}
	//------------------------------------------------------------------------------------------------------
	Model ModelManager::loadModelOBJ(const std::string& path)
	{
#ifdef _DEBUG
		return SnackerEngine::loadModelOBJ(path, true);
#else
		return SnackerEngine::loadModelOBJ(path);
#endif // _DEBUG

	}
	//------------------------------------------------------------------------------------------------------
	Model ModelManager::createModel(const std::vector<Mesh>& meshes)
	{
		ModelID modelID = getNewModelID();
		ModelData& modelData = modelDataArray[modelID];
		modelData.referenceCount = 0;
		modelData.persistent = false;
		modelData.valid = meshes.size() > 0 ? true : false;
		modelData.meshes = meshes;
		return Model(modelID);
	}
	//------------------------------------------------------------------------------------------------------
	Model ModelManager::createModel(const Mesh& mesh)
	{
		ModelID modelID = getNewModelID();
		ModelData& modelData = modelDataArray[modelID];
		modelData.referenceCount = 0;
		modelData.persistent = false;
		modelData.valid = mesh.isValid();
		modelData.meshes = std::vector<Mesh>();
		modelData.insertMesh(mesh);
		return Model(modelID);
	}
	//------------------------------------------------------------------------------------------------------
}