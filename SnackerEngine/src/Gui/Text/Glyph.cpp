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
		}
	}
	//------------------------------------------------------------------------------------------------------
}