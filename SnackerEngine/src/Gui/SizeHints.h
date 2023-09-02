#pragma once

#include "Math/Vec.h"

namespace SnackerEngine
{

	constexpr int SIZE_HINT_ARBITRARY = -1;;
	constexpr int SIZE_HINT_AS_LARGE_AS_POSSIBLE = -2;

	struct GuiSizeHints
	{
		/// The minimal and maximal size that this GuiElement object can be resized to. Only used
		/// when this elements resizeMode is set to ResizeMode::RESIZE_RANGE
		Vec2i minSize = Vec2i(0, 0);
		/// If a component of maxSize is set to -1 this means that there is no constraint along
		/// this direction and the element can get as large as necessary
		Vec2i maxSize = Vec2i(SIZE_HINT_ARBITRARY, SIZE_HINT_ARBITRARY);
		/// The preferred size of this element. If a component of preferredSize is set to -1
		/// this means that there is no preferred size along this direction!
		Vec2i preferredSize = Vec2i(SIZE_HINT_ARBITRARY, SIZE_HINT_ARBITRARY);
		GuiSizeHints operator&(const GuiSizeHints& other);
	};

	/// Clamps the given size vector to the min and maxe size
	Vec2i clampSize(const Vec2i& size, const Vec2i& minSize, const Vec2i& maxSize);
	Vec2i clampSize(const Vec2i& size, const GuiSizeHints& sizeHints);
	/// Clamps the given width or height value to min and max value
	int clampWidthOrHeight(int value, int minValue, int maxValue);

}