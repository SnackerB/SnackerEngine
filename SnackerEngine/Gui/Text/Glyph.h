#pragma once

#include "Math/Vec.h"

/// Forward declaration of msdf_atlas::GlyphGeometry
namespace msdf_atlas {
	class GlyphGeometry;
}

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	struct Glyph
	{
		/// Default Constructor (whitespace with 0 advance)
		Glyph();
		/// Constructor using GlyphGeometry
		Glyph(const msdf_atlas::GlyphGeometry& glyphGeometry, const Vec2i& bitmapSize);
		/// Advance from this glyph to the next one
		double advance;
		/// distance of the four edges of the glyph bounding box to the baseline
		double left;
		double bottom;
		double right;
		double top;
		/// Position and size of the glyph texture in the MSDF Texture
		double texLeft;
		double texBottom;
		double texRight;
		double texTop;
		/// If this is true the corresponding glyph is a whitespace and therefore has no geometry
		/// (But it still has an advance!)
		bool isWhitespace;
	};
	//------------------------------------------------------------------------------------------------------
}