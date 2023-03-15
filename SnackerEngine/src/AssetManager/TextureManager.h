#pragma once

#include "Graphics/Texture.h"
#include "Graphics/textureDataBuffer.h"
#include "Graphics/Texture.h"

#include <vector>
#include <queue>
#include <unordered_map>
#include <string_view>
#include <optional>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class TextureManager
	{
	private:
		/// This struct stores all the data that is necessary to store for each texture
		struct TextureData {
			TextureData() = default;
			// ID used for binding of the texture on the GPU
			unsigned int GPU_ID{};
			// Number of times this texture is referenced by other objects
			unsigned int referenceCount{};
			// If this is set to false, this texture will get deleted if referenceCount reaches zero
			bool persistent{};
			// This is set to false by standard and is set to true if an actual texture is stored!
			bool valid{};
			// file path of the texture. If the texture was not loaded from a file this should be an empty string.
			std::string path{};
			// Size of the texture in pixels
			Vec2i size{};
			// The type of the texture
			Texture::TextureType textureType{};
			// The type of the data stored in the texture		
			Texture::TextureDataType textureDataType{};
			// The format of the data stored in the texture (how many entries per pixel)
			Texture::TextureDataFormat textureDataFormat{};
			// The precision of the data stored in the texture
			Texture::TextureDataPrecision textureDataPrecision{};
			/// Destructor
			~TextureData();
			/// Move constructor and assignment operator
			TextureData(TextureData&& other) noexcept;
			TextureData& operator=(TextureData&& other) noexcept;

			/// Binds this texture
			void bind(const unsigned int& slot = 0) const;
			/// sets the variable textureDataFormat accordingly to the number of entries per pixel
			void determineTextureDataFormat(const int& nrComponents);
			/// Computes the number of components (channels)
			int determineNumberComponents() const;
		};
		/// We just use unsigned ints as TextureIDs. The IDs are also indices into the textureDataArray!
		using TextureID = unsigned int;
		/// Vector that stores all TextureData structs. TextureIDs are indices into this vector
		inline static std::vector<TextureData> textureDataArray;
		/// Queue of available (unused) TextureIDs
		inline static std::queue<TextureID> availableTextureIDs;
		/// Current maximal amount of Textures that we can have before we have to resize
		inline static TextureID maxTextures;
		/// Current count of Textures loaded
		inline static TextureID loadedTexturesCount;
		/// Used to look up textures using a path string (to prevent double loading)
		inline static std::unordered_map<std::string, TextureID> stringToTextureID;
		/// textureID of the default texture
		static constexpr TextureID defaultTexture{ 0 };
		/// path to the defaut texture (relative to resource directory)
		static constexpr std::string_view defaultTexturePath{ "textures/special/missing.png" };

		/// Delete the given texture. Is called by decreaseReferenceCount, if the reference count reaches zero
		static void deleteTexture(const TextureID& textureID);
		/// Returns a new textureID. The TextureData corresponding to this id can then be used to create a new texture.
		/// Potentially resizes the textureDataArray, in which case a warning will be printed
		static TextureID getNewTextureID();
		/// Fills a texture with data, data is stored at the given pointer. Must assure that the pointer
		/// actually points to a large enough data block.
		static void fillTexture2D(Texture& texture, const void* data, const bool& mip = true);
		/// Fills part of a texture with data, data is stored at the given pointer. Must assure that the pointer
		/// actually points to a large enough data block and that the data fits in the texture
		static void fillTexture2D(Texture& texture, const void* data, const Vec2i& offset, const Vec2i& size, const bool& mip = true);
		/// Resizes the given texture by deleting it and allocating a new texture. Texture has to be filled again
		// after this function is called
		static void resize(Texture& texture, const Vec2i& size, const bool& mip = true);
	protected:
		friend class Texture;
		friend class TextureDataBuffer;
		friend class AssetManager;
		friend class FrameBuffer;
		friend class FontManager;
		friend static bool loadTexture2D(TextureManager::TextureData& textureData, const std::string& fullPath);
		friend static void createTexture2D(TextureManager::TextureData& textureData, const bool& mip);
		friend static void createTextureCubemap(TextureManager::TextureData& textureData, const bool& mip);
		/// Initializes the textureManager
		static void initialize(const unsigned int& startingSize);
		/// Increases the reference count of the given texture
		static void increaseReferenceCount(const Texture& texture);
		/// Decreases the refrenece count of the given texture, and deletes its textureData struct if necessary
		static void decreaseReferenceCount(const Texture& texture);
		/// Binds a given texture to the given slot
		static void bind(const Texture& texture, const unsigned int& slot = 0);
		/// Unbinds all textures
		static void unbindAll();
		/// Terminates the TextureManager
		static void terminate();
		/// Tries to load a texture from a given path. Looks relative to the resources directory
		/// Returns the texture and true if successfull, and the missingTexture and false if not.
		static std::pair<Texture, bool> loadTexture2D(const std::string& path, const bool& persistent = false);
		/// Tries to load texture data from the GPU and store it in a TextureBufferObject
		static TextureDataBuffer getTextureDataFromGPU(Texture& texture, const int& mipLevel = 0);
		/// Tries to load texture data from the given path and store it in a TextureBufferObject
		static std::optional<TextureDataBuffer> loadTextureDataBuffer2D(const std::string& path);
		/// Tries to save a texture at a given path. Returns true on success
		static bool saveTexture2D(Texture& texture, const std::string& path, const bool& relativeToResourceDir = true);
		/// Creates a new texture with the given parameters
		static Texture createTexture(const Vec2i& dimensions, const Texture::TextureType& type = Texture::TextureType::TEXTURE2D,
			const Texture::TextureDataType& dataType = Texture::TextureDataType::UNSIGNED_BYTE, const Texture::TextureDataFormat& dataFormat = Texture::TextureDataFormat::RGB,
			const Texture::TextureDataPrecision& dataPrecision = Texture::TextureDataPrecision::PRECISION_NOT_SPECIFIED, const bool& mip = true);
		/// Creates a texture and fills it from the given buffer
		static Texture createTextureFromBuffer(TextureDataBuffer& buffer, const bool& mip);
		// Fills a texture with the given data. The data must match the textures size and dataType
		static void fillTexture2D(Texture& texture, TextureDataBuffer& textureDataBuffer, const bool& mip = true);
		// Fills a texture with the given data, placed at a given offset vector. The data must match the textures size and dataType
		static void fillTexture2D(Texture& texture, TextureDataBuffer& textureDataBuffer, const Vec2i& offset, const bool& mip = true);
		// Copies one texture to another one with a given offset
		static void copyTexture2D(const Texture& source, Texture& target, const Vec2i& offset = Vec2i{});
		/// Returns the size of a texture
		static Vec2i getSize(const Texture& texture);
		/// Returns a const reference to the textureData object corresponding to the given texture
		static const TextureData& getTextureData(const Texture& texture);
	public:
		/// Deleted constructor: this is a purely virtual class!
		TextureManager() = delete;
	};
	//------------------------------------------------------------------------------------------------------
}