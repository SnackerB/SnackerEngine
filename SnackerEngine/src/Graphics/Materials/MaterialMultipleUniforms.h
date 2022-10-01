#pragma once

#include "AssetManager/MaterialData.h"

#include <array>
#include <string>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<typename T, std::size_t N>
	class MaterialDataMultipleUniforms : public MaterialData
	{
		/// Constructor
		MaterialDataSingleUniform(const Shader& shader, const std::array<std::string, N>& uniformNames, const std::array<T, N>& dataArray)
			: MaterialData(shader), uniformNames(uniformNames), dataArray(dataArray) {}
		/// Name of the uniform
		std::array<std::string, N> uniformNames;
		/// Storage for uniform value
		std::array<T, N> dataArray;
		/// Overload for bind function
		void bind() override;
	};
	//------------------------------------------------------------------------------------------------------
	template<typename T, std::size_t N>
	inline void MaterialDataMultipleUniforms<T, N>::bind()
	{
		MaterialData::bind();
		for (std::size_t i = 0; i < N; ++i) {
			shader.setUniform<T>(uniformNames[i], dataArray[i]);
		}
	}
	//------------------------------------------------------------------------------------------------------
}