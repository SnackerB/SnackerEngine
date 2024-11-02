#include "Graphics/Material.h"
#include "AssetManager/MaterialManager.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	Material::Material(MaterialID materialID)
		: materialID(materialID)
	{
		MaterialManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Material::Material()
		: materialID(0) {}
	//------------------------------------------------------------------------------------------------------
	Material::Material(const Shader& shader)
		: Material(MaterialManager::createMaterial(shader)) {}
	//------------------------------------------------------------------------------------------------------
	Material::Material(std::unique_ptr<MaterialData>&& materialData)
		: Material(MaterialManager::createMaterial(std::move(materialData))) {}
	//------------------------------------------------------------------------------------------------------
	Material& Material::operator=(const Material& other) noexcept
	{
		MaterialManager::decreaseReferenceCount(*this);
		materialID = other.materialID;
		MaterialManager::increaseReferenceCount(*this);
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	Material::Material(const Material& other) noexcept
		: materialID(other.materialID)
	{
		MaterialManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	void Material::bind() const
	{
		MaterialManager::bind(*this);
	}
	//------------------------------------------------------------------------------------------------------
	void Material::unBind() const
	{
		MaterialManager::unbindAll();
	}
	//------------------------------------------------------------------------------------------------------
	Material::~Material()
	{
		MaterialManager::decreaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	const Shader& Material::getShader()
	{
		return MaterialManager::getMaterialData(*this)->shader;
	}
	//------------------------------------------------------------------------------------------------------
}