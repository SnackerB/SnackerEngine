
#include "Gui/Text/Font.h"
#include "Core/Log.h"
#include "Graphics/TextureDataBuffer.h"
#include "AssetManager/TextureManager.h"
#include "AssetManager/FontManager.h"
#include "core/Assert.h"

#include <msdfgen.h>
#include <msdfgen-ext.h>
#include "Gui/Text/Font.h"
#include "Utility\Json.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType<Font>(const nlohmann::json& json)
	{
		return json.is_string();
	}
	//------------------------------------------------------------------------------------------------------
	template<> Font parseJSON(const nlohmann::json& json)
	{
		return Font(parseJSON<std::string>(json));
	}
	//------------------------------------------------------------------------------------------------------
	Font::Font(const FontID& fontID)
		: fontID(fontID)
	{
		FontManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Font::Font()
		: fontID(0) {}
	//------------------------------------------------------------------------------------------------------
	/// Helper function that takes a string in the format "folder1/folder2/folder3/data.suffix" and returns
	/// a string of the format "folder1/folder2/folder3/loaded/data"
	static std::string getLoadedFontPath(const std::string& path) {
		auto slashPos = path.find_last_of("/");
		std::string result = "";
		if (slashPos < SIZE_MAX)
			result = path.substr(0, slashPos + 1) + "loaded";
		else result = "loaded";
		auto pointPos = path.find_last_of(".");
		if (slashPos < SIZE_MAX && pointPos < SIZE_MAX && pointPos > slashPos)
			return result + path.substr(slashPos, pointPos - slashPos);
		else
			return "";
	}
	//------------------------------------------------------------------------------------------------------
	Font::Font(const std::string& path)
		: fontID(FontManager::loadFont(path, getLoadedFontPath(path)))
	{
		FontManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Font& Font::operator=(const Font& other) noexcept
	{
		FontManager::decreaseReferenceCount(*this);
		fontID = other.fontID;
		FontManager::increaseReferenceCount(*this);
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	Font::Font(const Font& other) noexcept
		: fontID(other.fontID)
	{
		FontManager::increaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
	Font& Font::operator=(Font&& other) noexcept
	{
		FontManager::decreaseReferenceCount(*this);
		fontID = other.fontID;
		other.fontID = 0;
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	Font::Font(Font&& other) noexcept
		: fontID(other.fontID)
	{
		other.fontID = 0;
	}
	//------------------------------------------------------------------------------------------------------
	void Font::bind(const unsigned int& slot) const
	{
		FontManager::bind(*this);
	}
	//------------------------------------------------------------------------------------------------------
	void Font::unBind()
	{
		FontManager::unbindAll();
	}
	//------------------------------------------------------------------------------------------------------
	Glyph Font::getGlyph(const Unicode& codepoint)
	{
		return FontManager::getGlyph(*this, codepoint);
	}
	//------------------------------------------------------------------------------------------------------
	double Font::getVerticalLineDistance()
	{
		// TODO: How do we compute this?
		return 0.0f;
	}
	//------------------------------------------------------------------------------------------------------
	double Font::getAdvance(Unicode first, Unicode second)
	{
		if (!FontManager::isValidGlyph(*this, first)) {
			first = FontManager::getFontData(*this).missingCharacterReplacement;
			if (!FontManager::isValidGlyph(*this, first)) return 0.0;
		}
		if (!FontManager::isValidGlyph(*this, second)) {
			second = FontManager::getFontData(*this).missingCharacterReplacement;
			if (!FontManager::isValidGlyph(*this, second)) return 0.0;
		}
		double result;
		if (FontManager::getFontData(*this).fontGeometry.getAdvance(result, first, second)) {
			return result;
		}
		return FontManager::getFontData(*this).fontGeometry.getGlyph(first)->getAdvance();
	}
	//------------------------------------------------------------------------------------------------------
	double Font::getPixelRange() const
	{
		return FontManager::getFontData(*this).pixelRange;
	}
	//------------------------------------------------------------------------------------------------------
	bool Font::isValid() const
	{
		return FontManager::getFontData(*this).valid;
	}
	//------------------------------------------------------------------------------------------------------
	Vec2i Font::getMsdfDims() const
	{
		return FontManager::getFontData(*this).msdfTexture.getSize();
	}
	//------------------------------------------------------------------------------------------------------
	double Font::getLineHeight() const
	{
		return FontManager::getFontData(*this).fontGeometry.getMetrics().lineHeight;
	}
	//------------------------------------------------------------------------------------------------------
	double Font::getAscender() const
	{
		return FontManager::getFontData(*this).fontGeometry.getMetrics().ascenderY;
	}
	//------------------------------------------------------------------------------------------------------
	double Font::getDescender() const
	{
		return FontManager::getFontData(*this).fontGeometry.getMetrics().descenderY;
	}
	//------------------------------------------------------------------------------------------------------
	Texture Font::getMsdfTexture() const
	{
		return FontManager::getFontData(*this).msdfTexture;
	}
	//------------------------------------------------------------------------------------------------------
	Font::~Font()
	{
		FontManager::decreaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
}