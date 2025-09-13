#pragma once

#include "Graphics/Shader.h"

namespace SnackerEngine
{
	/// This struct acts as a base struct for data that needs to be stored for each material.
	/// Different other materials can be derived from this
	struct MaterialData
	{
		MaterialData() = default;
		/// Creates a MaterialData object using a shader
		MaterialData(const Shader& shader);
		// Number of times this material is referenced by other objects
		unsigned int referenceCount{};
		// If this is set to false, this material will get deleted if referenceCount reaches zero
		bool persistent{};
		// This is set to false by standard and is set to true if an actual material is stored!
		bool valid{};
		/// Each material needs a shader
		Shader shader{};
		// Destructor. Deletes the material from the GPU if it was loaded.
		virtual ~MaterialData();

		/// Binds this material
		virtual void bind();
	};
}