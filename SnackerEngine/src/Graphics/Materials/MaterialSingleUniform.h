#pragma once

#include "AssetManager/MaterialData.h"

#include <string>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	class MaterialDataSingleUniform : public MaterialData
	{
		/// Constructor
		MaterialDataSingleUniform(const Shader& shader, const std::string& uniformName, const T& data);
		/// Name of the uniform
		std::string uniformName;
		/// Storage for uniform value
		T data;
		/// Overload for bind function
		void bind() override;
	};
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline MaterialDataSingleUniform<T>::MaterialDataSingleUniform(const Shader& shader, const std::string& uniformName, const T& data)
		: MaterialData(shader), uniformName(uniformName), data(data) {}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void MaterialDataSingleUniform<T>::bind()
	{
		MaterialData::bind();
		shader.setUniform<T>(uniformName, data);
	}
	//------------------------------------------------------------------------------------------------------
}