#include "AssetManager/TextureManager.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "Core/Engine.h"
#include "Graphics/FrameBuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <external/stb_image.h>
#include <External/stb_image_write.h>

namespace SnackerEngine
{
	//======================================================================================================
	// TextureData implementation
	//======================================================================================================
	TextureManager::TextureData::~TextureData()
	{
		if (valid) {
			GLCall(glDeleteTextures(1, &GPU_ID));
		}
	}
	//------------------------------------------------------------------------------------------------------
	TextureManager::TextureData::TextureData(TextureData&& other) noexcept
		: GPU_ID(other.GPU_ID), referenceCount(other.referenceCount), persistent(other.persistent),
		valid(other.valid), path(std::move(other.path)), size(other.size), textureType(other.textureType),
		textureDataType(other.textureDataType), textureDataFormat(other.textureDataFormat), 
		textureDataPrecision(other.textureDataPrecision)
	{
		other.GPU_ID = 0;
		other.valid = false;
	}
	//------------------------------------------------------------------------------------------------------
	TextureManager::TextureData& TextureManager::TextureData::operator=(TextureData&& other) noexcept
	{
		if (valid) {
			GLCall(glDeleteTextures(1, &GPU_ID));
		}
		GPU_ID = other.GPU_ID;
		referenceCount = other.referenceCount;
		persistent = other.persistent;
		valid = other.valid;
		path = std::move(other.path); 
		size = other.size;
		textureType = other.textureType;
		textureDataType = other.textureDataType;
		textureDataFormat = other.textureDataFormat;
		textureDataPrecision = other.textureDataPrecision;
		// Leave other in a well-defined state
		other.GPU_ID = 0;
		other.valid = false;
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::TextureData::bind(const unsigned int& slot) const
	{
		if (valid) {
			GLCall(glActiveTexture(GL_TEXTURE0 + slot));
			switch (textureType)
			{
			case SnackerEngine::Texture::TextureType::TEXTURE2D:
				GLCall(glBindTexture(GL_TEXTURE_2D, GPU_ID)); break;
			case SnackerEngine::Texture::TextureType::CUBEMAP:
				GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, GPU_ID));  break;
			default:
				break;
			}
		}
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::TextureData::determineTextureDataFormat(const int& nrComponents)
	{
		switch (nrComponents)
		{
			case 1:
		{
			textureDataFormat = Texture::TextureDataFormat::R;
			return;
		}
		case 2:
		{
			textureDataFormat = Texture::TextureDataFormat::RG;
			return;
		}
		case 3:
		{
			textureDataFormat = Texture::TextureDataFormat::RGB;
			return;
		}
		case 4:
		{
			textureDataFormat = Texture::TextureDataFormat::RGBA;
			return;
		}
		default:
		{
#ifdef _DEBUG
			warningLogger << LOGGER::BEGIN << "unknown texture data format: " << nrComponents << "Components in loaded texture!" << LOGGER::ENDL;
#endif // _DEBUG
			textureDataFormat = Texture::TextureDataFormat::RGB;
			return;
		}
		}
	}
	//------------------------------------------------------------------------------------------------------
	int TextureManager::TextureData::determineNumberComponents() const
	{
		switch (textureDataFormat)
		{
		case SnackerEngine::Texture::TextureDataFormat::RGB: return 3;
		case SnackerEngine::Texture::TextureDataFormat::RGBA: return 4;
		case SnackerEngine::Texture::TextureDataFormat::RG: return 2;
		case SnackerEngine::Texture::TextureDataFormat::R: return 1;
		default: return 0;
		}
	}
	//------------------------------------------------------------------------------------------------------
	//======================================================================================================
	// Helper functions for extracting GLenums
	//======================================================================================================
	/// Helper function that returns the textureDataFormat as GLenum
	GLenum getFormatGL(const Texture::TextureDataFormat& textureDataFormat)
	{
		switch (textureDataFormat)
		{
		case Texture::TextureDataFormat::RGB: return GL_RGB;
		case Texture::TextureDataFormat::RGBA: return GL_RGBA;
		case Texture::TextureDataFormat::RG: return GL_RG;
		case Texture::TextureDataFormat::R: return GL_RED;
		default: warningLogger << LOGGER::BEGIN << "Illegal texture data format" << LOGGER::ENDL; break;
		}
		return GL_RGB;
	}
	/// Helper function that returns the textureDataType as GLenum
	GLenum getTypeGL(const Texture::TextureDataType& textureDataType)
	{
		switch (textureDataType)
		{
		case Texture::TextureDataType::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
		case Texture::TextureDataType::FLOAT: return GL_FLOAT;
		default: warningLogger << LOGGER::BEGIN << "Illegal texture data type" << LOGGER::ENDL; break;
		}
		return GL_UNSIGNED_BYTE;
	}
	/// Helper function that returns the internal format of the texture as a GLenum
	GLenum getInternalFormatGL(const Texture::TextureDataPrecision& textureDataPrecision,
		const Texture::TextureDataFormat& textureDataFormat,
		const Texture::TextureDataType& textureDataType)
	{
		switch (textureDataPrecision)
		{
		case Texture::TextureDataPrecision::PRECISION_NOT_SPECIFIED: 
		{
			return getFormatGL(textureDataFormat);
		}
		case Texture::TextureDataPrecision::PRECISION_16:
		{
			switch (textureDataFormat)
			{
			case Texture::TextureDataFormat::R:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: return GL_R16;
				case Texture::TextureDataType::FLOAT: return GL_R16F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			case Texture::TextureDataFormat::RG:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: return GL_RG16;
				case Texture::TextureDataType::FLOAT: return GL_RG16F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			case  Texture::TextureDataFormat::RGB:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: return GL_RGB16;
				case Texture::TextureDataType::FLOAT: return GL_RGB16F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			case Texture::TextureDataFormat::RGBA:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: return GL_RGBA16;
				case Texture::TextureDataType::FLOAT: return GL_RGBA16F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			}
		}
		case Texture::TextureDataPrecision::PRECISION_32:
		{
			switch (textureDataFormat)
			{
			case Texture::TextureDataFormat::R:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				case Texture::TextureDataType::FLOAT: return GL_R32F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			case Texture::TextureDataFormat::RG:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				case Texture::TextureDataType::FLOAT: return GL_RG32F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			case  Texture::TextureDataFormat::RGB:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				case Texture::TextureDataType::FLOAT: return GL_RGB32F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			case Texture::TextureDataFormat::RGBA:
			{
				switch (textureDataType)
				{
				case Texture::TextureDataType::UNSIGNED_BYTE: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				case Texture::TextureDataType::FLOAT: return GL_RGBA32F;
				default: warningLogger << LOGGER::BEGIN << "Illegal combination of texture precision and texture format" << LOGGER::ENDL; return getFormatGL(textureDataFormat);
				}
			}
			}
		}
		default: break;
		}
		return getFormatGL(textureDataFormat);
	}
	/// Struct that stores all three GLenums needed for texture creation
	struct TextureEnumsGL {
		GLenum formatGL;
		GLenum dataTypeGL;
		GLenum internalFormatGL;
	};
	/// Helper function that wraps up the above three functions into a single call
	TextureEnumsGL getTextureEnumsGL(const Texture::TextureDataFormat& textureDataFormat,
		const Texture::TextureDataType& textureDataType, const Texture::TextureDataPrecision& textureDataPrecision)
	{
		return
		{
			getFormatGL(textureDataFormat),
			getTypeGL(textureDataType),
			getInternalFormatGL(textureDataPrecision, textureDataFormat, textureDataType),
		};
	}
	//======================================================================================================
	// Helper functions for loading and creating textures
	//======================================================================================================
	/// Returns true if the given string ends with the given suffix
	static bool endsWith(std::string_view str, std::string_view suffix)
	{
		return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
	}
	//------------------------------------------------------------------------------------------------------
	static bool loadTexture2D(TextureManager::TextureData& textureData, const std::string& fullPath)
	{
		// Determine data type (either float (hdr) or unsigned byte (non hdr))
		textureData.textureDataType = endsWith(fullPath, ".hdr") ? Texture::TextureDataType::FLOAT : Texture::TextureDataType::UNSIGNED_BYTE;
		// Try to load the texture
		int width, height, nrComponents;
		void* data = nullptr;
		if (textureData.textureDataType == Texture::TextureDataType::FLOAT) {
			data = stbi_loadf(fullPath.c_str(), &width, &height, &nrComponents, 0);
		}
		else {
			data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
		}
		if (!data) {
			warningLogger << LOGGER::BEGIN << "Texture failed to load at " << fullPath << LOGGER::ENDL;
			return false; // Texture failed to load
		}
		textureData.determineTextureDataFormat(nrComponents);
		GLCall(glGenTextures(1, &textureData.GPU_ID));
		GLCall(glBindTexture(GL_TEXTURE_2D, textureData.GPU_ID));
		GLenum internalFormat = getInternalFormatGL(textureData.textureDataPrecision, textureData.textureDataFormat, textureData.textureDataType);
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, getFormatGL(textureData.textureDataFormat), getTypeGL(textureData.textureDataType), data));
		if (textureData.textureDataType == Texture::TextureDataType::FLOAT) {
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
		else {
			// Only generate mipmap for non hdr textures (TODO: Why?)
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
		// Set remaining parameters
		textureData.size = Vec2i(width, height);
		textureData.path = fullPath;
		textureData.persistent = false;
		textureData.referenceCount = 0;
		textureData.valid = true;
		// Free data
		stbi_image_free(data);
		// Loaded texture successfully!
		return true;
	}
	//------------------------------------------------------------------------------------------------------
	static void createTexture2D(TextureManager::TextureData& textureData, const bool& mip)
	{
		GLCall(glGenTextures(1, &textureData.GPU_ID));
		GLCall(glBindTexture(GL_TEXTURE_2D, textureData.GPU_ID));
		GLenum internalFormat = getInternalFormatGL(textureData.textureDataPrecision, textureData.textureDataFormat, textureData.textureDataType);
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureData.size.x, textureData.size.y, 0, getFormatGL(textureData.textureDataFormat), getTypeGL(textureData.textureDataType), NULL));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		if (mip)
		{
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		}
		else
		{
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		}
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		textureData.valid = true;
	}
	//------------------------------------------------------------------------------------------------------
	static void createTextureCubemap(TextureManager::TextureData& textureData, const bool& mip)
	{
		GLCall(glGenTextures(1, &textureData.GPU_ID));
		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureData.GPU_ID));
		GLenum internalFormatGL = getInternalFormatGL(textureData.textureDataPrecision, textureData.textureDataFormat, textureData.textureDataType);
		GLenum formatGL = getFormatGL(textureData.textureDataFormat);
		GLenum typeGL = getTypeGL(textureData.textureDataType);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormatGL, textureData.size.x, textureData.size.y, 0, formatGL, typeGL, nullptr);
		}
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
		if (mip)
		{
			GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
			GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		}
		else
		{
			GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		}
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		textureData.valid = true;
	}
	//======================================================================================================
	// TextureManager implementation
	//======================================================================================================
	void TextureManager::deleteTexture(const TextureID& textureID)
	{
#ifdef _DEBUG
		if (textureID > maxTextures) {
			warningLogger << LOGGER::BEGIN << "tried to delete texture that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!textureDataArray[textureID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to delete texture that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		// Delete entries in path map
		stringToTextureID.erase(textureDataArray[textureID].path);
		// Reset and unload Texture
		textureDataArray[textureID] = TextureData();
		loadedTexturesCount--;
		availableTextureIDs.push(textureID);
	}
	//------------------------------------------------------------------------------------------------------
	TextureManager::TextureID TextureManager::getNewTextureID()
	{
		if (loadedTexturesCount >= maxTextures)
		{
			// Resize vector and add new available textureID slots accordingly. For now: double size everytime this happens and send warning!
			textureDataArray.resize(maxTextures * 2 + 1);
			for (TextureID id = maxTextures + 1; id <= 2 * maxTextures; ++id)
			{
				availableTextureIDs.push(id);
			}
			warningLogger << LOGGER::BEGIN << "maximum amount of textures exceeded. Resizing textureManager to be able to store "
				<< 2 * maxTextures << " textures!" << LOGGER::ENDL;
			maxTextures *= 2;
		}
		// Take ID from the front of the queue
		TextureID id = availableTextureIDs.front();
		availableTextureIDs.pop();
		++loadedTexturesCount;
		return id;
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::fillTexture2D(Texture& texture, const void* data, const bool& mip)
	{
		const TextureData& textureData = textureDataArray[texture.textureID];
		textureData.bind();
		GLCall(glTextureSubImage2D(textureData.GPU_ID, 0, 0, 0, textureData.size.x, textureData.size.y, getFormatGL(textureData.textureDataFormat), getTypeGL(textureData.textureDataType), data));
		if (mip) {
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::fillTexture2D(Texture& texture, const void* data, const Vec2i& offset, const Vec2i& size, const bool& mip)
	{
		const TextureData& textureData = textureDataArray[texture.textureID];
		textureData.bind();
		GLCall(glTextureSubImage2D(textureData.GPU_ID, 0, offset.x, offset.y, size.x, size.y, getFormatGL(textureData.textureDataFormat), getTypeGL(textureData.textureDataType), data));
		if (mip) {
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::resize(Texture& texture, const Vec2i& size, const bool& mip)
	{
		TextureData& textureData = textureDataArray[texture.textureID];
		if (textureData.valid) {
			GLCall(glDeleteTextures(1, &textureData.GPU_ID));
		}
		textureData.size = size;
		if (textureData.textureType == Texture::TextureType::CUBEMAP) {
			createTextureCubemap(textureData, mip);
		}
		else {
			createTexture2D(textureData, mip);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::initialize(const unsigned int& startingSize)
	{
		// Initializes queue with all possible textureIDs. textureID = 0 is reserved for missing textures.
		for (TextureID id = 1; id <= startingSize; ++id)
		{
			availableTextureIDs.push(id);
		}
		// textureDataArray[0] stores the default texture
		textureDataArray.resize(startingSize + 1);
		maxTextures = startingSize;
		// Load default texture!
		std::string fullPath = Engine::getResourcePath();
		fullPath.append(defaultTexturePath);
		if (!SnackerEngine::loadTexture2D(textureDataArray[0], fullPath)) {
			errorLogger << LOGGER::BEGIN << "Could not load default texture" << LOGGER::ENDL;
		}
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::increaseReferenceCount(const Texture& texture)
	{
		if (texture.textureID == 0)
			return;
#ifdef _DEBUG
		if (texture.textureID > maxTextures) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of texture that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!textureDataArray[texture.textureID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of texture that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		textureDataArray[texture.textureID].referenceCount++;
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::decreaseReferenceCount(const Texture& texture)
	{
		if (texture.textureID == 0)
			return;
#ifdef _DEBUG
		if (texture.textureID > maxTextures) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of texture that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!textureDataArray[texture.textureID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of texture that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		textureDataArray[texture.textureID].referenceCount--;
		if (textureDataArray[texture.textureID].referenceCount == 0 && !textureDataArray[texture.textureID].persistent) {
			deleteTexture(texture.textureID);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::bind(const Texture& texture, const unsigned int& slot)
	{
#ifdef _DEBUG
		if (texture.textureID > maxTextures) {
			warningLogger << LOGGER::BEGIN << "tried to bind texture that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!textureDataArray[texture.textureID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to bind texture that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		textureDataArray[texture.textureID].bind(slot);
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::unbindAll()
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::terminate()
	{
		/// Clears the textureDataArray, deleting all textures from the GPU!
		textureDataArray.clear();
	}
	//------------------------------------------------------------------------------------------------------
	std::pair<Texture, bool> TextureManager::loadTexture2D(const std::string& path, const bool& persistent)
	{
		// First, look if texture is already loaded!
		auto it = stringToTextureID.find(path);
		if (it != stringToTextureID.end()) {
			return std::make_pair(Texture(it->second), true);
		}
		// We did not find it, load it from file!
		std::string fullPath = Engine::getResourcePath(); 
		fullPath.append(path);
		// Important so the image gets loaded the way OpenGL expects.
		stbi_set_flip_vertically_on_load(true);
		// Get new textureData object
		TextureID textureID = getNewTextureID();
		TextureData& textureData = textureDataArray[textureID];
		textureData.textureType = Texture::TextureType::TEXTURE2D;
		if (!SnackerEngine::loadTexture2D(textureData, fullPath)) {
			return std::make_pair(Texture(defaultTexture), false);
		}
		// Texture was successfully loaded!
		stringToTextureID[fullPath] = textureID;
		return std::make_pair(Texture(textureID), true);
	}
	//------------------------------------------------------------------------------------------------------
	TextureDataBuffer TextureManager::getTextureDataFromGPU(Texture& texture, const int& mipLevel)
	{
		TextureDataBuffer buffer(texture);
		texture.bind();
		GLCall(glGetTexImage(GL_TEXTURE_2D, mipLevel, getFormatGL(buffer.textureDataFormat), getTypeGL(buffer.textureDataType), buffer.getDataPointer()));
		return buffer;
	}
	//------------------------------------------------------------------------------------------------------
	std::optional<TextureDataBuffer> TextureManager::loadTextureDataBuffer2D(const std::string& path)
	{
		std::string fullPath = Engine::getResourcePath();
		fullPath.append(path);
		// Important so the image gets loaded the way OpenGL expects.
		stbi_set_flip_vertically_on_load(true);

		// Determine data type (either float (hdr) or unsigned byte (non hdr))
		TextureManager::TextureData textureData{};
		textureData.textureDataType = endsWith(fullPath, ".hdr") ? Texture::TextureDataType::FLOAT : Texture::TextureDataType::UNSIGNED_BYTE;
		// Try to load the texture
		int width, height, nrComponents, bytesPerPixel;
		void* data = nullptr;
		if (textureData.textureDataType == Texture::TextureDataType::FLOAT) {
			data = stbi_loadf(fullPath.c_str(), &width, &height, &nrComponents, 0);
			bytesPerPixel = sizeof(float);
		}
		else {
			data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
			bytesPerPixel = sizeof(stbi_uc);
		}
		if (!data) {
			warningLogger << LOGGER::BEGIN << "Texture failed to load at " << fullPath << LOGGER::ENDL;
			return {}; // Texture failed to load
		}
		textureData.determineTextureDataFormat(nrComponents);
		textureData.size = Vec2i(width, height);
		TextureDataBuffer buffer(textureData.textureDataType, textureData.textureDataPrecision, textureData.textureDataFormat, textureData.size);
		std::memcpy(buffer.getDataPointer(), data, std::min(buffer.dataSize, static_cast<unsigned int>(width * height * nrComponents * bytesPerPixel)));
		// Free data
		stbi_image_free(data);
		// Loaded texture successfully!
		return std::move(buffer);
	}
	//------------------------------------------------------------------------------------------------------
	bool TextureManager::saveTexture2D(Texture& texture, const std::string& path, const bool& relativeToResourceDir)
	{
		std::string fullpath;
		if (relativeToResourceDir) {
			fullpath = Engine::getResourcePath();
		}
		fullpath.append(path);
		TextureDataBuffer buffer = getTextureDataFromGPU(texture);
		// Important so the image gets saved correctly (OpenGL detail)
		stbi_flip_vertically_on_write(true);
		const Vec2i& size = texture.getSize();
		return stbi_write_png(fullpath.c_str(), size.x, size.y, textureDataArray[texture.textureID].determineNumberComponents(), buffer.getDataPointer(), buffer.stride * size.x) == 1;
	}
	//------------------------------------------------------------------------------------------------------
	Texture TextureManager::createTexture(const Vec2i& dimensions, const Texture::TextureType& type, const Texture::TextureDataType& dataType, const Texture::TextureDataFormat& dataFormat, const Texture::TextureDataPrecision& dataPrecision, const bool& mip)
	{
		TextureID textureID = getNewTextureID();
		TextureData& textureData = textureDataArray[textureID];
		textureData.size = dimensions;
		textureData.textureType = type;
		textureData.textureDataType = dataType;
		textureData.textureDataFormat = dataFormat;
		textureData.textureDataPrecision = dataPrecision;
		if (textureData.textureType == Texture::TextureType::CUBEMAP) {
			createTextureCubemap(textureData, mip);
		}
		else {
			createTexture2D(textureData, mip);
		}
		return Texture(textureID);
	}
	//------------------------------------------------------------------------------------------------------
	Texture TextureManager::createTextureFromBuffer(TextureDataBuffer& buffer, const bool& mip)
	{
		// TODO: Make this work with cubemaps as well
		Texture texture = createTexture(buffer.size, Texture::TextureType::TEXTURE2D, buffer.textureDataType, buffer.textureDataFormat, buffer.textureDataPrecision);
		texture.fill2D(buffer, mip);
		return texture;
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::fillTexture2D(Texture& texture, TextureDataBuffer& textureDataBuffer, const bool& mip)
	{
		fillTexture2D(texture, textureDataBuffer, Vec2i(0), mip);
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::fillTexture2D(Texture& texture, TextureDataBuffer& textureDataBuffer, const Vec2i& offset, const bool& mip)
	{
		const TextureData& textureData = textureDataArray[texture.textureID];
		if (textureData.textureDataType != textureDataBuffer.textureDataType ||
			textureData.textureDataPrecision != textureDataBuffer.textureDataPrecision ||
			textureData.textureDataFormat != textureDataBuffer.textureDataFormat ||
			textureData.size != textureDataBuffer.size) {
			warningLogger << LOGGER::BEGIN << "Could not fill texture: Buffer and textureData do not match" << LOGGER::ENDL;
			return;
		}
		fillTexture2D(texture, textureDataBuffer.getDataPointer(), offset, textureDataBuffer.size, mip);
	}
	//------------------------------------------------------------------------------------------------------
	void TextureManager::copyTexture2D(const Texture& source, Texture& target, const Vec2i& offset)
	{
		Vec2i sourceSize = source.getSize();
		Vec2i targetSize = target.getSize();
		if (sourceSize.x + offset.x > targetSize.x || sourceSize.y + offset.y > targetSize.y) {
			warningLogger << LOGGER::BEGIN << "tried to copy 2D texture into texture that was too small" << LOGGER::ENDL;
			return;
		}
		FrameBuffer framebuffer(sourceSize, false, false);
		framebuffer.attachTexture(source);
		framebuffer.finalize();
		framebuffer.bindAsTarget();
		target.bind();
		GLCall(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, 0, 0, sourceSize.x, sourceSize.y));
		framebuffer.unbindAllTargets();
	}
	//------------------------------------------------------------------------------------------------------
	Vec2i TextureManager::getSize(const Texture& texture)
	{
		if (texture.textureID > maxTextures || !textureDataArray[texture.textureID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to get size of invalid texture ..." << LOGGER::ENDL;
			return Vec2i(0, 0);
		}
		else {
			return textureDataArray[texture.textureID].size;
		}
	}
	//------------------------------------------------------------------------------------------------------
	const TextureManager::TextureData& TextureManager::getTextureData(const Texture& texture)
	{
		return textureDataArray[texture.textureID];
	}
	//------------------------------------------------------------------------------------------------------
}