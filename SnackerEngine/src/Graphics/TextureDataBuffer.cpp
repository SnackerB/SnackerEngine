#include "Graphics/TextureDataBuffer.h"
#include "AssetManager/TextureManager.h"
#include "Core/Log.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// Helper function computing the stride (in "number of Elements") of a given textureDataFormat
	static unsigned int determineStride(const Texture::TextureDataFormat& textureDataFormat) 
	{
		switch (textureDataFormat)
		{
		case Texture::TextureDataFormat::R: return 1;
		case Texture::TextureDataFormat::RG: return 2;
		case Texture::TextureDataFormat::RGB: return 3;
		case Texture::TextureDataFormat::RGBA: return 4;
		default: return 0;
		}
	}
	//------------------------------------------------------------------------------------------------------
	/// Helper function determining the bytes per element necessary for storing a texture of a given textureDataPrecision
	unsigned int determineBytesPerElement(const Texture::TextureDataPrecision textureDataPrecision)
	{
		switch (textureDataPrecision)
		{
		case Texture::TextureDataPrecision::PRECISION_32: return 4;
		case Texture::TextureDataPrecision::PRECISION_16: return 2;
		default: return 1;
		}
	}
	//------------------------------------------------------------------------------------------------------
	unsigned int TextureDataBuffer::computeIndex(const Vec2i& position) const
	{
		return position.y * (padding + size.x * stride) + position.x * stride;
	}
	//------------------------------------------------------------------------------------------------------
	void TextureDataBuffer::setSingleValue(const unsigned int& index, const int& value)
	{
		if (std::endian::native == std::endian::big) {
			// Big endian
			switch (textureDataPrecision)
			{
			case SnackerEngine::Texture::TextureDataPrecision::PRECISION_16:
			{
				dataStorage[index + 0] = static_cast<std::byte>(value << 24);
				dataStorage[index + 1] = static_cast<std::byte>(value << 16);
				break;
			}
			case SnackerEngine::Texture::TextureDataPrecision::PRECISION_32:
			{
				std::int32_t tempVal = static_cast<std::int32_t>(value);
				dataStorage[index + 0] = static_cast<std::byte>(value << 24);
				dataStorage[index + 1] = static_cast<std::byte>(value << 16);
				dataStorage[index + 2] = static_cast<std::byte>(value << 8);
				dataStorage[index + 3] = static_cast<std::byte>(value);
				break;
			}
			default:
			{
				dataStorage[index + 0] = static_cast<std::byte>(value << 24);
				break;
			}
			}
		}
		else {
			// Little endian
			switch (textureDataPrecision)
			{
			case SnackerEngine::Texture::TextureDataPrecision::PRECISION_16:
			{
				dataStorage[index + 0] = static_cast<std::byte>(value);
				dataStorage[index + 1] = static_cast<std::byte>(value << 8);
				break;
			}
			case SnackerEngine::Texture::TextureDataPrecision::PRECISION_32:
			{
				std::int32_t tempVal = static_cast<std::int32_t>(value);
				dataStorage[index + 0] = static_cast<std::byte>(value);
				dataStorage[index + 1] = static_cast<std::byte>(value << 8);
				dataStorage[index + 2] = static_cast<std::byte>(value << 16);
				dataStorage[index + 3] = static_cast<std::byte>(value << 24);
				break;
			}
			default:
			{
				dataStorage[index + 0] = static_cast<std::byte>(value);
				break;
			}
			}
		}
	}
	//------------------------------------------------------------------------------------------------------
	void TextureDataBuffer::setSingleValue(const unsigned int& index, const float& value)
	{
		switch (textureDataPrecision)
		{
		case SnackerEngine::Texture::TextureDataPrecision::PRECISION_16:
		{
			warningLogger << LOGGER::BEGIN << "16 bit floating point numbers are not supported in TextureDataBuffer" << LOGGER::ENDL;
			break;
		}
		case SnackerEngine::Texture::TextureDataPrecision::PRECISION_32:
		{
			const float* tempPtr = &value;
			if (std::endian::native == std::endian::big) {
				// Big endian
				dataStorage[index + 0] = *(reinterpret_cast<const std::byte*>(tempPtr) + 3);
				dataStorage[index + 1] = *(reinterpret_cast<const std::byte*>(tempPtr) + 2);
				dataStorage[index + 2] = *(reinterpret_cast<const std::byte*>(tempPtr) + 1);
				dataStorage[index + 3] = *(reinterpret_cast<const std::byte*>(tempPtr) + 0);
			}
			else {
				// Little endian
				dataStorage[index + 0] = *(reinterpret_cast<const std::byte*>(tempPtr) + 0);
				dataStorage[index + 1] = *(reinterpret_cast<const std::byte*>(tempPtr) + 1);
				dataStorage[index + 2] = *(reinterpret_cast<const std::byte*>(tempPtr) + 2);
				dataStorage[index + 3] = *(reinterpret_cast<const std::byte*>(tempPtr) + 3);
			}
			break;
		}
		default:
		{
			warningLogger << LOGGER::BEGIN << "8 bit floating point numbers are not supported in TextureDataBuffer" << LOGGER::ENDL;
			break;
		}
		}
	}
	//------------------------------------------------------------------------------------------------------
	void* TextureDataBuffer::getDataPointer()
	{
		return dataStorage.data();
	}
	//------------------------------------------------------------------------------------------------------
	TextureDataBuffer::TextureDataBuffer(const Texture::TextureDataType& textureDataType, const Texture::TextureDataPrecision& textureDataPrecision, const Texture::TextureDataFormat& textureDataFormat, const Vec2i& size)
		: textureDataType(textureDataType), textureDataPrecision(textureDataPrecision), textureDataFormat(textureDataFormat), size(size), stride(determineStride(textureDataFormat)), dataSize(0), padding(0), dataStorage{}, bytesPerElement(determineBytesPerElement(textureDataPrecision))
	{
		stride *= bytesPerElement;
		if (textureDataPrecision == Texture::TextureDataPrecision::PRECISION_32) {
			padding = 0;
			dataSize = stride * size.x * size.y;
		}
		else if (textureDataPrecision == Texture::TextureDataPrecision::PRECISION_16) {
			padding = static_cast<int>((stride * size.x)) % 2;
			if (padding != 0) padding = 2 - padding;
			padding *= bytesPerElement;
			dataSize = (stride * size.x + padding) * size.y;
		}
		else {
			padding = (int)(stride * size.x) % 4;
			if (padding != 0) padding = 4 - padding;
			padding *= bytesPerElement;
			dataSize = (stride * size.x + padding) * size.y;
		}
		dataStorage.resize(dataSize, static_cast<std::byte>(0));
	}
	//------------------------------------------------------------------------------------------------------
	TextureDataBuffer::TextureDataBuffer(const Texture& texture)
		: TextureDataBuffer(TextureManager::getTextureData(texture).textureDataType, TextureManager::getTextureData(texture).textureDataPrecision, TextureManager::getTextureData(texture).textureDataFormat, TextureManager::getTextureData(texture).size) {}
	//------------------------------------------------------------------------------------------------------
	TextureDataBuffer::TextureDataBuffer(const Texture& texture, const Vec2i& size)
		: TextureDataBuffer(TextureManager::getTextureData(texture).textureDataType, TextureManager::getTextureData(texture).textureDataPrecision, TextureManager::getTextureData(texture).textureDataFormat, size) {}
	//------------------------------------------------------------------------------------------------------
	std::optional<TextureDataBuffer> TextureDataBuffer::loadTextureDataBuffer2D(const std::string& path)
	{
		return std::move(TextureManager::loadTextureDataBuffer2D(path));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const float& value) 
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		setSingleValue(computeIndex(position), value);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const Vec2f& value)
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		unsigned int index = computeIndex(position);
		setSingleValue(index, value.x);
		setSingleValue(index + bytesPerElement, value.y);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const Vec3f& value)
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		unsigned int index = computeIndex(position);
		setSingleValue(index, value.x);
		setSingleValue(index + bytesPerElement, value.y);
		setSingleValue(index + 2 * bytesPerElement, value.z);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const Vec4f& value)
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		unsigned int index = computeIndex(position);
		setSingleValue(index, value.x);
		setSingleValue(index + bytesPerElement, value.y);
		setSingleValue(index + 2 * bytesPerElement, value.z);
		setSingleValue(index + 3 * bytesPerElement, value.w);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const int& value)
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		setSingleValue(computeIndex(position), value);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const Vec2i& value)
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		unsigned int index = computeIndex(position);
		setSingleValue(index, value.x);
		setSingleValue(index + bytesPerElement, value.y);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const Vec3i& value)
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		unsigned int index = computeIndex(position);
		setSingleValue(index, value.x);
		setSingleValue(index + bytesPerElement, value.y);
		setSingleValue(index + 2 * bytesPerElement, value.z);
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void TextureDataBuffer::setPixel(const Vec2i& position, const Vec4i& value)
	{
#ifdef _DEBUG
		if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y) {
			warningLogger << LOGGER::BEGIN << "Tried to set pixel in textureDatBuffer out of bounds!" << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		unsigned int index = computeIndex(position);
		setSingleValue(index, value.x);
		setSingleValue(index + bytesPerElement, value.y);
		setSingleValue(index + 2 * bytesPerElement, value.z);
		setSingleValue(index + 3 * bytesPerElement, value.w);
	}
	//------------------------------------------------------------------------------------------------------
}