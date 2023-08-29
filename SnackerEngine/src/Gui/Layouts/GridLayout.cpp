#include "Gui/Layouts/GridLayout.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::enforceLayoutSplitCellsEqually(const Vec2i& elementSize)
	{
		// Compute alignment
		const Vec2i totalSize = Vec2i(elementSize.x * totalColumns + border * (totalColumns + 1),
			elementSize.y * totalRows + border * (totalRows + 1));
		Vec2i topLeft = Vec2i(0, 0);
		const Vec2i mySize = getSize();
		switch (alignmentHorizontal)
		{
		case AlignmentHorizontal::LEFT: break;
		case AlignmentHorizontal::CENTER: topLeft.x = (mySize.x - totalSize.x) / 2; break;
		case AlignmentHorizontal::RIGHT: topLeft.x = mySize.x - totalSize.x; break;
		default: break;
		}
		switch (alignmentVertical)
		{
		case AlignmentVertical::TOP: break;
		case AlignmentVertical::CENTER: topLeft.y = (mySize.y - totalSize.y) / 2; break;
		case AlignmentVertical::BOTTOM: topLeft.y = mySize.y - totalSize.y; break;
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
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::enforceLayoutSplitCellsEqually()
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
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(const unsigned& totalColumns, const unsigned& totalRows, const Mode& mode, const unsigned& border)
		: GuiLayout(), totalColumns(totalColumns), totalRows(totalRows), mode(mode), border(border)
	{
	}
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType<GuiGridLayout::Mode>(const nlohmann::json& json)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "AlignmentHorizontal has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "SPLIT_CELLS_EQUALLY" ||
			json == "ADAPT_CELLS_EQUALLY" ||
			json == "ADAPT_CELLS_EQUALLY_WEAK") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid GuiGridLayout::Mode." << LOGGER::ENDL;
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiGridLayout::Mode parseJSON(const nlohmann::json& json)
	{
		if (json == "SPLIT_CELLS_EQUALLY") return GuiGridLayout::Mode::SPLIT_CELLS_EQUALLY;
		if (json == "ADAPT_CELLS_EQUALLY") return GuiGridLayout::Mode::ADAPT_CELLS_EQUALLY;
		if (json == "ADAPT_CELLS_EQUALLY_WEAK") return GuiGridLayout::Mode::ADAPT_CELLS_EQUALLY_WEAK;
		return GuiGridLayout::Mode::SPLIT_CELLS_EQUALLY;
	}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(totalColumns, "totalColumns", json, data);
		parseJsonOrReadFromData(totalRows, "totalRows", json, data);
		parseJsonOrReadFromData(border, "border", json, data);
		parseJsonOrReadFromData(mode, "mode", json, data);
		parseJsonOrReadFromData(alignmentHorizontal, "alignmentHorizontal", json, data);
		parseJsonOrReadFromData(alignmentVertical, "alignmentVertical", json, data);
	}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(const GuiGridLayout& other) noexcept
		: GuiLayout(other), totalColumns(other.totalColumns), totalRows(other.totalRows),
		border(other.border), mode(other.mode), alignmentHorizontal(other.alignmentHorizontal),
		alignmentVertical(other.alignmentVertical), layoutOptions{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout& GuiGridLayout::operator=(const GuiGridLayout& other) noexcept
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
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(GuiGridLayout&& other) noexcept
		: GuiLayout(std::move(other)), totalColumns(other.totalColumns), totalRows(other.totalRows),
		border(other.border), mode(other.mode), alignmentHorizontal(other.alignmentHorizontal),
		alignmentVertical(other.alignmentVertical), layoutOptions(std::move(other.layoutOptions))
	{
		other.layoutOptions.clear();
	}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout& GuiGridLayout::operator=(GuiGridLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		totalColumns = other.totalColumns;
		totalRows = other.totalRows;
		border = other.border;
		mode = other.mode;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		layoutOptions = std::move(other.layoutOptions);
		other.layoutOptions.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiGridLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, 0, 0);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiGridLayout::registerChild(GuiElement& guiElement, unsigned row, unsigned column)
	{
		if (GuiLayout::registerChild(guiElement)) {
			layoutOptions.push_back({ row, column });
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiGridLayout::registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		unsigned row = 0;
		unsigned column = 0;
		parseJsonOrReadFromData(row, "row", json, data, parameterNames);
		parseJsonOrReadFromData(column, "column", json, data, parameterNames);
		return registerChild(guiElement, row, column);
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<unsigned> GuiGridLayout::removeChild(GuiID guiElement)
	{
		std::optional<unsigned> index = GuiLayout::removeChild(guiElement);
		if (index.has_value()) {
			layoutOptions.erase(layoutOptions.begin() + index.value());
		}
		return index;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::enforceLayout()
	{
		if (!getGuiManager()) return;
		switch (mode)
		{
		case SnackerEngine::GuiGridLayout::Mode::SPLIT_CELLS_EQUALLY:
		{
			enforceLayoutSplitCellsEqually();
			break;
		}
		case SnackerEngine::GuiGridLayout::Mode::ADAPT_CELLS_EQUALLY:
		{
			// First we need to find a size range that suits all children
			const Vec2i& mySize = getSize();
			Vec2i minElementSize = Vec2i(0, 0);
			Vec2i maxElementSize = Vec2i(mySize.x / static_cast<int>(totalColumns) - static_cast<int>(std::ceil(border * (totalColumns + 1) / totalColumns)),
				mySize.y / static_cast<int>(totalRows) - static_cast<int>(std::ceil(border * (totalRows + 1) / totalRows)));
			for (const auto& childID : getChildren()) {
				GuiElement* child = getElement(childID);
				if (!child) continue;
				const Vec2i childMinSize = child->getMinSize();
				const Vec2i childMaxSize = child->getMaxSize();
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
				GuiElement* child = getElement(childID);
				if (!child) continue;
				const Vec2i childPreferredSize = child->getPreferredSize();
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
		case SnackerEngine::GuiGridLayout::Mode::ADAPT_CELLS_EQUALLY_WEAK:
		{
			// TODO: Implement
			break;
		}
		default:
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------
}
