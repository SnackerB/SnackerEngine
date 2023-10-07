#pragma once

#include "Core/Log.h"
#include "Math/Vec.h"
#include "Math/Mat.h"

#include <string>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class Shader
	{
	private:
		using ShaderID = unsigned int;
		friend class ShaderManager;
		ShaderID shaderID;
		Shader(const ShaderID& shaderID);
	public:
		Shader();
		/// Load shader from a given path relative to the resource directory
		explicit Shader(const std::string& path);
		/// Copy constructor and assignment operator
		Shader& operator=(const Shader& other) noexcept;
		Shader(const Shader& other) noexcept;
		/// Move constructor and assignment operator
		Shader& operator=(Shader&& other) noexcept;
		Shader(Shader&& other) noexcept;
		/// Binds this shader. Should be called before doing other operations, eg. setting uniforms
		void bind() const;
		/// Unbinds all shaders
		static void unBind();
		/// Setting uniforms
		template<typename T>
		void setUniform(const std::string& name, const T& data) const;
		/// Set model, view and projection matrix uniforms all at once
		void setModelViewProjection(const Mat4f& model, const Mat4f& view, const Mat4f& projection) const;
		/// Returns true if this shader is valid and corresponds to a shader on the GPU
		bool isValid() const;
		/// Destructor
		~Shader();
	};
	//------------------------------------------------------------------------------------------------------
}