#pragma once

#include "Graphics/Texture.h"
#include "Math/Vec.h"
#include "Utility\Buffer.h"

#include <cstddef>
#include <vector>
#include <optional>
#include <bit>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// Class that works as a buffer for textureData. Each single pixel can be modified seperately
	class TextureDataBuffer : public Buffer
	{
		/// TextureManager needs friend access
		friend class TextureManager;
		Texture::TextureDataType textureDataType = Texture::TextureDataType::UNSIGNED_BYTE;
		Texture::TextureDataPrecision textureDataPrecision = Texture::TextureDataPrecision::PRECISION_NOT_SPECIFIED;
		Texture::TextureDataFormat textureDataFormat = Texture::TextureDataFormat::RGBA;
		/// Size of the buffer in pixels
		Vec2i size2D = Vec2i();
		/// Stride between neighbouring elements, in bytes
		unsigned int stride = 0;
		// padding in bytes after each row necessary for OpenGL texture Storage
		unsigned int padding = 0;
		/// Number of bytes per element (PRECISION_32: bytesPerElement = 4, PRECISION_16: bytesPerElement = 2,
		/// else: bytesPerElement = 1)
		unsigned int bytesPerElement = 1;
		/// Helper function that computes the index into the storageArray of a 2D position
		unsigned int computeIndex(const Vec2i& position) const;
		/// Helper function that sets a value starting at the given index. Automatically uses the correct precision
		void setSingleValue(const unsigned int& index, const int& value);
		void setSingleValue(const unsigned int& index, const float& value);
		/// Computes the size of the data storage, given the other member variables
		unsigned int computeDataStorageSize();
	public:
		/// Default constructor
		TextureDataBuffer() = default;
		/// Constructor using different enums to fully specify the type of texture
		TextureDataBuffer(const Texture::TextureDataType& textureDataType, const Texture::TextureDataPrecision& textureDataPrecision,
			const Texture::TextureDataFormat& textureDataFormat, const Vec2i& size);
		/// Constructor that creates a buffer having the same data types and size as a given texture
		TextureDataBuffer(const Texture& texture);
		/// Constructor that creates a buffer having the same data types as a given texture, but a different size
		TextureDataBuffer(const Texture& texture, const Vec2i& size);
		/// Sets a single pixel.
		template<typename T>
		void setPixel(const Vec2i& position, const T& value);
		/// Tries to load texture data from the given path and store it in a TextureBufferObject
		static std::optional<TextureDataBuffer> loadTextureDataBuffer2D(const std::string& path);
		/// Tries to load texture data from the raw data stored in the buffer, as if it was directly copied from a file.
		/// The filename has to be given, because the loading can switch between different compression techniques based
		/// on the file type.
		static std::optional<TextureDataBuffer> loadTextureDataBuffer2DFromRawData(const SnackerEngine::ConstantBufferView& bufferView, const std::string filename);
	};
	//------------------------------------------------------------------------------------------------------
}