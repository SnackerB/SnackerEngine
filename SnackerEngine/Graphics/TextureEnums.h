#pragma once

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// The type of the texture. Can be TEXTURE2D or CUBEMAP
	enum class TextureType {
		TEXTURE2D,
		CUBEMAP,
	};
	//------------------------------------------------------------------------------------------------------
	/// The type of the data stored in the texture
	enum class TextureDataType {
		UNSIGNED_BYTE,
		FLOAT,
	};
	//------------------------------------------------------------------------------------------------------
	/// The format of the data stored in the texture (how many entries per pixel)
	enum class TextureDataFormat {
		RGB,
		RGBA,
		RG,
		R,
	};
	//------------------------------------------------------------------------------------------------------
	/// The precision of the data stored in the texture
	enum class TextureDataPrecision {
		PRECISION_NOT_SPECIFIED,
		PRECISION_16,
		PRECISION_32,
	};
	//------------------------------------------------------------------------------------------------------
}