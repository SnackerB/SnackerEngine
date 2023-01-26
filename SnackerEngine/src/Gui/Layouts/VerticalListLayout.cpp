#include "Gui/Layouts/VerticalLayout.h"
#include "VerticalListLayout.h"

namespace SnackerEngine
{

	void VerticalListLayout::removeChild(GuiElement& guiElement)
	{
		GuiLayout::removeChild(guiElement);
	}
	
	void VerticalListLayout::enforceLayout()
	{
		const auto& children = getChildren();
		if (children.empty()) return;
		// First, we need to compute the total height of the list and the number of elements
		// whose height is not specified
		std::vector<int> necessarySpaceUntilEnd(children.size());
		// Iterate backwards!
		necessarySpaceUntilEnd.back() = border;
		for (int i = children.size() - 1; i > 0; --i) {
			int childMinHeight = getMinSize(children[i]).y;
			necessarySpaceUntilEnd[i - 1] = necessarySpaceUntilEnd[i] + childMinHeight + border;
		}
		// Compute the position of the children. Save the largest width for alignment
		int largestWidth = 0;
		int currentY = border;
		int heightLeft = getSize().y;
		std::vector<Vec2i> positions;
		std::vector<Vec2i> sizes;
		for (unsigned i = 0; i < children.size(); ++i) {
			Vec2i currentPosition = Vec2i(border, currentY);
			const int preferredHeight = getPreferredSize(children[i]).y;
			int currentHeight = 0;
			if (preferredHeight == -1) {
				// Make as large as possible
				if (getMaxSize(children[i]).y == -1) {
					currentHeight = std::max(getMinSize(children[i]).y, heightLeft - necessarySpaceUntilEnd[i] - static_cast<int>(border));
				}
				else {
					currentHeight = std::max(getMinSize(children[i]).y, std::min(getMaxSize(children[i]).y, heightLeft - necessarySpaceUntilEnd[i] - static_cast<int>(border)));
				}
			}
			else {
				// Use preferred height if possible, else make as large as possible
				currentHeight = std::max(getMinSize(children[i]).y, std::min(preferredHeight, heightLeft - necessarySpaceUntilEnd[i] - static_cast<int>(border)));
			}
			// Compute width
			int currentWidth = getPreferredSize(children[i]).x;
			if (currentWidth == -1) {
				if (getMaxSize(children[i]).x == -1) {
					currentWidth = std::max(getMinSize(children[i]).x, getWidth() - static_cast<int>(2 * border));
				}
				else {
					currentWidth = std::max(getMinSize(children[i]).x, std::min(getMaxSize(children[i]).x, getWidth() - static_cast<int>(2 * border)));
				}
			}
			else {
				currentWidth = std::max(getMinSize(children[i]).x, std::min(currentWidth, getWidth() - static_cast<int>(2 * border)));
			}
			Vec2i currentSize = Vec2i(currentWidth, currentHeight);
			// save positions and sizes
			positions.push_back(currentPosition);
			sizes.push_back(currentSize);
			if (currentSize.x > largestWidth) largestWidth = currentSize.x;
			// increase currentY
			currentY += currentHeight + border;
			heightLeft -= (currentHeight + border);
		}
		// Set position and size and do final alignment
		int heightOffset = 0;
		switch (alignmentVertical)
		{
		case SnackerEngine::AlignmentVertical::TOP:
			break;
		case SnackerEngine::AlignmentVertical::CENTER:
			heightOffset = (getSize().y - currentY) / 2;
			break;
		case SnackerEngine::AlignmentVertical::BOTTOM:
			heightOffset = getSize().y - currentY;
			break;
		default:
			break;
		}
		switch (alignmentHorizontal)
		{
		case SnackerEngine::AlignmentHorizontal::LEFT:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(border, positions[i].y + heightOffset), sizes[i]);
				enforceLayoutOnElement(children[i]);
			}
			break;
		}
		case SnackerEngine::AlignmentHorizontal::CENTER:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(getWidth() / 2 - sizes[i].x / 2, positions[i].y + heightOffset), sizes[i]);
				enforceLayoutOnElement(children[i]);
			}
			break;
		}
		case SnackerEngine::AlignmentHorizontal::RIGHT:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(getWidth() - sizes[i].x - border, positions[i].y + heightOffset), sizes[i]);
				enforceLayoutOnElement(children[i]);
			}
			break;
		}
		default:
			break;
		}
	}

	VerticalListLayout::VerticalListLayout(const unsigned& border, AlignmentHorizontal alignmentHorizontal, AlignmentVertical alignmentVertical)
		: border(border), alignmentHorizontal(alignmentHorizontal), alignmentVertical(alignmentVertical) {}
	
	VerticalListLayout::VerticalListLayout(const VerticalListLayout& other) noexcept
		: GuiLayout(other), border(other.border), alignmentHorizontal(other.alignmentHorizontal),
		alignmentVertical(other.alignmentVertical) {}
	
	VerticalListLayout& VerticalListLayout::operator=(const VerticalListLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		border = other.border;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		return *this;
	}
	
	VerticalListLayout::VerticalListLayout(VerticalListLayout&& other) noexcept
		: GuiLayout(std::move(other)), border(other.border), 
		alignmentHorizontal(other.alignmentHorizontal), alignmentVertical(other.alignmentVertical) {}

	
	VerticalListLayout& VerticalListLayout::operator=(VerticalListLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		border = other.border;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		return *this;
	}

}