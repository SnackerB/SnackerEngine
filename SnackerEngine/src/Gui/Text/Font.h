#pragma once

#include "Gui/Text/Glyph.h"

#include <string>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	using Unicode = unsigned int;
	//------------------------------------------------------------------------------------------------------
	class Font
	{
	private:
		using FontID = unsigned int;
		friend class FontManager;
		// FontID fontID;
		Font(const FontID& fontID);
	public:
		FontID fontID; // TODO: Make private again!
		Font();
		/// Load font from a given path relative to the resource directory
		explicit Font(const std::string& path);
		// Copy constructor and assignment operator
		Font& operator=(const Font& other) noexcept;
		explicit Font(const Font& other) noexcept;
		/// Binds this font. Should be called before rendering text
		void bind(const unsigned int& slot = 0) const;
		/// Unbinds all fonts
		static void unBind();
		/// Returns the glyph with the given codepoint
		Glyph getGlyph(const Unicode& codepoint);
		/// returns the vertical distance between two lines
		double getVerticalLineDistance();
		/// Returns the advance distance between two specific glyphs (kerning)
		double getAdvance(const Unicode& first, const Unicode& second);
		/// Returns the pixel range of this font
		double getPixelRange() const;
		/// Returns true if this font is valid and corresponds to a msdf texture on the GPU
		bool isValid() const;
		/// Only used for DEBUGGING. TODO: Remove
		void addNewGlyph(const Unicode& codepoint);
		/// Returns the dimensions of the msdf texture
		Vec2i getMsdfDims() const;
		/// Returns the line spacing (vertical distance between two lines)
		double getLineHeight() const;
		/// Returns the ascender height
		double getAscender() const;
		/// Returns the descender height
		double getDescender() const;
		/// Destructor
		~Font();
	};
	//------------------------------------------------------------------------------------------------------
}