#include "VerticalListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{

	void VerticalListLayout::removeChild(GuiID guiElement)
	{
		GuiLayout::removeChild(guiElement);
		registerEnforceLayoutDown();
	}
	
	void VerticalListLayout::enforceLayout()
	{
		const auto& children = getChildren();
		if (children.empty()) {
			// If necessary, snap width
			if (snapWidthToPreferred) {
				setPreferredWidth(0);
			}
			// If necessary, snap height
			if (snapHeight) {
				setPreferredHeight(0);
			}
			return;
		}
		// First, we need to compute the total height of the list and the number of elements
		// whose height is not specified
		std::vector<int> necessarySpaceUntilEnd(children.size());
		// Iterate backwards!
		necessarySpaceUntilEnd.back() = border;
		for (int i = children.size() - 1; i > 0; --i) {
			int childMinHeight = getMinSize(children[i]).y;
			necessarySpaceUntilEnd[i - 1] = necessarySpaceUntilEnd[i] + childMinHeight + border;
		}
		// We need to compute this additional variable if snapHeightToPreferred is set to true!
		int maxSnapWidth = 0;
		if (snapWidthToPreferred) {
			for (const auto& childID : children) {
				int temp = getPreferredSize(childID).x;
				if (temp > maxSnapWidth) maxSnapWidth = temp;
				temp = getMinSize(childID).x;
				if (temp > maxSnapWidth) maxSnapWidth = temp;
			}
		}
		else {
			maxSnapWidth = getWidth() - static_cast<int>(2 * border);
		}
		int commonWidth = -1;
		if (makeChildrenSameWidth) commonWidth = computeBestCommonWidth();
		double heightScaleFactor = 1.0;
		if (mustFit && necessarySpaceUntilEnd.front() > getHeight())
		{
			heightScaleFactor = static_cast<double>(getHeight()) / static_cast<double>(necessarySpaceUntilEnd.front());
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
			if (makeChildrenSameWidth && commonWidth != -1) {
				currentWidth = commonWidth;
				currentWidth = std::max(getMinSize(children[i]).x, currentWidth);
				if (getMaxSize(children[i]).x != -1)
					currentWidth = std::min(getMaxSize(children[i]).x, currentWidth);
			}
			else {
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
			}
			Vec2i currentSize = Vec2i(currentWidth, currentHeight * heightScaleFactor);
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
			heightOffset = (getSize().y - currentY * heightScaleFactor) / 2;
			break;
		case SnackerEngine::AlignmentVertical::BOTTOM:
			heightOffset = getSize().y - currentY * heightScaleFactor;
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
				setPositionAndSizeOfChild(children[i], Vec2i(border, positions[i].y * heightScaleFactor + heightOffset), sizes[i]);
			}
			break;
		}
		case SnackerEngine::AlignmentHorizontal::CENTER:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeOfChild(children[i], Vec2i(getWidth() / 2 - sizes[i].x / 2, positions[i].y * heightScaleFactor + heightOffset), sizes[i]);
			}
			break;
		}
		case SnackerEngine::AlignmentHorizontal::RIGHT:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeOfChild(children[i], Vec2i(getWidth() - sizes[i].x - border, positions[i].y * heightScaleFactor + heightOffset), sizes[i]);
			}
			break;
		}
		default:
			break;
		}
		// If necessary, snap width
		if (snapWidthToPreferred) {
			setPreferredWidth(maxSnapWidth + static_cast<int>(2 * border));
		}
		// If necessary, snap height
		if (snapHeight) {
			setPreferredHeight(currentY);
		}
	}

	void VerticalListLayout::computeModelMatrix()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-getHeight()), 0.0f),
			Vec3f(static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.0f));
	}

	void VerticalListLayout::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (backgroundColor.alpha != 0.0f)
		{
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
			backgroundShader.setUniform<Color3f>("u_color", Color3f(backgroundColor.r, backgroundColor.g, backgroundColor.b));
			Renderer::draw(guiManager->getModelSquare());
		}
		pushClippingBox(worldPosition);
		GuiElement::draw(worldPosition);
		popClippingBox();
	}

	void VerticalListLayout::onSizeChange()
	{
		GuiLayout::onSizeChange();
		computeModelMatrix();
	}

	int VerticalListLayout::computeBestCommonWidth()
	{
		int minWidth = 0;
		int maxWidth = -1;
		for (const auto& childID : getChildren())
		{
			const int& currentMinWidth = getMinSize(childID).x;
			const int& currentMaxWidth = getMaxSize(childID).x;
			if (minWidth < currentMinWidth) minWidth = currentMinWidth;
			if (maxWidth == -1 || maxWidth > currentMaxWidth) maxWidth = currentMaxWidth;
		}
		// We don't have a good common width, so we just return the minWidth!
		if (maxWidth != -1 && minWidth >= maxWidth) return minWidth;
		// Pick a width that is the preferred width of one of the elements, if possible.
		// Out of these preferred widths pick the largest one!
		int resultWidth = -1;
		for (const auto& childID : getChildren())
		{
			const int& currentPreferredWidth = getPreferredSize(childID).x;
			if (currentPreferredWidth != -1 && minWidth <= currentPreferredWidth && (maxWidth == -1 || currentPreferredWidth <= maxWidth))
			{
				int newWidth = std::max(minWidth, std::min(currentPreferredWidth, getWidth() - static_cast<int>(2 * border)));
				if (newWidth > resultWidth) resultWidth = newWidth;
			}
		}
		if (resultWidth > -1) return resultWidth;
		// else just snap common width to layout width (as large as possible)!
		if (maxWidth == -1) return std::max(minWidth, getWidth() - static_cast<int>(2 * border));
		return std::max(minWidth, std::min(maxWidth, getWidth() - static_cast<int>(2 * border)));
	}

	VerticalListLayout::VerticalListLayout(unsigned border, bool snapWidthToPreferred, bool snapHeight, bool makeChildrenSameWidth, bool mustFit, AlignmentHorizontal alignmentHorizontal, AlignmentVertical alignmentVertical, const Color4f& backgroundColor)
		: border(border), snapWidthToPreferred(snapWidthToPreferred), snapHeight(snapHeight), makeChildrenSameWidth(makeChildrenSameWidth),
		mustFit(mustFit), alignmentHorizontal(alignmentHorizontal), alignmentVertical(alignmentVertical), backgroundColor(backgroundColor),
		modelMatrixBackground{}, backgroundShader("shaders/gui/simpleColor.shader") {}

	bool VerticalListLayout::registerChild(GuiElement& guiElement)
	{
		return GuiLayout::registerChild(guiElement);
	}
	
	VerticalListLayout::VerticalListLayout(const VerticalListLayout& other) noexcept
		: GuiLayout(other), border(other.border), snapWidthToPreferred(other.snapWidthToPreferred),
		snapHeight(other.snapHeight), makeChildrenSameWidth(other.makeChildrenSameWidth), mustFit(other.mustFit),
		alignmentHorizontal(other.alignmentHorizontal), alignmentVertical(other.alignmentVertical),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground),
		backgroundShader(other.backgroundShader) {}
	
	VerticalListLayout& VerticalListLayout::operator=(const VerticalListLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		border = other.border;
		snapWidthToPreferred = other.snapWidthToPreferred;
		snapHeight = other.snapHeight;
		mustFit = other.mustFit;
		makeChildrenSameWidth = other.makeChildrenSameWidth;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = other.backgroundShader;
		return *this;
	}
	
	VerticalListLayout::VerticalListLayout(VerticalListLayout&& other) noexcept
		: GuiLayout(std::move(other)), border(other.border), snapWidthToPreferred(other.snapWidthToPreferred),
		snapHeight(other.snapHeight), makeChildrenSameWidth(other.makeChildrenSameWidth), mustFit(other.mustFit),
		alignmentHorizontal(other.alignmentHorizontal), alignmentVertical(other.alignmentVertical),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground),
		backgroundShader(other.backgroundShader) {}

	VerticalListLayout& VerticalListLayout::operator=(VerticalListLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		border = other.border;
		snapWidthToPreferred = other.snapWidthToPreferred;
		snapHeight = other.snapHeight;
		mustFit = other.mustFit;
		makeChildrenSameWidth = other.makeChildrenSameWidth;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = other.backgroundShader;
		return *this;
	}

	void VerticalListLayout::clear()
	{
		auto& children = getChildren();
		while (!children.empty()) {
			removeChild(children.back());
		}
	}

	void VerticalListLayout::setSnapWidthToPreferred(const bool& snapWidthToPreferred)
	{
		if (snapWidthToPreferred != this->snapWidthToPreferred) {
			this->snapWidthToPreferred = snapWidthToPreferred;
			registerEnforceLayoutDown();
		}
	}

	void VerticalListLayout::setMakeChildrenSameWidth(const bool& makeChildrenSameWidth)
	{
		if (makeChildrenSameWidth != this->makeChildrenSameWidth) {
			this->makeChildrenSameWidth = makeChildrenSameWidth;
			registerEnforceLayoutDown();
		}
	}

	void VerticalListLayout::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}

}