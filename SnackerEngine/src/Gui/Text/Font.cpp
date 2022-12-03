
#include "Gui/Text/Font.h"
#include "Core/Log.h"
#include "Graphics/TextureDataBuffer.h"
#include "AssetManager/TextureManager.h"
#include "AssetManager/FontManager.h"
#include "core/Assert.h"

#include <msdfgen.h>
#include <msdfgen-ext.h>
#include "Gui/Text/Font.h"

namespace SnackerEngine
{
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
	Font::Font(const std::string& path)
		: fontID(FontManager::loadFont(path))
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
	Font::~Font()
	{
		FontManager::decreaseReferenceCount(*this);
	}
	//------------------------------------------------------------------------------------------------------
}