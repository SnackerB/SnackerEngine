#pragma once

#include "AssetManager/MaterialData.h"
#include "Math/Vec.h"
#include "Graphics/Texture.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class MaterialDataPBR : public MaterialData
	{
		/// Constructor
		MaterialDataPBR(const Shader& shader, const Vec3f& albedo, const float& metallic, const float& roughness, const float& ao);
		/// PBR parameters
		Vec3f albedo;
		float metallic;
		float roughness;
		float ao;
		/// override for binding the material
		void bind() override;
	};
	//------------------------------------------------------------------------------------------------------
	class MaterialDataPBRTextured : public MaterialData
	{
		/// Constructor
		MaterialDataPBRTextured(const Shader& shader, const Texture& albedo, const Texture& metallic, const Texture& roughness, const Texture& ao, const Texture& height);
		/// PBR parameters
		Texture albedo;
		Texture metallic;
		Texture roughness;
		Texture ao;
		Texture height;
		/// override for binding the material
		void bind() override;
	};
	//------------------------------------------------------------------------------------------------------
}