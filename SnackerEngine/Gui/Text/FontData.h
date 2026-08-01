#pragma once

#include "Graphics\Texture.h"
#include "AssetManager\FontManager.h"

#include <msdf-atlas-gen.h>

namespace SnackerEngine
{

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
}