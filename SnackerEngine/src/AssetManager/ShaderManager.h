#pragma once

#include "Graphics/Shader.h"
#include "Core/Log.h"
#include "Math/Vec.h"
#include "Math/Mat.h"

#include <vector>
#include <queue>
#include <unordered_map>
#include <string_view>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class ShaderManager
	{
		/// This struct stores all the data that is necessary to store for each shader
		struct ShaderData
		{
			ShaderData() = default;
			// ID used for binding of the shader on the GPU
			unsigned int GPU_ID{};
			// Number of times this shader is referenced by other objects
			unsigned int referenceCount{};
			// If this is set to false, this shader will get deleted if referenceCount reaches zero
			bool persistent{};
			// This is set to false by standard and is set to true if an actual shader is stored!
			bool valid{};
			// File path of the shader. If the shader was not loaded from a file this should be an empty string.
			std::string path{};
			// Destructor. Deletes the shader from the GPU if it was loaded.
			~ShaderData();

			/// Binds this shader
			void bind() const;
			/// Returns the location of an uniform (location identifier is just an integer)
			int getUniformLocation(const std::string& name) const;
		};
		/// We just use unsigned ints as shaderIDs. The IDs are also indices into the shaderDataArray!
		using ShaderID = unsigned int;
		/// Vector that stores all ShaderData structs. ShaderIDs are indices into this vector
		inline static std::vector<ShaderData> shaderDataArray;
		/// Queue of available (unused) ShaderIDs
		inline static std::queue<ShaderID> availableShaderIDs;
		/// Current maximal amount of Shaders that we can have before we have to resize
		inline static ShaderID maxShaders;
		/// Current count of Shaders loaded
		inline static ShaderID loadedShadersCount;
		/// Used to look up Shaders using a path string (to prevent double loading)
		inline static std::unordered_map<std::string, ShaderID> stringToShaderID;
		/// shaderID of the default shader
		static constexpr ShaderID defaultShader{ 0 };
		/// path to the defaut shader (relative to resource directory)
		static constexpr std::string_view defaultShaderPath{ "shaders/basic.shader" };

		/// Delete the given shader. Is called by decreaseReferenceCount, if the reference count reaches zero
		static void deleteShader(const ShaderID& shaderID);
		/// Returns a new shaderID. The ShaderData corresponding to this id can then be used to create a new shader.
		/// Potentially resizes the shaderDataArray, in which case a warning will be printed
		static ShaderID getNewShaderID();
	protected:
		friend class Shader;
		friend class AssetManager;
		/// Initializes the shaderManager
		static void initialize(const unsigned int& startingSize);
		/// Increases the reference count of the given shader
		static void increaseReferenceCount(const Shader& shader);
		/// Decreases the refrenece count of the given shader, and deletes its shaderData struct if necessary
		static void decreaseReferenceCount(const Shader& shader);
		/// Binds a given shader
		static void bind(const Shader& shader);
		/// Unbinds all shaders
		static void unbindAll();
		/// Loads a shader from a file. If the shader is already loaded, the shaderID of the existing shader is returned!
		static ShaderID loadShader(const std::string& path);
		/// Terminates the shaderManager
		static void terminate();
		/// Setting uniforms
		template<typename T>
		static void setUniform(const Shader& shader, const std::string& name, const T& data);
		/// Set model, view and projection matrix uniforms all at once
		static void setModelViewProjection(const Shader& shader, const Mat4f& model, const Mat4f& view, const Mat4f& projection);
		/// Returns a const refrence to the corresponding ShaderData object
		static const ShaderData& getShaderData(const Shader& shader);
	public:
		/// Deleted constructor: this is a purely virtual class!
		ShaderManager() = delete;
	};
	//------------------------------------------------------------------------------------------------------
}