#include "Gui/Text/Glyph.h"

#include <msdf-atlas-gen.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	Glyph::Glyph()
		: advance(0.0), left{}, bottom{}, right{}, top{}, texLeft{}, texBottom{}, texRight{}, texTop{}, isWhitespace(true) {}
	//------------------------------------------------------------------------------------------------------
	Glyph::Glyph(const msdf_atlas::GlyphGeometry& glyphGeometry, const Vec2i& bitmapSize)
		: advance(glyphGeometry.getAdvance()), left{}, bottom{}, right{}, top{}, 
		texLeft{}, texBottom{}, texRight{}, texTop{}, isWhitespace(glyphGeometry.isWhitespace())
	{
		if (!isWhitespace) {
			glyphGeometry.getQuadAtlasBounds(texLeft, texBottom, texRight, texTop);
			glyphGeometry.getQuadPlaneBounds(left, bottom, right, top);
			// Somewhat hacky solution to stop the next glyph from showing at some text sizes.
			// More correct solution would be to introduce a border between the glyphs on the 
			// MSDF texture. TODO: Implement this!
			//double border = (texRight - texLeft) / 20.0f;
			//texLeft += border;
			//texRight -= border;
			//texBottom += border;
			//texTop -= border;
		}
	}
	//------------------------------------------------------------------------------------------------------
}