#include "Gui/Layouts/HorizontalListLayout.h"

namespace SnackerEngine
{

	void HorizontalListLayout::removeChild(GuiElement& guiElement)
	{
		GuiLayout::removeChild(guiElement);
	}

	void HorizontalListLayout::enforceLayout()
	{
		const auto& children = getChildren();
		if (children.empty()) return;
		// First, we need to compute the total height of the list and the number of elements
		// whose height is not specified
		std::vector<int> necessarySpaceUntilEnd(children.size());
		// Iterate backwards!
		necessarySpaceUntilEnd.back() = border;
		for (int i = children.size() - 1; i > 0; --i) {
			int childMinWidth = getMinSize(children[i]).x;
			necessarySpaceUntilEnd[i - 1] = necessarySpaceUntilEnd[i] + childMinWidth + border;
		}
		// We need to compute this additional variable if snapHeightToPreferred is set to true!
		int maxSnapHeight = 0;
		if (snapHeightToPreferred) {
			for (const auto& childID : children) {
				int temp = getPreferredSize(childID).y;
				if (temp > maxSnapHeight) maxSnapHeight = temp;
				temp = getMinSize(childID).y;
				if (temp > maxSnapHeight) maxSnapHeight = temp;
			}
			maxSnapHeight;
		}
		else {
			maxSnapHeight = getHeight() - static_cast<int>(2 * border);
		}
		// position the children
		int currentX = border;
		int widthLeft = getSize().x;
		for (unsigned i = 0; i < children.size(); ++i) {
			Vec2i currentPosition = Vec2i(currentX, border);
			const int preferredWidth = getPreferredSize(children[i]).x;
			int currentWidth = 0;
			if (preferredWidth == -1) {
				// Make as large as possible
				if (getMaxSize(children[i]).x == -1) {
					currentWidth = std::max(getMinSize(children[i]).x, widthLeft - necessarySpaceUntilEnd[i] - static_cast<int>(border));
				}
				else {
					currentWidth = std::max(getMinSize(children[i]).x, std::min(getMaxSize(children[i]).x, widthLeft - necessarySpaceUntilEnd[i] - static_cast<int>(border)));
				}
			}
			else {
				// Use preferred width if possible, else make as large as possible
				currentWidth = std::max(getMinSize(children[i]).x, std::min(preferredWidth, widthLeft - necessarySpaceUntilEnd[i] - static_cast<int>(border)));
			}
			// Compute height
			int currentHeight = getPreferredSize(children[i]).y;
			if (currentHeight == -1) {
				if (getMaxSize(children[i]).y == -1) {
					currentHeight = std::max(getMinSize(children[i]).y, maxSnapHeight);
				}
				else {
					currentHeight = std::max(getMinSize(children[i]).y, std::min(getMaxSize(children[i]).y, maxSnapHeight));
				}
			}
			else {
				currentHeight = std::max(getMinSize(children[i]).y, std::min(currentHeight, maxSnapHeight));
			}
			// Set attributes and enforce layouts
			Vec2i currentSize = Vec2i(currentWidth, currentHeight);
			setPositionAndSizeWithoutEnforcingLayouts(children[i], currentPosition, currentSize);
			enforceLayoutOnElement(children[i]);
			// increase currentY
			currentX += currentWidth + border;
			widthLeft -= (currentWidth + border);
		}
		// If necessary, snap height
		if (snapHeightToPreferred) {
			preferredSize = Vec2i(-1, maxSnapHeight + static_cast<int>(2 * border));
		}
	}

	HorizontalListLayout::HorizontalListLayout(const unsigned& border, const bool& snapHeightToPreferred)
		: border(border), snapHeightToPreferred(snapHeightToPreferred) {}

	bool HorizontalListLayout::registerChild(GuiElement& guiElement)
	{
		return GuiLayout::registerChild(guiElement);
	}

	HorizontalListLayout::HorizontalListLayout(const HorizontalListLayout& other) noexcept
		: GuiLayout(other), border(other.border), snapHeightToPreferred(other.snapHeightToPreferred) {}

	HorizontalListLayout& HorizontalListLayout::operator=(const HorizontalListLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		border = other.border;
		snapHeightToPreferred = other.snapHeightToPreferred;
		return *this;
	}

	HorizontalListLayout::HorizontalListLayout(HorizontalListLayout&& other) noexcept
		: GuiLayout(std::move(other)), border(other.border), snapHeightToPreferred(other.snapHeightToPreferred) {}

	HorizontalListLayout& HorizontalListLayout::operator=(HorizontalListLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		border = other.border;
		snapHeightToPreferred = other.snapHeightToPreferred;
		return *this;
	}

	void HorizontalListLayout::setSnapheightToPreferred(const bool& snapHeightToPreferred)
	{
		if (snapHeightToPreferred != this->snapHeightToPreferred) {
			this->snapHeightToPreferred = snapHeightToPreferred;
			enforceLayout();
		}
	}

}