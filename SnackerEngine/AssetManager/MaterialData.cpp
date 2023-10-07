#include "AssetManager/MaterialData.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	MaterialData::MaterialData(const Shader& shader)
		: referenceCount(0), persistent(false), valid(shader.isValid()), shader(shader) {}
	//------------------------------------------------------------------------------------------------------
	MaterialData::~MaterialData() {}
	//------------------------------------------------------------------------------------------------------
	void MaterialData::bind()
	{
		shader.bind();
	}
	//------------------------------------------------------------------------------------------------------
}