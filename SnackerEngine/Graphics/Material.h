#pragma once

#include "Graphics/Shader.h"
#include "AssetManager/MaterialData.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class Material
	{
	private:
		using MaterialID = unsigned int;
		friend class MaterialManager;
		MaterialID materialID;
		Material(MaterialID materialID);
	public:
		Material();
		/// Constructor from shader
		explicit Material(const Shader& shader);
		/// Constructor from pointer to material data
		explicit Material(std::unique_ptr<MaterialData>&& materialData);
		// Copy constructor and assignment operator
		Material& operator=(const Material& other) noexcept;
		explicit Material(const Material& other) noexcept;
		/// Binds this material. Should be called before doing other operations, eg. binding the material
		void bind() const;
		/// Unbinds all materials
		void unBind() const;
		/// Returns a const reference to the shader of this material
		const Shader& getShader();
		/// Destructor
		~Material();
	};
	//------------------------------------------------------------------------------------------------------
}