#pragma once

#include "Graphics/Mesh.h"

#include <string>
#include <vector>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class Model
	{
	private:
		using ModelID = unsigned int;
		friend class ModelManager;
		ModelID modelID;
		Model(ModelID modelID);
	public:
		/// Default constructor
		Model();
		/// Loads model from .obj file. Path is relative to resource directory
		Model(const std::string& path);
		/// Creates model from array of meshes
		Model(const std::vector<Mesh>& meshes);
		/// Creates model from a single mesh
		Model(const Mesh& mesh);
		/// Copy constructor and assignment operator
		Model& operator=(const Model& other) noexcept;
		Model(const Model& other) noexcept;
		/// Returns the true if this Model object is valid and corresponds to data stored on the GPU
		bool isValid() const;
		/// Returns the vector of meshes that this model contains
		const std::vector<Mesh>& getMeshes() const;
		// Destructor
		~Model();
	};
	//------------------------------------------------------------------------------------------------------
}