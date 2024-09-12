#include "Graphics/Texture.h"
#include "AssetManager/TextureManager.h"
#include "Utility\Json.h"

#include <msdfgen.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Texture>(const nlohmann::json& json, JsonTag<Texture> tag)
	{
		return json.is_string();
	}
	//------------------------------------------------------------------------------------------------------
	template<> Texture parseJSON(const nlohmann::json& json, JsonTag<Texture> tag)
	{
		return Texture::Load2D(parseJSON(json, JsonTag<std::string>{})).first;
	}
	//--------------------------------------------------------------------------------------------------
	Texture::Texture(TextureID textureID)
		: textureID(textureID)
	{
		TextureManager::increaseReferenceCount(*this);
	}
	//--------------------------------------------------------------------------------------------------
	Texture::Texture()
		: textureID(0) {}
	//--------------------------------------------------------------------------------------------------
	Texture& Texture::operator=(const Texture& other) noexcept
	{
		TextureManager::decreaseReferenceCount(*this);
		textureID = other.textureID;
		TextureManager::increaseReferenceCount(*this);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	Texture::Texture(const Texture& other) noexcept
		: textureID(other.textureID)
	{
		TextureManager::increaseReferenceCount(*this);
	}
	//--------------------------------------------------------------------------------------------------
	void Texture::bind(const unsigned int& slot)
	{
		TextureManager::bind(*this);
	}
	//--------------------------------------------------------------------------------------------------
	void Texture::unBind()
	{
		TextureManager::unbindAll();
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i Texture::getSize() const
	{
		return TextureManager::getSize(*this);
	}
	//--------------------------------------------------------------------------------------------------
	bool Texture::isValid() const
	{
		return TextureManager::getTextureData(*this).valid;
	}
	//--------------------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		TextureManager::decreaseReferenceCount(*this);
	}
	//--------------------------------------------------------------------------------------------------
	std::pair<Texture, bool> Texture::Load2D(const std::string& path, const bool& persistent)
	{
		return TextureManager::loadTexture2D(path, persistent);
	}
	//--------------------------------------------------------------------------------------------------
	std::pair<Texture, bool> Texture::Load2DFromRawData(const ConstantBufferView& buffer, const std::string& fileName)
	{
		return TextureManager::loadTexture2DFromRawData(buffer, fileName);
	}
	//--------------------------------------------------------------------------------------------------
	Texture Texture::Create2D(const Vec2i& dimensions, const TextureDataFormat& dataFormat, const bool& mip)
	{
		return TextureManager::createTexture(dimensions, TextureType::TEXTURE2D,
			TextureDataType::UNSIGNED_BYTE, dataFormat, TextureDataPrecision::PRECISION_NOT_SPECIFIED, mip);
	}
	//--------------------------------------------------------------------------------------------------
	Texture Texture::CreateFloat2D(const Vec2i& dimensions, const TextureDataFormat& dataFormat, const bool& mip)
	{
		return TextureManager::createTexture(dimensions, TextureType::TEXTURE2D,
			TextureDataType::FLOAT, dataFormat, TextureDataPrecision::PRECISION_32, mip);
	}
	//--------------------------------------------------------------------------------------------------
	Texture Texture::CreateFloatCubemap(const Vec2i& dimensions, const TextureDataFormat& dataFormat, const bool& mip)
	{
		return TextureManager::createTexture(dimensions, TextureType::CUBEMAP,
			TextureDataType::FLOAT, dataFormat, TextureDataPrecision::PRECISION_32, mip);
	}
	//--------------------------------------------------------------------------------------------------
	Texture Texture::CreateFromBuffer(TextureDataBuffer& buffer, const bool& mip)
	{
		return TextureManager::createTextureFromBuffer(buffer, mip);
	}
	//--------------------------------------------------------------------------------------------------
	void Texture::fill2D(TextureDataBuffer& textureDataBuffer, const bool& mip)
	{
		TextureManager::fillTexture2D(*this, textureDataBuffer, mip);
	}
	//--------------------------------------------------------------------------------------------------
	void Texture::fill2D(TextureDataBuffer& textureDataBuffer, const Vec2i& offset, const bool& mip)
	{
		TextureManager::fillTexture2D(*this, textureDataBuffer, offset, mip);
	}
	//--------------------------------------------------------------------------------------------------
	void Texture::copyTexture2D(const Texture& source, Texture& target, const Vec2i& offset)
	{
		TextureManager::copyTexture2D(source, target, offset);
	}
	//--------------------------------------------------------------------------------------------------
	bool Texture::saveInFile(const std::string& path, const bool& relativeToResourcePath)
	{
		return TextureManager::saveTexture2D(*this, path, relativeToResourcePath);
	}
	//--------------------------------------------------------------------------------------------------
}