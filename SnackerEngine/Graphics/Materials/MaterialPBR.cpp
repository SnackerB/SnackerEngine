#include "Graphics/Materials/MaterialPBR.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	MaterialDataPBR::MaterialDataPBR(const Shader& shader, const Vec3f& albedo, const float& metallic, const float& roughness, const float& ao)
		: MaterialData(shader), albedo(albedo), metallic(metallic), roughness(roughness), ao(ao) {}
	//------------------------------------------------------------------------------------------------------
	void MaterialDataPBR::bind()
	{
		MaterialData::bind();
		shader.setUniform<Vec3f>(std::string("u_albedo"), albedo);
		shader.setUniform<float>("u_metallic", metallic);
		shader.setUniform<float>("u_roughness", roughness);
		shader.setUniform<float>("u_ao", ao);
	}
	//------------------------------------------------------------------------------------------------------
	MaterialDataPBRTextured::MaterialDataPBRTextured(const Shader& shader, const Texture& albedo, const Texture& metallic, const Texture& roughness, const Texture& ao, const Texture& height)
		: MaterialData(shader), albedo(albedo), metallic(metallic), roughness(roughness), ao(ao), height(height) {}
	//------------------------------------------------------------------------------------------------------
	void MaterialDataPBRTextured::bind()
	{
		MaterialData::bind();
		albedo.bind(0);
		metallic.bind(1);
		roughness.bind(2);
		ao.bind(3);
		height.bind(3);
	}
	//------------------------------------------------------------------------------------------------------
}