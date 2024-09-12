#include "Gui/Layouts/GridLayout.h"
#include "Core\Log.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	std::pair<Vec2i, Vec2i> GuiGridLayout::computeLargestMinSizeAndPreferredSize() const
	{
		const auto& children = getChildren();
		// Find the largest minWidth and minHeight, and the largest preferredWidth and preferredHeight
		Vec2i largestMinSize(0, 0);
		Vec2i largestPreferredSize(SIZE_HINT_ARBITRARY, SIZE_HINT_ARBITRARY);
		for (auto childID : children) {
			GuiElement* child = getElement(childID);
			if (child) {
				largestMinSize.x = std::max(largestMinSize.x, child->getMinWidth());
				largestMinSize.y = std::max(largestMinSize.y, child->getMinHeight());
				if (largestPreferredSize.x != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					if (child->getPreferredWidth() >= 0) {
						largestPreferredSize.x = std::max(largestPreferredSize.x, child->getPreferredWidth());
					}
					else if (child->getPreferredWidth() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) largestPreferredSize.x = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				}
				if (largestPreferredSize.y != SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
					if (child->getPreferredHeight() >= 0) {
						largestPreferredSize.y = std::max(largestPreferredSize.y, child->getPreferredHeight());
					}
					else if (child->getPreferredHeight() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) largestPreferredSize.y = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
				}
			}
		}
		return std::make_pair(largestMinSize, largestPreferredSize);
	}
	//--------------------------------------------------------------------------------------------------
	int GuiGridLayout::computeTotalBorderX() const
	{
		return 2 * outerBorder + (totalColumns - 1) * border;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiGridLayout::computeTotalBorderY() const
	{
		return 2 * outerBorder + (totalRows - 1) * border;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiGridLayout::computeAnchorPoint(const Vec2i& firstGridCellSize, const Vec2i& gridCellSize, int totalBorderX, int totalBorderY) const
	{
		return computeAnchorPoint(
			firstGridCellSize.x + gridCellSize.x * (totalColumns - 1) + totalBorderX,
			firstGridCellSize.y + gridCellSize.y * (totalRows - 1) + totalBorderY
		);
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiGridLayout::computeAnchorPoint(int totalGridWidth, int totalGridHeight) const
	{
		Vec2i anchorPoint(0, 0);
		switch (alignmentHorizontal)
		{
		case SnackerEngine::AlignmentHorizontal::LEFT:
			anchorPoint.x = 0;
			break;
		case SnackerEngine::AlignmentHorizontal::CENTER:
			anchorPoint.x = (getWidth() - totalGridWidth) / 2;
			break;
		case SnackerEngine::AlignmentHorizontal::RIGHT:
			anchorPoint.x = getWidth() - totalGridWidth;
			break;
		default:
			break;
		}
		switch (alignmentVertical)
		{
		case SnackerEngine::AlignmentVertical::TOP:
			anchorPoint.y = 0;
			break;
		case SnackerEngine::AlignmentVertical::CENTER:
			anchorPoint.y = (getHeight() - totalGridHeight) / 2;
			break;
		case SnackerEngine::AlignmentVertical::BOTTOM:
			anchorPoint.y = getHeight() - totalGridHeight;
			break;
		default:
			break;
		}
		return anchorPoint;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::positionChildren(const Vec2i& anchorPoint, const Vec2i& firstCellSize, const Vec2i& cellSize)
	{
		const auto& children = getChildren();
		for (unsigned i = 0; i < children.size(); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) {
				Vec2i currentCellSize = cellSize;
				Vec2i cellPosition = anchorPoint + Vec2i(outerBorder, outerBorder);
				if (layoutOptions[i].column == 0) currentCellSize.x = firstCellSize.x;
				else cellPosition.x += firstCellSize.x + border + (cellSize.x + border) * (layoutOptions[i].column - 1);
				if (layoutOptions[i].row == 0) currentCellSize.y = firstCellSize.y;
				else cellPosition.y += firstCellSize.y + border + (cellSize.y + border) * (layoutOptions[i].row - 1);
				Vec2i childSize = child->clampToMinMaxSize(cellSize);
				Vec2i childPosition = cellPosition + (cellSize - childSize) / 2;
				child->setPosition(childPosition);
				child->setSize(childSize);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::positionChildren(const Vec2i& anchorPoint, const std::vector<int>& gridCellWidths, const std::vector<int>& gridCellHeights)
	{
		std::vector<int> cellPositionsX;
		cellPositionsX.push_back(anchorPoint.x + outerBorder);
		for (int i = 0; i < static_cast<int>(totalColumns) - 1; ++i) cellPositionsX.push_back(cellPositionsX.back() + gridCellWidths[i] + border);
		std::vector<int> cellPositionsY;
		cellPositionsY.push_back(anchorPoint.y + outerBorder);
		for (int i = 0; i < static_cast<int>(totalRows) - 1; ++i) cellPositionsY.push_back(cellPositionsY.back() + gridCellHeights[i] + border);
		const auto& children = getChildren();
		for (unsigned i = 0; i < children.size(); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) {
				Vec2i cellPosition(cellPositionsX[layoutOptions[i].column], cellPositionsY[layoutOptions[i].row]);
				Vec2i cellSize(gridCellWidths[layoutOptions[i].column], gridCellHeights[layoutOptions[i].row]);
				Vec2i childSize = child->clampToMinMaxSize(cellSize);
				Vec2i childPosition = cellPosition + (cellSize - childSize) / 2;
				child->setPosition(childPosition);
				child->setSize(childSize);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::enforceLayoutSplitCellsEqually()
	{
		// Find the largest minWidth and minHeight, and the largest preferredWidth and preferredHeight
		auto result = computeLargestMinSizeAndPreferredSize();
		Vec2i& largestMinSize = result.first;
		Vec2i& largestPreferredSize = result.second;
		// Compute the total borders in x and y direction
		int totalBorderX = computeTotalBorderX();
		int totalBorderY = computeTotalBorderY();
		// Compute the size of the grid cells
		Vec2i cellSize(
			(getWidth() - totalBorderX) / static_cast<int>(totalColumns),
			(getHeight() - totalBorderY) / static_cast<int>(totalRows)
		);
		Vec2i firstCellSize = cellSize;
		if (largestMinSize.x > cellSize.x) {
			cellSize.x = largestMinSize.x;
			firstCellSize.x = largestMinSize.x;
		}
		else {
			firstCellSize.x += (getWidth() - totalBorderX) % totalColumns;
		}
		if (largestMinSize.y > cellSize.y) {
			cellSize.y = largestMinSize.y;
			firstCellSize.y = largestMinSize.y;
		}
		else {
			firstCellSize.y += (getHeight() - totalBorderY) % totalRows;
		}
		/// Compute the upper left point of the grid
		Vec2i anchorPoint = computeAnchorPoint(firstCellSize, cellSize, totalBorderX, totalBorderY);
		// Position all elements!
		positionChildren(anchorPoint, firstCellSize, cellSize);
		// Change minSize and preferredSize of the layout
		if (getResizeMode() == ResizeMode::RESIZE_RANGE) {
			setMinSize(Vec2i(largestMinSize.x * totalColumns + totalBorderX, largestMinSize.x * totalRows + totalBorderY));
			setPreferredSize(Vec2i(largestPreferredSize.x * totalColumns + totalBorderX, largestPreferredSize.x * totalRows + totalBorderY));
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::enforceLayoutSplitCellsEquallyShrink()
	{
		// Find the largest minWidth and minHeight, and the largest preferredWidth and preferredHeight
		auto result = computeLargestMinSizeAndPreferredSize();
		Vec2i& largestMinSize = result.first;
		Vec2i& largestPreferredSize = result.second;
		// Compute the total borders in x and y direction
		int totalBorderX = computeTotalBorderX();
		int totalBorderY = computeTotalBorderY();
		// Compute the size of the grid cells
		Vec2i cellSize(
			largestPreferredSize.x >= 0 ? largestPreferredSize.x : largestMinSize.x,
			largestPreferredSize.y >= 0 ? largestPreferredSize.y : largestMinSize.y
		);
		Vec2i firstCellSize = cellSize;
		Vec2i cellSizeEvenSplit(
			(getWidth() - totalBorderX) / static_cast<int>(totalColumns),
			(getHeight() - totalBorderY) / static_cast<int>(totalRows)
		);
		if (cellSizeEvenSplit.x < largestPreferredSize.x) {
			if (cellSizeEvenSplit.x <= largestMinSize.x) {
				cellSize.x = largestMinSize.x;
				firstCellSize.x = largestMinSize.x;
			}
			else {
				cellSize.x = cellSizeEvenSplit.x;
				firstCellSize.x = cellSizeEvenSplit.x + (getWidth() - totalBorderX) % totalColumns;
			}
		}
		if (cellSizeEvenSplit.y < largestPreferredSize.y) {
			if (cellSizeEvenSplit.y <= largestMinSize.y) {
				cellSize.y = largestMinSize.y;
				firstCellSize.y = largestMinSize.y;
			}
			else {
				cellSize.y = cellSizeEvenSplit.y;
				firstCellSize.y = cellSizeEvenSplit.y + (getHeight() - totalBorderY) % totalRows;
			}
		}
		/// Compute the upper left point of the grid
		Vec2i anchorPoint = computeAnchorPoint(cellSize, firstCellSize, totalBorderX, totalBorderY);
		// Position all elements!
		positionChildren(anchorPoint, cellSize, firstCellSize);
		// Change minSize and preferredSize of the layout
		if (getResizeMode() == ResizeMode::RESIZE_RANGE) {
			setMinSize(Vec2i(largestMinSize.x * totalColumns + totalBorderX, largestMinSize.x * totalRows + totalBorderY));
			setPreferredSize(Vec2i(largestPreferredSize.x * totalColumns + totalBorderX, largestPreferredSize.x * totalRows + totalBorderY));
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::enforceLayoutAdaptCellsShrink()
	{
		const auto& children = getChildren();
		// Find the largest minWidth and preferredWidth of each column
		// and the largest minHeight and preferredHeight of each row
		std::vector<int> largestMinWidths(totalColumns, 0);
		std::vector<int> largestPreferredWidths(totalColumns, SIZE_HINT_ARBITRARY);
		std::vector<int> largestMinHeights(totalRows, 0);
		std::vector<int> largestPreferredHeights(totalRows, SIZE_HINT_ARBITRARY);
		for (unsigned i = 0; i < children.size(); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) {
				const Vec2i& childMinSize = child->getMinSize();
				if (childMinSize.x > largestMinWidths[layoutOptions[i].column]) largestMinWidths[layoutOptions[i].column] = childMinSize.x;
				if (childMinSize.y > largestMinHeights[layoutOptions[i].row]) largestMinHeights[layoutOptions[i].row] = childMinSize.y;
				const Vec2i& childPreferredSize = child->getPreferredSize();
				if (childPreferredSize.x >= 0 && childPreferredSize.x > largestPreferredWidths[layoutOptions[i].column])
					largestPreferredWidths[layoutOptions[i].column] = childPreferredSize.x;
				if (childPreferredSize.y >= 0 && childPreferredSize.y > largestPreferredHeights[layoutOptions[i].row])
					largestPreferredHeights[layoutOptions[i].row] = childPreferredSize.y;
			} 
		}
		// Compute the total borders in x and y direction
		int totalBorderX = computeTotalBorderX();
		int totalBorderY = computeTotalBorderY();
		// Compute the widths of the grid cells
		std::vector<int> gridCellWidths(totalColumns, 0);
		int totalPreferredWidth = totalBorderX;
		int totalMinWidth = totalBorderX;
		int totalGridWidth = 0;
		for (unsigned i = 0; i < totalColumns; ++i) {
			totalMinWidth += largestMinWidths[i];
			totalPreferredWidth += largestPreferredWidths[i] >= 0 ? largestMinWidths[i] : largestPreferredWidths[i];
		}
		if (totalPreferredWidth <= getWidth()) {
			// Set all widths to preferredWidth!
			for (unsigned i = 0; i < totalColumns; ++i) gridCellWidths[i] = largestPreferredWidths[i] >= 0 ? largestMinWidths[i] : largestPreferredWidths[i];
			totalGridWidth = totalPreferredWidth;
		}
		else if (totalMinWidth >= getWidth()) {
			// Set all widths to minWidth!
			gridCellWidths = largestMinWidths;
			totalGridWidth = totalMinWidth;
		}
		else {
			// Distribute remaining width evenly between columns
			gridCellWidths = distributeSizeBetweenChildren(largestMinWidths, largestPreferredWidths, getWidth() - totalMinWidth);
			totalGridWidth = getWidth();
		}
		// Compute the heights of the grid cells
		std::vector<int> gridCellHeights(totalRows, 0);
		int totalPreferredHeight = totalBorderY;
		int totalMinHeight = totalBorderY;
		int totalGridHeight = 0;
		for (unsigned i = 0; i < totalRows; ++i) {
			totalMinHeight += largestMinHeights[i];
			totalPreferredHeight += largestPreferredHeights[i] >= 0 ? largestMinHeights[i] : largestPreferredHeights[i];
		}
		if (totalPreferredHeight <= getHeight()) {
			// Set all heights to preferredHeight!
			for (unsigned i = 0; i < totalRows; ++i) gridCellHeights[i] = largestPreferredHeights[i] >= 0 ? largestMinHeights[i] : largestPreferredHeights[i];
			totalGridHeight = totalPreferredHeight;
		}
		else if (totalMinHeight >= getHeight()) {
			// Set all heights to minHeight!
			gridCellHeights = largestMinHeights;
			totalGridHeight = totalMinHeight;
		}
		else {
			// Distribute remaining height evenly between rows
			gridCellHeights = distributeSizeBetweenChildren(largestMinHeights, largestPreferredHeights, getHeight() - totalMinHeight);
			totalGridHeight = getHeight();
		}
		/// Compute the upper left point of the grid
		Vec2i anchorPoint = computeAnchorPoint(totalGridWidth, totalGridHeight);
		// Position all elements!
		positionChildren(anchorPoint, gridCellWidths, gridCellHeights);
		// Change minSize and preferredSize of the layout
		if (getResizeMode() == ResizeMode::RESIZE_RANGE) {
			setMinSize(Vec2i(totalMinWidth, totalMinHeight));
			setPreferredSize(Vec2i(totalPreferredWidth, totalPreferredHeight));
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiGridLayout::Mode> tag)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "AlignmentHorizontal has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "SPLIT_CELLS_EQUALLY" ||
			json == "SPLIT_CELLS_EQUALLY_SHRINK" ||
			json == "ADAPT_CELLS_SHRINK") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid GuiGridLayout::Mode." << LOGGER::ENDL;
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiGridLayout::Mode parseJSON(const nlohmann::json& json, JsonTag<GuiGridLayout::Mode> tag)
	{
		if (json == "SPLIT_CELLS_EQUALLY") return GuiGridLayout::Mode::SPLIT_CELLS_EQUALLY;
		if (json == "SPLIT_CELLS_EQUALLY_SHRINK") return GuiGridLayout::Mode::SPLIT_CELLS_EQUALLY_SHRINK;
		if (json == "ADAPT_CELLS_SHRINK") return GuiGridLayout::Mode::ADAPT_CELLS_SHRINK;
		return GuiGridLayout::Mode::SPLIT_CELLS_EQUALLY;
	}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(unsigned totalColumns, unsigned totalRows, Mode mode, unsigned border, unsigned outerBorder)
		: GuiLayout(), totalColumns(totalColumns), totalRows(totalRows), mode(mode), border(border), outerBorder(outerBorder) {}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(totalColumns, "totalColumns", json, data, parameterNames);
		parseJsonOrReadFromData(totalRows, "totalRows", json, data, parameterNames);
		parseJsonOrReadFromData(border, "border", json, data, parameterNames);
		parseJsonOrReadFromData(outerBorder, "outerBorder", json, data, parameterNames);
		parseJsonOrReadFromData(mode, "mode", json, data, parameterNames);
		parseJsonOrReadFromData(alignmentHorizontal, "alignmentHorizontal", json, data, parameterNames);
		parseJsonOrReadFromData(alignmentVertical, "alignmentVertical", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(const GuiGridLayout& other) noexcept
		: GuiLayout(other), totalColumns(other.totalColumns), totalRows(other.totalRows),
		border(other.border), outerBorder(other.outerBorder), mode(other.mode), 
		alignmentHorizontal(other.alignmentHorizontal),
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
		outerBorder = other.outerBorder;
		mode = other.mode;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		layoutOptions.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiGridLayout::GuiGridLayout(GuiGridLayout&& other) noexcept
		: GuiLayout(std::move(other)), totalColumns(other.totalColumns), totalRows(other.totalRows),
		border(other.border), outerBorder(other.outerBorder), mode(other.mode), 
		alignmentHorizontal(other.alignmentHorizontal),
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
		outerBorder = other.outerBorder;
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
	void GuiGridLayout::setTotalColumns(unsigned totalColumns)
	{
		if (this->totalColumns != totalColumns) {
			this->totalColumns = totalColumns;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::setTotalRows(unsigned totalRows)
	{
		if (this->totalRows != totalRows) {
			this->totalRows = totalRows;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::setBorder(unsigned border)
	{
		if (this->border != border) {
			this->border = border;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiGridLayout::setOuterBorder(unsigned outerBorder)
	{
		if (this->outerBorder != outerBorder) {
			this->outerBorder = outerBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiGridLayout::animateTotalColumns(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiGridLayoutTotalColumnsAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiGridLayout*>(element)->setTotalColumns(currentVal); };
		public:
			GuiGridLayoutTotalColumnsAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiGridLayoutTotalColumnsAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiGridLayout::animateTotalRows(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiGridLayoutTotalRowsAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiGridLayout*>(element)->setTotalRows(currentVal); };
		public:
			GuiGridLayoutTotalRowsAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiGridLayoutTotalRowsAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiGridLayout::animateBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiGridLayoutBorderAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiGridLayout*>(element)->setBorder(currentVal); };
		public:
			GuiGridLayoutBorderAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiGridLayoutBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiGridLayout::animateOuterBorder(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiGridLayoutOuterBorderAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiGridLayout*>(element)->setOuterBorder(currentVal); };
		public:
			GuiGridLayoutOuterBorderAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiGridLayoutOuterBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
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
		case SnackerEngine::GuiGridLayout::Mode::SPLIT_CELLS_EQUALLY_SHRINK:
		{
			enforceLayoutSplitCellsEquallyShrink();
			break;
		}
		case SnackerEngine::GuiGridLayout::Mode::ADAPT_CELLS_SHRINK:
		{
			enforceLayoutAdaptCellsShrink();
			break;
		}
		default:
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------
}
