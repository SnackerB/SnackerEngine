#pragma once

#include "Graphics/Texture.h"
#include "GUI/Text/Font.h"
#include "Gui/Text/Glyph.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>

// Forward declarations
namespace msdfgen {
	class FreetypeHandle;
	class FontHandle;
}

namespace SnackerEngine
{
	/// Forward declaration of struct that stores all the data that is necessary to store for each font
	struct FontData;

	class FontManager
	{
	private:
		/// We use unsigned as unicode type
		using Unicode = unsigned int;
		/// We just use unsigned ints as fontIDs. The IDs are also indices into the fontDataArray!
		using FontID = unsigned int;
		/// Freetype handle
		inline static msdfgen::FreetypeHandle* freetypeHandle;
		/// Vector that stores all fontHandle objects. FontIDs are indices into this vector
		inline static std::vector<msdfgen::FontHandle*> fontHandles;
		/// Queue of available (unused) FontIDs
		inline static std::queue<FontID> availableFontIDs;
		/// Current maximal amount of Fonts that we can have before we have to resize
		inline static FontID maxFonts;
		/// Current count of Fonts loaded
		inline static FontID loadedFontsCount;
		/// Used to look up Fonts using a path string (to prevent double loading)
		inline static std::unordered_map<std::string, FontID> stringToFontID;
		/// If this is set to true, all fonts are loaded persistently, i.e. never unloaded
		inline static bool loadFontsPersistently;

		/// Helper function to setup the missing character codepoint for a 
		/// newly loaded font
		static void setupMissingCharacterReplacement(const FontID& fontID);
		/// Delete the given font. Is called by decreaseReferenceCount, if the reference count reaches zero
		static void deleteFont(const FontID& fontID);
		/// Returns a new fontID. The FontData corresponding to this id can then be used to create a new font.
		/// Potentially resizes the fontDataArray, in which case a warning will be printed
		static FontID getNewFontID();
		/// Loads a new font from a file and stores it in the given slot in the fontDataArray. Returns true on success.
		/// If loadDefaultCharset is set to true, a default char set consisting of typical ASCII characters is loaded.
		static bool loadNewFont(const std::string& path, const FontID& fontID);
		/// Loads the default, consisting of typical ASCII characters, into the given font
		static void loadDefaultCharset(const FontID& fontID);
		/// adds a new glyph with the given codepoint to the font. Does not check if the codepoint was already added before.
		/// Returns true on success
		static bool addNewGlyph(const Unicode& codepoint, const FontID& fontID);
		/// Returns a glyph from the given font. If the glyph wasn't loaded yet it is loaded from the font file.
		/// If the glyph can't be loaded, a missing character glyph is returned instead and a warning is printed.
		static Glyph getGlyph(Font& font, const Unicode& codepoint);
		/// Tries to load a fontData object from a given file path
		//bool loadFontDataFromFile(const std::string& path);
		/// Tries to save a fontData object in the given file path
		//bool saveFontDataInFile(const Font& font, const std::string& path);
	protected:
		friend class Font;
		friend class AssetManager;
		/// Initializes the fontManager
		static void initialize(const unsigned int& startingSize);
		/// Increases the reference count of the given font
		static void increaseReferenceCount(const Font& font);
		/// Decreases the refrenece count of the given font, and deletes its fontData struct if necessary
		static void decreaseReferenceCount(const Font& font);
		/// Binds a given font
		static void bind(const Font& font);
		/// Unbinds all fonts
		static void unbindAll();
		/// Loads a font from a file. If the font is already loaded, the fontID of the existing font is returned!
		/// If existingFontDataPath is not equal to the empty string, it is attempted to load previously saved 
		/// font metrics and msdf texture at the given path. If this fails, we fall back to the default routine.
		static FontID loadFont(const std::string& path, const std::string& existingFontDataPath);
		/// Terminates the fontManager
		static void terminate();
		/// Returns a const refrence to the corresponding FontData object
		static const FontData& getFontData(const Font& font);
		/// Checks if the given codepoint was loaded already / corresponds to a valid glyph
		static bool isValidGlyph(const Font& font, const Unicode& codepoint);
	public:
		/// Deleted constructor: this is a purely virtual class!
		FontManager() = delete;
		/// Sets the loadFontsPersistently bool variable. If this is set to true, all fonts are loaded persistently
		/// per default, i.e. they are only deleted when the engine terminates.
		static void setLoadFontsPersistently(bool loadFontsPersistently) { FontManager::loadFontsPersistently = loadFontsPersistently; }
		static bool isLoadFontsPersistently() { return loadFontsPersistently; }

		// DEBUG
		/// Tries to load a fontData object from a given file path
		static bool loadFontDataFromFile(const std::string& path, const FontID& fontID);
		/// Tries to save a fontData object in the given file path
		static bool saveFontDataInFile(const FontID& fontID, const std::string& path);
	};
}