#pragma once

#include "Graphics/Texture.h"
#include "GUI/Text/Font.h"
#include "Gui/Text/Glyph.h"

#include <msdf-atlas-gen.h>
#include <queue>
#include <unordered_map>
#include <unordered_set>

// Forward declarations
namespace msdfgen {
	class FreetypeHandle;
	class FontHandle;
}
class DynamicAtlas;

namespace SnackerEngine
{

	class FontManager
	{
	private:
		/// We use unsigned as unicode type
		using Unicode = unsigned int;
		/// specifies the parameters of the atlas generator
		/// 1st template argument: pixel type of buffer for individual glyphs depends on generator function
		/// 2nd template argument: number of atlas color channels
		/// 3rd template argument: function to generate bitmaps for individual glyphs
		/// 4th template argument: class that stores the atlas bitmap
		using AtlasGenerator = msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>>;
		using DynamicAtlas = msdf_atlas::DynamicAtlas<AtlasGenerator>;
		/// This struct stores all the data that is necessary to store for each font
		struct FontData
		{
			FontData() = default;
			/// move constructor and assignment operator
			FontData(FontData&& other) = default;
			FontData& operator=(FontData&& other) = default;
			/// msdf texture
			Texture msdfTexture;
			/// Number of times this font is referenced by other objects
			unsigned int referenceCount{};
			/// If this is set to false, this font will get deleted if referenceCount reaches zero
			bool persistent{};
			/// This is set to false by standard and is set to true if an actual font is stored!
			bool valid{};
			/// File path of the font
			std::string path{};
			/// parameters used in msdf generation
			double maxCornerAngle{};
			double glyphScale{};
			double pixelRange{};
			double miterLimit{};
			/// Unicode symbol that is used as a replacement for missing/unknowns characters
			Unicode missingCharacterReplacement{};
			/// msdf atlas
			DynamicAtlas dynamicAtlas{};
			/// Vector of glyphs, stores important geometry information about each glyph
			std::vector<msdf_atlas::GlyphGeometry> glyphs{};
			/// Font geometry object, used to access specific glyphs
			msdf_atlas::FontGeometry fontGeometry{};
			/// Set of Unicode symbols that are represented by the "missing" symbol,
			/// ie. symbols that could not be loaded with this font
			std::unordered_set<Unicode> missingCharacters{};
			/// Destructor. Deletes the texture from the GPU if it was loaded.
			~FontData();

			/// Binds this font
			void bind(const unsigned int& slot = 0);
		};
		/// We just use unsigned ints as fontIDs. The IDs are also indices into the fontDataArray!
		using FontID = unsigned int;
		/// Freetype handle
		inline static msdfgen::FreetypeHandle* freetypeHandle;
		/// Vector that stores all FontData structs. FontIDs are indices into this vector
		inline static std::vector<FontData> fontDataArray;
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

		/// Helper function to setup the missing character codepoint for a 
		/// newly loaded font
		static void setupMissingCharacterReplacement(const FontID& fontID);
		/// Delete the given font. Is called by decreaseReferenceCount, if the reference count reaches zero
		static void deleteFont(const FontID& fontID);
		/// Returns a new fontID. The FontData corresponding to this id can then be used to create a new font.
		/// Potentially resizes the fontDataArray, in which case a warning will be printed
		static FontID getNewFontID();
		/// Loads a new font from a file and stores it in the given slot in the fontDataArray. Returns true on success.
		static bool loadNewFont(const std::string& path, const FontID& fontID);
		/// adds a new glyph with the given codepoint to the font. Does not check if the codepoint was already added before.
		/// Returns true on success
		static bool addNewGlyph(const Unicode& codepoint, const FontID& fontID);
		/// Returns a glyph from the given font. If the glyph wasn't loaded yet it is loaded from the font file.
		/// If the glyph can't be loaded, a missing character glyph is returned instead and a warning is printed.
		static Glyph getGlyph(Font& font, const Unicode& codepoint);
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
		static FontID loadFont(const std::string& path);
		/// Terminates the fontManager
		static void terminate();
		/// Returns a const refrence to the corresponding FontData object
		static const FontData& getFontData(const Font& font);
		/// Checks if the given codepoint was loaded already / corresponds to a valid glyph
		static bool isValidGlyph(const Font& font, const Unicode& codepoint);
	public:
		/// Deleted constructor: this is a purely virtual class!
		FontManager() = delete;
	};
}