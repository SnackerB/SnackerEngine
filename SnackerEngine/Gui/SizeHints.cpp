#include "Gui/SizeHints.h"

namespace SnackerEngine
{

	GuiSizeHints GuiSizeHints::operator&(const GuiSizeHints& other)
	{
		GuiSizeHints result;
		// Compute new minSize
		result.minSize.x = std::max(minSize.x, other.minSize.x);
		result.minSize.y = std::max(minSize.y, other.minSize.y);
		// Compute new maxSize
		if (maxSize.x == -1) result.maxSize.x = other.maxSize.x;
		else if (other.maxSize.x == -1) result.maxSize.x = maxSize.x;
		else result.maxSize.x = std::min(maxSize.y, other.maxSize.y);
		if (maxSize.y == -1) result.maxSize.y = other.maxSize.y;
		else if (other.maxSize.y == -1) result.maxSize.y = maxSize.y;
		else result.maxSize.y = std::min(maxSize.y, other.maxSize.y);
		// Compute new preferredSize
		if (preferredSize.x == -1) {
			if (other.preferredSize.x == -1) result.preferredSize.x = -1;
			else result.preferredSize.x = std::max(result.minSize.x, std::min(result.maxSize.x, other.preferredSize.x));
		}
		else {
			result.preferredSize.x = std::max(result.minSize.x, std::min(result.maxSize.x, preferredSize.x));
		}
		if (preferredSize.y == -1) {
			if (other.preferredSize.y == -1) result.preferredSize.y = -1;
			else result.preferredSize.y = std::max(result.minSize.y, std::min(result.maxSize.y, other.preferredSize.y));
		}
		else {
			result.preferredSize.y = std::max(result.minSize.y, std::min(result.maxSize.y, preferredSize.y));
		}
		return result;
	}

	Vec2i clampSize(const Vec2i& size, const Vec2i& minSize, const Vec2i& maxSize)
	{
		Vec2i result = Vec2i(
			std::max(size.x, minSize.x),
			std::max(size.y, minSize.y));
		if (maxSize.x != -1) result.x = std::min(result.x, maxSize.x);
		if (maxSize.y != -1) result.y = std::min(result.y, maxSize.y);
		return result;
	}

	Vec2i clampSize(const Vec2i& size, const GuiSizeHints& sizeHints)
	{
		return clampSize(size, sizeHints.minSize, sizeHints.maxSize);
	}

	int clampWidthOrHeight(int value, int minValue, int maxValue)
	{
		int result = std::max(value,minValue);
		if (maxValue != -1) return std::min(result, maxValue);
		return result;
	}

}