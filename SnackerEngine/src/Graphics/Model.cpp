#include "Graphics/Model.h"
#include "AssetManager/ModelManager.h"

namespace SnackerEngine
{
    //------------------------------------------------------------------------------------------------------
    Model::Model(ModelID modelID)
        : modelID(modelID)
    {
        ModelManager::increaseReferenceCount(*this);
    }
    //------------------------------------------------------------------------------------------------------
    Model::Model() 
        : modelID(0) {}
    //------------------------------------------------------------------------------------------------------
    Model::Model(const std::string& path)
        : Model(ModelManager::loadModelOBJ(path)) {}
    //------------------------------------------------------------------------------------------------------
    Model::Model(const std::vector<Mesh>& meshes)
        : Model(ModelManager::createModel(meshes)) {}
    //------------------------------------------------------------------------------------------------------
    Model::Model(const Mesh& mesh)
        : Model(ModelManager::createModel(mesh)) {}
    //------------------------------------------------------------------------------------------------------
    Model& Model::operator=(const Model& other) noexcept
    {
        ModelManager::decreaseReferenceCount(*this);
        modelID = other.modelID;
        ModelManager::increaseReferenceCount(*this);
        return *this;
    }
    //------------------------------------------------------------------------------------------------------
    Model::Model(const Model& other) noexcept
        : modelID(other.modelID)
    {
        ModelManager::increaseReferenceCount(*this);
    }
    //------------------------------------------------------------------------------------------------------
    bool Model::isValid() const
    {
        return ModelManager::getModelData(*this).valid;
    }
    //------------------------------------------------------------------------------------------------------
    const std::vector<Mesh>& Model::getMeshes() const
    {
        return ModelManager::getModelData(*this).meshes;
    }
    //------------------------------------------------------------------------------------------------------
    Model::~Model()
    {
        ModelManager::decreaseReferenceCount(*this);
    }
    //------------------------------------------------------------------------------------------------------
}