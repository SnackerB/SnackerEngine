#include "Graphics/Shader.h"
#include "AssetManager/ShaderManager.h"
#include "Graphics/Color.h"
#include "Utility/Json.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Shader>(const nlohmann::json& json, JsonTag<Shader> tag)
	{
		return json.is_string();
	}
	//------------------------------------------------------------------------------------------------------
	template<> Shader parseJSON(const nlohmann::json& json, JsonTag<Shader> tag)
	{
		return Shader(parseJSON(json, JsonTag<std::string>{}));
	}
	//------------------------------------------------------------------------------------------------------
	Shader::Shader(const ShaderID& shaderID)
		: shaderID(shaderID) 
	{
		ShaderManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Shader::Shader() 
		: shaderID(0) {}
	//------------------------------------------------------------------------------------------------------
	Shader::Shader(const std::string& path)
		: shaderID(ShaderManager::loadShader(path))
	{
		ShaderManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Shader& Shader::operator=(const Shader& other) noexcept
	{
		ShaderManager::decreaseReferenceCount(*this);
		shaderID = other.shaderID;
		ShaderManager::increaseReferenceCount(*this);
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	Shader::Shader(const Shader& other) noexcept
		: shaderID(other.shaderID)
	{
		ShaderManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Shader& Shader::operator=(Shader&& other) noexcept
	{
		ShaderManager::decreaseReferenceCount(*this);
		shaderID = other.shaderID;
		other.shaderID = 0;
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	Shader::Shader(Shader&& other) noexcept
		: shaderID(other.shaderID)
	{
		other.shaderID = 0;
	}
	//------------------------------------------------------------------------------------------------------
	void Shader::bind() const
	{
		ShaderManager::bind(*this);
	}
	//------------------------------------------------------------------------------------------------------
	void Shader::unBind()
	{
		ShaderManager::unbindAll();
	}
	//------------------------------------------------------------------------------------------------------
	Shader::~Shader()
	{
		ShaderManager::decreaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<float>(const std::string& name, const float& data) const
	{
		ShaderManager::setUniform<float>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Vec2f>(const std::string& name, const Vec2f& data) const
	{
		ShaderManager::setUniform<Vec2f>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Vec3f>(const std::string& name, const Vec3f& data) const
	{
		ShaderManager::setUniform<Vec3f>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Vec4f>(const std::string& name, const Vec4f& data) const
	{
		ShaderManager::setUniform<Vec4f>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<int>(const std::string& name, const int& data) const
	{
		ShaderManager::setUniform<int>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Mat3f>(const std::string& name, const Mat3f& data) const
	{
		ShaderManager::setUniform<Mat3f>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Mat4f>(const std::string& name, const Mat4f& data) const
	{
		ShaderManager::setUniform<Mat4f>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Color3f>(const std::string& name, const Color3f& data) const
	{
		ShaderManager::setUniform<Color3f>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Color4f>(const std::string& name, const Color4f& data) const
	{
		ShaderManager::setUniform<Color4f>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Vec2i>(const std::string& name, const Vec2i& data) const
	{
		ShaderManager::setUniform<Vec2i>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Vec3i>(const std::string& name, const Vec3i& data) const
	{
		ShaderManager::setUniform<Vec3i>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::Shader::setUniform<Vec4i>(const std::string& name, const Vec4i& data) const
	{
		ShaderManager::setUniform<Vec4i>(*this, name, data);
	}
	//------------------------------------------------------------------------------------------------------
	void Shader::setModelViewProjection(const Mat4f& model, const Mat4f& view, const Mat4f& projection) const
	{
		ShaderManager::setModelViewProjection(*this, model, view, projection);
	}
	//------------------------------------------------------------------------------------------------------
	bool Shader::isValid() const
	{
		return ShaderManager::getShaderData(*this).valid;
	}
	//------------------------------------------------------------------------------------------------------
}