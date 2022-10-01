#include "AssetManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
#include "MaterialManager.h"
#include "FontManager.h"

namespace SnackerEngine
{

	void AssetManager::initialize()
	{
		ShaderManager::initialize(10); // TODO: Variable starting size
		TextureManager::initialize(10); // TODO: Variable starting size
		MeshManager::initialize(10);  // TODO: Variable starting size 
		ModelManager::initialize(10); // TODO: Variable starting size
		MaterialManager::initialize(10); // TODO: Variable starting size
		FontManager::initialize(10); // TODO: Variable starting size
	}

	void AssetManager::terminate()
	{
		FontManager::terminate();
		MaterialManager::terminate();
		ModelManager::terminate();
		MeshManager::terminate();
		TextureManager::terminate();
		ShaderManager::terminate();
	}

}