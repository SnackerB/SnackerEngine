#include "Gui/Layouts/GridLayout.h"
#include "Core/Log.h"

namespace SnackerEngine
{

	void GridLayout::removeChild(GuiElement& guiElement)
	{
		const auto& children = getChildren();
		auto result = std::find(children.begin(), children.end(), guiElement.getGuiID());
		if (result != children.end()) {
 			layoutOptions.erase(layoutOptions.begin() + (result - children.begin()));
			GuiLayout::removeChild(guiElement);
			registerEnforceLayoutDown();
		}
	}

	void GridLayout::enforceLayout()
	{
		if (!getGuiManager()) return;
		switch (mode)
		{
		case SnackerEngine::GridLayout::Mode::SPLIT_CELLS_EQUALLY:
		{
			enforceLayoutSplitCellsEqually();
			break;
		}
		case SnackerEngine::GridLayout::Mode::ADAPT_CELLS_EQUALLY:
		{
			// First we need to find a size range that suits all children
			const Vec2i& mySize = getSize();
			Vec2i minElementSize = Vec2i(0, 0);
			Vec2i maxElementSize = Vec2i(mySize.x / totalColumns - std::ceil(border * (totalColumns + 1) / totalColumns),
				mySize.y / totalRows - std::ceil(border * (totalRows + 1) / totalRows));
			for (const auto& childID : getChildren()) {
				const Vec2i childMinSize = getMinSize(childID);
				const Vec2i childMaxSize = getMaxSize(childID);
				if (childMinSize.x > minElementSize.x) minElementSize.x = childMinSize.x;
				if (childMinSize.y > minElementSize.y) minElementSize.y = childMinSize.y;
				if (childMaxSize.x >= 0 && childMaxSize.x < maxElementSize.x) maxElementSize.x = childMaxSize.x;
				if (childMaxSize.y >= 0 && childMaxSize.y < maxElementSize.y) maxElementSize.y = childMaxSize.y;
			}
			Vec2i elementSize(-1, -1);
			// Choose best size
			if (maxElementSize.x >= 0 && minElementSize.x > maxElementSize.x) {
				elementSize.x = std::min(maxElementSize.x, static_cast<int>(mySize.x / totalColumns - std::ceil(border * (totalColumns + 1) / totalColumns)));
			}
			if (minElementSize.y >= 0 && minElementSize.y > maxElementSize.y) {
				elementSize.y = std::min(maxElementSize.y, static_cast<int>(mySize.y / totalRows - std::ceil(border * (totalRows + 1) / totalRows)));
			}
			for (const auto& childID : getChildren()) {
				const Vec2i childPreferredSize = getPreferredSize(childID);
				if (elementSize.x == -1 && childPreferredSize.x != -1 && childPreferredSize.x >= minElementSize.x
					&& childPreferredSize.x <= maxElementSize.x) {
					elementSize.x = childPreferredSize.x;
					if (elementSize.y != -1) break;
				}
				if (elementSize.y == -1 && childPreferredSize.y != -1 && childPreferredSize.y >= minElementSize.y
					&& childPreferredSize.y <= maxElementSize.y) {
					elementSize.y = childPreferredSize.y;
					if (elementSize.x != -1) break;
				}
			}
			// If no child had a preferredSize: Make as large as possible!
			if (elementSize.x == -1) elementSize.x = maxElementSize.x;
			if (elementSize.y == -1) elementSize.y = maxElementSize.y;
			enforceLayoutSplitCellsEqually(elementSize);
			break;
		}
		case SnackerEngine::GridLayout::Mode::ADAPT_CELLS_EQUALLY_WEAK:
		{
			// TODO: Implement
			break;
		}
		default:
			break;
		}
	}

	void GridLayout::enforceLayoutSplitCellsEqually(const Vec2i& elementSize)
	{
		// Compute alignment
		const Vec2i totalSize = Vec2i(elementSize.x * totalColumns + border * (totalColumns + 1),
			elementSize.y * totalRows + border * (totalRows + 1));
		Vec2i topLeft = Vec2i(0, 0);
		const Vec2i mySize = getSize();
		switch (alignmentHorizontal)
		{
		case SnackerEngine::GridLayout::AlignmentHorizontal::LEFT: break;
		case SnackerEngine::GridLayout::AlignmentHorizontal::CENTER: topLeft.x = (mySize.x - totalSize.x) / 2; break;
		case SnackerEngine::GridLayout::AlignmentHorizontal::RIGHT: topLeft.x = mySize.x - totalSize.x; break;
		default: break;
		}
		switch (alignmentVertical)
		{
		case SnackerEngine::GridLayout::AlignmentVertical::TOP: break;
		case SnackerEngine::GridLayout::AlignmentVertical::CENTER: topLeft.y = (mySize.y - totalSize.y) / 2; break;
		case SnackerEngine::GridLayout::AlignmentVertical::BOTTOM: topLeft.y = mySize.y - totalSize.y; break;
		default: break;
		}
		// Place child elements
		auto& children = getChildren();
		for (unsigned i = 0; i < children.size(); ++i) {
			// Compute size
			Vec2i currentSize = elementSize;
			// Compute position
			Vec2i currentPosition = topLeft;
			currentPosition += Vec2i((elementSize.x + border) * (layoutOptions[i].column - 1) + border, (elementSize.y + border) * (layoutOptions[i].row - 1) + border);
			// Set attributes and enforce layouts
			setPositionAndSizeOfChild(children[i], currentPosition, currentSize);
		}
	}

	void GridLayout::enforceLayoutSplitCellsEqually()
	{
		unsigned borderPerElementX = border * (totalColumns + 1) / totalColumns;
		unsigned borderPerElementY = border * (totalRows + 1) / totalRows;
		const Vec2i& mySize = getSize();
		Vec2i elementSize = Vec2i(mySize.x / totalColumns - borderPerElementX, mySize.y / totalRows - borderPerElementY);
		Vec2i lastElementSize = elementSize + Vec2i(mySize.x % totalColumns, mySize.y % totalRows);
		lastElementSize.x -= border * (totalColumns + 1) % totalColumns;
		lastElementSize.y -= border * (totalRows + 1) % totalRows;
		const auto& children = getChildren();
		for (unsigned i = 0; i < children.size(); ++i) {
			// Compute size
			Vec2i currentSize = elementSize;
			if (layoutOptions[i].column == totalColumns) currentSize.x = lastElementSize.x;
			if (layoutOptions[i].row == totalRows) currentSize.y = lastElementSize.y;
			// Compute position
			Vec2i currentPosition = Vec2i((elementSize.x + border) * (layoutOptions[i].column - 1) + border, (elementSize.y + border) * (layoutOptions[i].row - 1) + border);
			// Set attributes and enforce layouts
			setPositionAndSizeOfChild(children[i], currentPosition, currentSize);
		}
	}

	GridLayout::GridLayout(const unsigned& totalColumns, const unsigned& totalRows, const Mode& mode, const unsigned& border)
		: totalColumns(totalColumns), totalRows(totalRows), border(border), mode(mode), 
		alignmentHorizontal(AlignmentHorizontal::CENTER),
		alignmentVertical(AlignmentVertical::CENTER), layoutOptions{} {}

	bool GridLayout::registerChild(GuiElement& guiElement, const unsigned& row, const unsigned& column)
	{
		if (GuiLayout::registerChild(guiElement)) {
			layoutOptions.push_back({ row, column });
			return true;
		}
		return false;
	}

	bool GridLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, 0, 0);
	}

	GridLayout::GridLayout(const GridLayout& other) noexcept
		: GuiLayout(other), totalColumns(other.totalColumns), totalRows(other.totalRows), border(other.border), 
		mode(other.mode), alignmentHorizontal(other.alignmentHorizontal), 
		alignmentVertical(other.alignmentVertical), layoutOptions{} {}

	GridLayout& GridLayout::operator=(const GridLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		totalColumns = other.totalColumns;
		totalRows = other.totalRows;
		border = other.border;
		mode = other.mode;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		layoutOptions.clear();
		return *this;
	}

	GridLayout::GridLayout(GridLayout&& other) noexcept
		: GuiLayout(std::move(other)), totalColumns(other.totalColumns), totalRows(other.totalRows), border(other.border),
		mode(other.mode), alignmentHorizontal(other.alignmentHorizontal),
		alignmentVertical(other.alignmentVertical), layoutOptions(other.layoutOptions)
	{
		other.layoutOptions.clear();
	}

	GridLayout& GridLayout::operator=(GridLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		totalColumns = other.totalColumns;
		totalRows = other.totalRows;
		border = other.border;
		mode = other.mode;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		layoutOptions = other.layoutOptions;
		other.layoutOptions.clear();
		return *this;
	}

	const unsigned& GridLayout::getTotalColumns() const
	{
		return totalColumns;
	}

	const unsigned& GridLayout::getTotalRows() const
	{
		return totalRows;
	}

	const unsigned& GridLayout::getBorder() const
	{
		return border;
	}

	const GridLayout::Mode& GridLayout::getMode() const
	{
		return mode;
	}

	const GridLayout::AlignmentHorizontal& GridLayout::getAlignmentHorizontal() const
	{
		return alignmentHorizontal;
	}

	const GridLayout::AlignmentVertical& GridLayout::getAlignmentVertical() const
	{
		return alignmentVertical;
	}

	void GridLayout::setTotalColumns(const unsigned& totalColumns)
	{
		this->totalColumns = totalColumns;
	}

	void GridLayout::setTotalRows(const unsigned& totalRows)
	{
		this->totalRows = totalRows;
	}

	void GridLayout::setBorder(const unsigned& border)
	{
		this->border = border;
	}

	void GridLayout::setMode(const Mode& mode)
	{
		this->mode = mode;
	}

	void GridLayout::setAlignmentHorizontal(const AlignmentHorizontal& alignmentHorizontal)
	{
		this->alignmentVertical = alignmentVertical;
	}

	void GridLayout::setAlignmentVertical(const AlignmentVertical& alignmentVertical)
	{
		this->alignmentHorizontal = alignmentHorizontal;
	}

}