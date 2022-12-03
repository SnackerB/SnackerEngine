
#include "Gui/Text/Font.h"
#include "Core/Log.h"
#include "Graphics/TextureDataBuffer.h"
#include "AssetManager/TextureManager.h"
#include "Core/Assert.h"
#include "AssetManager/FontManager.h"
#include "Core/Engine.h"
#include "Gui/Text/Unicode.h"

#include <msdfgen.h>
#include <msdfgen-ext.h>
#include <msdf-atlas-gen.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <External/json.hpp>
#include <fstream>

namespace SnackerEngine
{
    //======================================================================================================
    // FontData implementation
    //======================================================================================================
    FontManager::FontData::~FontData() { }
    //------------------------------------------------------------------------------------------------------
    void FontManager::FontData::bind(const unsigned int& slot)
    {
        msdfTexture.bind(slot);
    }
    //======================================================================================================
    // Helper functions for loading and updating fonts
    //======================================================================================================

    void FontManager::setupMissingCharacterReplacement(const FontID& fontID)
    {
        if (addNewGlyph(0x25a1, fontID)) { // White square
            fontDataArray[fontID].missingCharacterReplacement = 0x25a1;
        }
        else if (addNewGlyph(0xfffd, fontID)) { // Black diamond with question mark
            fontDataArray[fontID].missingCharacterReplacement = 0xfffd;
        }
        else if (addNewGlyph(0x25a1, fontID)) { // White square small
            fontDataArray[fontID].missingCharacterReplacement = 0x25ab;
        }
        else if (addNewGlyph(0x3f, fontID)) { // Question mark
            fontDataArray[fontID].missingCharacterReplacement = 0x3f;
        }
        else {
            errorLogger << LOGGER::BEGIN << "Could not load a codepoint for replacing invalid codepoints using font " <<
                fontDataArray[fontID].path << "!" << LOGGER::ENDL;
        }
    }

    //======================================================================================================
    // FontManager implementation
    //======================================================================================================
    void FontManager::deleteFont(const FontID& fontID)
    {
#ifdef _DEBUG
        if (fontID > maxFonts) {
            warningLogger << LOGGER::BEGIN << "tried to delete font that does not exist ..." << LOGGER::ENDL;
            return;
        }
        else if (!fontDataArray[fontID].valid) {
            warningLogger << LOGGER::BEGIN << "tried to delete font that was not valid ..." << LOGGER::ENDL;
            return;
        }
#endif // _DEBUG

        // Delete entries in path map
        stringToFontID.erase(fontDataArray[fontID].path);
        // Reset and unload fontData
        fontDataArray[fontID] = std::move(FontData());
        loadedFontsCount--;
        availableFontIDs.push(fontID);
        // Destroy fontHandle
        if (fontHandles[fontID]) {
            msdfgen::destroyFont(fontHandles[fontID]);
            fontHandles[fontID] = nullptr;
        }
    }
    //------------------------------------------------------------------------------------------------------
    FontManager::FontID FontManager::getNewFontID()
    {
        if (loadedFontsCount >= maxFonts)
        {
            // Resize vector and add new available shaderID slots accordingly. For now: double size everytime this happens and send warning!
            fontDataArray.resize(static_cast<std::size_t>(maxFonts) * 2 + 1);
            for (FontID id = maxFonts + 1; id <= 2 * maxFonts; ++id)
            {
                availableFontIDs.push(id);
            }
            warningLogger << LOGGER::BEGIN << "maximum amount of shaders exceeded. Resizing shaderManager to be able to store "
                << 2 * maxFonts << " shaders!" << LOGGER::ENDL;
            maxFonts *= 2;
        }
        // Take ID from the front of the queue
        FontID id = availableFontIDs.front();
        availableFontIDs.pop();
        ++loadedFontsCount;
        return id;
    }
    //------------------------------------------------------------------------------------------------------
    bool FontManager::loadNewFont(const std::string& path, const FontID& fontID)
    {
        fontHandles[fontID] = msdfgen::loadFont(freetypeHandle, path.c_str());
        if (!fontHandles[fontID]) {
            errorLogger << LOGGER::BEGIN << "font could not be loaded at " << path << LOGGER::ENDL;
            return false;
        }
        // Storage for glyph geometry and their coordinates in the atlas:
        // fontDataArray[fontID].glyphs
        // FontGeometry is a helper class that loads a set of glyphs from a single font.
        // It can also be used to get additional font metrics, kerning information, etc.
        fontDataArray[fontID].glyphs.clear();
        fontDataArray[fontID].fontGeometry = msdf_atlas::FontGeometry(&fontDataArray[fontID].glyphs);
        fontDataArray[fontID].dynamicAtlas = DynamicAtlas();
        // specifies the maximum angle to be considered a corner in radians
        fontDataArray[fontID].maxCornerAngle = 3.0;
        // glyph scale
        fontDataArray[fontID].glyphScale = 64.0;
        // specifies the width of the range around the shape between the minimum and maximum representable signed 
        // distance in shape units or distance field pixels, respectivelly
        fontDataArray[fontID].pixelRange = 2.0;
        // limits the extension of each glyph's bounding box due to very sharp corners (
        fontDataArray[fontID].miterLimit = 1.0;
        // Temporarily set the default texture as msdf texture
        fontDataArray[fontID].msdfTexture = Texture();
        // Success! We can set the font to valid!
        fontDataArray[fontID].valid = true;
        return true;
    }
    //------------------------------------------------------------------------------------------------------
    void FontManager::loadDefaultCharset(const FontID& fontID)
    {
        // Load a set of character glyphs:
        // The second argument can be ignored unless you mix different font sizes in one atlas.
        // In the last argument, you can specify a charset other than ASCII.
        // To load specific glyph indices, use loadGlyphs instead.
        fontDataArray[fontID].fontGeometry.loadCharset(fontHandles[fontID], 1.0f, msdf_atlas::Charset::ASCII);
        // Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
        for (msdf_atlas::GlyphGeometry& glyph : fontDataArray[fontID].glyphs) {
            glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, fontDataArray[fontID].maxCornerAngle, 0);
            glyph.wrapBox(fontDataArray[fontID].glyphScale, fontDataArray[fontID].pixelRange / fontDataArray[fontID].glyphScale, fontDataArray[fontID].miterLimit);
        }
        fontDataArray[fontID].dynamicAtlas.add(fontDataArray[fontID].glyphs.data(), static_cast<int>(fontDataArray[fontID].glyphs.size()));

        msdfgen::BitmapConstRef<msdf_atlas::byte, 3>& storageRef = (msdfgen::BitmapConstRef<msdf_atlas::byte, 3>&)fontDataArray[fontID].dynamicAtlas.atlasGenerator().atlasStorage();
        fontDataArray[fontID].msdfTexture = Texture::Create2D(Vec2i(storageRef.width, storageRef.height));
        GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); // disable byte-alignment restriction
        TextureManager::fillTexture2D(fontDataArray[fontID].msdfTexture, storageRef.pixels);
        GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4)); // enable byte-alignment restriction
    }
    //------------------------------------------------------------------------------------------------------
    bool FontManager::addNewGlyph(const Unicode& codepoint, const FontID& fontID)
    {
        // Load the glyph
        msdf_atlas::GlyphGeometry glyph;
        if (!glyph.load(fontHandles[fontID], fontDataArray[fontID].fontGeometry.getGeometryScale(), codepoint)) {
            return false;
        }
        // Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
        glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, fontDataArray[fontID].maxCornerAngle, 0);
        // Finalize glyph box size based on the parameters
        glyph.wrapBox(fontDataArray[fontID].glyphScale, fontDataArray[fontID].pixelRange / fontDataArray[fontID].glyphScale, fontDataArray[fontID].miterLimit);
        // Add glyphs to atlas - invokes the underlying atlas generator
        // Adding multiple glyphs at once may improve packing efficiency.
        DynamicAtlas::ChangeFlags change = fontDataArray[fontID].dynamicAtlas.add(&glyph, 1);
        // Add glyph to fontGeometry as well so that we can look it up in the future
        fontDataArray[fontID].fontGeometry.addGlyph(glyph);
        if (change & DynamicAtlas::RESIZED) {
            // Atlas has been enlarged: create new texture
            msdfgen::BitmapConstRef<msdf_atlas::byte, 3>& storageRef = (msdfgen::BitmapConstRef<msdf_atlas::byte, 3>&)fontDataArray[fontID].dynamicAtlas.atlasGenerator().atlasStorage();
            TextureManager::resize(fontDataArray[fontID].msdfTexture, Vec2i(storageRef.width, storageRef.height));
            GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); // disable byte-alignment restriction
            TextureManager::fillTexture2D(fontDataArray[fontID].msdfTexture, storageRef.pixels);
            GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4)); // enable byte-alignment restriction
            // TODO: Update all texts using this font ...
        }
        else {
            Vec2i offset{};
            Vec2i size{};
            glyph.getBoxRect(offset.x, offset.y, size.x, size.y);
            msdfgen::Bitmap<msdf_atlas::byte, 3> subBitmap(size.x, size.y);
            fontDataArray[fontID].dynamicAtlas.atlasGenerator().atlasStorage().get(offset.x, offset.y, subBitmap);
            GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); // disable byte-alignment restriction
            TextureManager::fillTexture2D(fontDataArray[fontID].msdfTexture, subBitmap, offset, size);
            GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4)); // enable byte-alignment restriction
        }
        return true;
    }
    //------------------------------------------------------------------------------------------------------
    Glyph FontManager::getGlyph(Font& font, const Unicode& codepoint)
    {
#ifdef _DEBUG
        if (font.fontID == 0 || font.fontID > maxFonts || !fontDataArray[font.fontID].valid) {
            warningLogger << LOGGER::BEGIN << "tried to get glyph from invalid font" << LOGGER::ENDL;
            return {};
        }
#endif // _DEBUG
        const msdf_atlas::GlyphGeometry* glyph = fontDataArray[font.fontID].fontGeometry.getGlyph(codepoint);
        // Add glyph to atlas if necessary
        if (glyph == nullptr) {
            if (fontDataArray[font.fontID].missingCharacters.find(codepoint) != fontDataArray[font.fontID].missingCharacters.end()) {
                // We tried to load this glyph before and it was not possible
                glyph = fontDataArray[font.fontID].fontGeometry.getGlyph(fontDataArray[font.fontID].missingCharacterReplacement);
            }
            // try to add the new codepoint
            else if (addNewGlyph(codepoint, font.fontID)) {
                // glyph was added successfully, we need to load it again
                glyph = fontDataArray[font.fontID].fontGeometry.getGlyph(codepoint);
            }
            else {
                // glyph could not be loaded
                // Use the missing character replacement symbol instead and print warning
                glyph = fontDataArray[font.fontID].fontGeometry.getGlyph(fontDataArray[font.fontID].missingCharacterReplacement);
                fontDataArray[font.fontID].missingCharacters.insert(codepoint);
                warningLogger << LOGGER::BEGIN << "Could not load codepoint " << codepoint << " from font " << fontDataArray[font.fontID].path << "!" << LOGGER::ENDL;
            }
        }
        if (glyph == nullptr) {
            return Glyph();
        }
        // Now we have loaded our glyph
        msdfgen::BitmapConstRef<msdfgen::byte, 3> bitmap = fontDataArray[font.fontID].dynamicAtlas.atlasGenerator().atlasStorage();
        return Glyph(*glyph, Vec2i(bitmap.width, bitmap.height));
    }
    //------------------------------------------------------------------------------------------------------
    bool FontManager::loadFontDataFromFile(const std::string& path, const FontID& fontID)
    {
        auto& fontData = fontDataArray[fontID];
        std::string fullpath = Engine::getResourcePath();
        fullpath.append(path);
        std::ifstream f(fullpath);
        if (!f) {
            return false;
        }
        // Load metrics & kerning
        fontData.fontGeometry.loadMetrics(fontHandles[fontID], 1.0);
        fontData.fontGeometry.loadKerning(fontHandles[fontID]);
        try {
            // Parse JSON file
            nlohmann::json data = nlohmann::json::parse(f);
            // Resize font atlas
            fontData.dynamicAtlas.atlasGenerator().resize(data["atlas"]["width"], data["atlas"]["height"]);
            // Add glyphs one by one
            for (const auto& it : data["glyphs"]) {
                // Load the glyph
                Unicode codepoint = it["unicode"];
                msdf_atlas::GlyphGeometry glyph;
                if (!glyph.load(fontHandles[fontID], fontDataArray[fontID].fontGeometry.getGeometryScale(), codepoint)) {
                    warningLogger << LOGGER::BEGIN << "Could not load glyph with codepoint " << codepoint << LOGGER::ENDL;
                    return false;
                }
                msdf_atlas::Rectangle rec{};
                if (it.contains("atlasBounds")) {
                    rec.x = it["atlasBounds"]["left"];
                    rec.y = it["atlasBounds"]["bottom"];
                    rec.x = it["atlasBounds"]["right"] - rec.x;
                    rec.y = it["atlasBounds"]["top"] - rec.y;
                    glyph.setBoxRect(rec);
                }
                glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, fontData.maxCornerAngle, 0);
                // Finalize glyph box size based on the parameters
                glyph.wrapBox(fontData.glyphScale, fontData.pixelRange / fontData.glyphScale, fontData.miterLimit);
                // Add glyph to fontGeometry as well so that we can look it up in the future
                fontData.fontGeometry.addGlyph(glyph);
            }
        }
        catch (...) {
            warningLogger << LOGGER::BEGIN << "Error parsing the JSON file at " << path << ". Falling back to default behaviour." << LOGGER::ENDL;
            return false;
        }
        fontData.dynamicAtlas.add(fontData.glyphs.data(), static_cast<unsigned int>(fontData.glyphs.size()), false, false);
        // Load texture
        auto result = Texture::Load2D(path+".png");
        fontData.msdfTexture = result.first;
        return result.second;
    }
    //------------------------------------------------------------------------------------------------------
    bool FontManager::saveFontDataInFile(const FontID& fontID, const std::string& path)
    {
        auto& fontData = fontDataArray[fontID];
        std::string filepath = Engine::getResourcePath();
        filepath = filepath.append(path);
        bool success = msdf_atlas::exportJSON(&fontData.fontGeometry, 1, 1, fontData.pixelRange, fontData.msdfTexture.getSize().x, fontData.msdfTexture.getSize().y, msdf_atlas::ImageType::MSDF, msdf_atlas::YDirection::TOP_DOWN, filepath.c_str(), true);
        if (!success) return false;
        success = fontData.msdfTexture.saveInFile(path+".png", true);
        return success;
    }
    //------------------------------------------------------------------------------------------------------
    void FontManager::initialize(const unsigned int& startingSize)
    {
        // Initializes queue with all possible FontIDs. FontID = 0 is reserved for invalid Fonts.
        for (FontID id = 1; id <= startingSize; ++id)
        {
            availableFontIDs.push(id);
        }
        // FontDataArray[0] stores the default Font, which is just a invalid font
        fontDataArray.resize(static_cast<std::size_t>(startingSize) + 1);
        fontHandles.resize(startingSize + 1, nullptr);
        maxFonts = startingSize;
        // Initialize freetype
        freetypeHandle = msdfgen::initializeFreetype();
        if (!freetypeHandle) {
            errorLogger << LOGGER::BEGIN << "Could'nt initialize freetype" << LOGGER::ENDL;
        }
    }
    //------------------------------------------------------------------------------------------------------
    void FontManager::increaseReferenceCount(const Font& font)
    {
        if (font.fontID == 0)
            return;
#ifdef _DEBUG
        if (font.fontID > maxFonts) {
            warningLogger << LOGGER::BEGIN << "tried to increase reference count of font that does not exist ..." << LOGGER::ENDL;
            return;
        }
        else if (!fontDataArray[font.fontID].valid) {
            warningLogger << LOGGER::BEGIN << "tried to increase reference count of font that was not valid ..." << LOGGER::ENDL;
            return;
        }
#endif // _DEBUG
        fontDataArray[font.fontID].referenceCount++;
    }
    //------------------------------------------------------------------------------------------------------
    void FontManager::decreaseReferenceCount(const Font& font)
    {
        if (font.fontID == 0)
            return;
#ifdef _DEBUG
        if (font.fontID > maxFonts) {
            warningLogger << LOGGER::BEGIN << "tried to decrease reference count of font that does not exist ..." << LOGGER::ENDL;
            return;
        }
        else if (!fontDataArray[font.fontID].valid) {
            warningLogger << LOGGER::BEGIN << "tried to decrease reference count of font that was not valid ..." << LOGGER::ENDL;
            return;
        }
#endif // _DEBUG
        fontDataArray[font.fontID].referenceCount--;
        if (fontDataArray[font.fontID].referenceCount == 0 && !fontDataArray[font.fontID].persistent) {
            deleteFont(font.fontID);
        }
    }
    //------------------------------------------------------------------------------------------------------
    void FontManager::bind(const Font& font)
    {
#ifdef _DEBUG
        if (font.fontID > maxFonts) {
            warningLogger << LOGGER::BEGIN << "tried to bind font that does not exist ..." << LOGGER::ENDL;
            return;
        }
        else if (!fontDataArray[font.fontID].valid) {
            warningLogger << LOGGER::BEGIN << "tried to bind font that was not valid ..." << LOGGER::ENDL;
            return;
        }
#endif // _DEBUG
        fontDataArray[font.fontID].bind();
    }
    //------------------------------------------------------------------------------------------------------
    void FontManager::unbindAll()
    {
        GLCall(glBindTexture(0, 0));
    }
    //------------------------------------------------------------------------------------------------------
    FontManager::FontID FontManager::loadFont(const std::string& path, const std::string& existingFontDataPath)
    {
        // First look if font was already valid
        auto it = stringToFontID.find(path);
        if (it != stringToFontID.end()) {
            return it->second;
        }
        // Load new font!
        FontID fontID = getNewFontID();
        std::string fullPath = Engine::getResourcePath();
        fullPath.append(path);
        if (!loadNewFont(fullPath, fontID)) {
            deleteFont(fontID);
            return 0;
        }
        if (existingFontDataPath != "") {
            if (!loadFontDataFromFile(existingFontDataPath, fontID)) {
                // Reset atlas
                fontDataArray[fontID].glyphs.clear();
                fontDataArray[fontID].fontGeometry = msdf_atlas::FontGeometry(&fontDataArray[fontID].glyphs);
                fontDataArray[fontID].dynamicAtlas = DynamicAtlas();
                // Load default charset
                loadDefaultCharset(fontID);
                if (fontDataArray[fontID].valid) {
                    // Save the font, st. it can be loaded faster next time!
                    saveFontDataInFile(fontID, existingFontDataPath);
                }
            }
        }
        else {
            loadDefaultCharset(fontID);
        }
        if (fontDataArray[fontID].valid) {
            stringToFontID[path] = fontID;
            fontDataArray[fontID].path = path;
            setupMissingCharacterReplacement(fontID);
            return fontID;
        }
        else {
            deleteFont(fontID);
            return 0;
        }
    }
    //------------------------------------------------------------------------------------------------------
    void FontManager::terminate()
    {
        fontDataArray.clear();
        for (unsigned int i = 0; i < fontHandles.size(); ++i) {
            if (fontHandles[i]) {
                msdfgen::destroyFont(fontHandles[i]);
            }
        }
        if (freetypeHandle)
            msdfgen::deinitializeFreetype(freetypeHandle);
    }
    //------------------------------------------------------------------------------------------------------
    const FontManager::FontData& FontManager::getFontData(const Font& font)
    {
        return fontDataArray[font.fontID];
    }
    //------------------------------------------------------------------------------------------------------
    bool FontManager::isValidGlyph(const Font& font, const Unicode& codepoint)
    {
        return fontDataArray[font.fontID].fontGeometry.getGlyph(codepoint) != nullptr;
    }
    //------------------------------------------------------------------------------------------------------
}