#pragma once

#include "Math/Vec.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// Forward declaration of TextureDataBuffer
	class TextureDataBuffer;
	//------------------------------------------------------------------------------------------------------
	class Texture {
	public:
		//--------------------------------------------------------------------------------------------------
		/// The type of the texture. Can be TEXTURE2D or CUBEMAP
		enum class TextureType {
			TEXTURE2D,
			CUBEMAP,
		};
		//--------------------------------------------------------------------------------------------------
		/// The type of the data stored in the texture
		enum class TextureDataType {
			UNSIGNED_BYTE,
			FLOAT,
		};
		//--------------------------------------------------------------------------------------------------
		/// The format of the data stored in the texture (how many entries per pixel)
		enum class TextureDataFormat {
			RGB,
			RGBA,
			RG,
			R,
		};
		//--------------------------------------------------------------------------------------------------
		/// The precision of the data stored in the texture
		enum class TextureDataPrecision {
			PRECISION_NOT_SPECIFIED,
			PRECISION_16,
			PRECISION_32,
		};
		//--------------------------------------------------------------------------------------------------
	private:
		using TextureID = unsigned int;
		friend class TextureManager;
		friend class FrameBuffer;
		TextureID textureID;
		Texture(TextureID textureID);	
	public:
		//// Default constructor
		Texture();
		/// Copy constructor and assignment operator
		Texture& operator=(const Texture& other) noexcept;
		Texture(const Texture& other) noexcept;
		/// Binds this texture to the given slot
		void bind(const unsigned int& slot = 0);
		/// Unbinds all textures
		static void unBind();
		/// Returns the size of this texture
		Vec2i getSize() const;
		/// Checks wether this texture is validand corresponds to actual data on the GPU
		bool isValid() const;
		
		/// Destructor
		~Texture();
		//==================================================================================================
		/// Special constructors
		//==================================================================================================
		/// Tries to load a texture from a given path. Looks relative to the resources directory
		/// Returns the texture and true if successfull, and the missingTexture and false if not.
		static std::pair<Texture, bool> Load2D(const std::string& path, const bool& persistent = false);
		/// Creates a new 2D texture with the given dimensions and dataFormat. 
		static Texture Create2D(const Vec2i& dimensions,
			const TextureDataFormat& dataFormat = TextureDataFormat::RGB, const bool& mip = true);
		/// Creates a new 2D floating point (hdr) texture with the given dimensions and dataFormat. Uses a higher precision
		static Texture CreateFloat2D(const Vec2i& dimensions,
			const TextureDataFormat& dataFormat = TextureDataFormat::RGB, const bool& mip = true);
		/// Creates a new cubemap texture with the given dimensions and dataFormat. 
		static Texture CreateFloatCubemap(const Vec2i& dimensions,
			const TextureDataFormat& dataFormat = TextureDataFormat::RGB, const bool& mip = true);
		static Texture CreateFromBuffer(TextureDataBuffer& buffer, const bool& mip = true);
		//==================================================================================================
		// Functions manipulating the texture
		//==================================================================================================
		/// Fills this texture with the given data. The data must match the textures size and dataType
		void fill2D(TextureDataBuffer& textureDataBuffer, const bool& mip = true);
		/// Fills this texture with the given data, placed at a given offset vector. The data must match the textures size and dataType
		void fill2D(TextureDataBuffer& textureDataBuffer, const Vec2i& offset, const bool& mip = true);
		/// Copies one texture to another one with a given offset
		static void copyTexture2D(const Texture& source, Texture& target, const Vec2i& offset = Vec2i{});
		//==================================================================================================
		// Miscellaneous
		//==================================================================================================
		bool saveInFile(const std::string& path, const bool& relativeToResourcePath = true);
	};
	//------------------------------------------------------------------------------------------------------
}