#include "Gui/Layouts/HorizontalListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

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
		// First, we need to compute the total width of the list and the number of elements
		// whose width is not specified
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
		}
		else {
			maxSnapHeight = getHeight() - static_cast<int>(2 * border);
		}
		// Compute the position of the children. Save the largest width for alignment
		int largestHeight = 0;
		int currentX = border;
		int widthLeft = getSize().x;
		std::vector<Vec2i> positions;
		std::vector<Vec2i> sizes;
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
			Vec2i currentSize = Vec2i(currentWidth, currentHeight);
			// save positions and sizes
			positions.push_back(currentPosition);
			sizes.push_back(currentSize);
			if (currentSize.y > largestHeight) largestHeight = currentSize.y;
			// increase currentX
			currentX += currentWidth + border;
			widthLeft -= (currentWidth + border);
		}
		// Set position and size and do final alignment
		int widthOffset = 0;
		switch (alignmentHorizontal)
		{
		case SnackerEngine::AlignmentHorizontal::LEFT:
			break;
		case SnackerEngine::AlignmentHorizontal::CENTER:
			widthOffset = (getSize().x - currentX) / 2;
			break;
		case SnackerEngine::AlignmentHorizontal::RIGHT:
			widthOffset = (getSize().x - currentX);
			break;
		default:
			break;
		}
		switch (alignmentVertical)
		{
		case SnackerEngine::AlignmentVertical::TOP:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(positions[i].x + widthOffset, border), sizes[i]);
				enforceLayoutOnElement(children[i]);
			}
			break;
		}
		case SnackerEngine::AlignmentVertical::CENTER:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(positions[i].x + widthOffset, (getHeight() - sizes[i].y) / 2), sizes[i]);
				enforceLayoutOnElement(children[i]);
			}
			break;
		}
		case SnackerEngine::AlignmentVertical::BOTTOM:
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// Set attributes and enforce layouts
				setPositionAndSizeWithoutEnforcingLayouts(children[i], Vec2i(positions[i].x + widthOffset, getHeight() - sizes[i].y - border), sizes[i]);
				enforceLayoutOnElement(children[i]);
			}
			break;
		}
		default:
			break;
		}
		// If necessary, snap height
		if (snapHeightToPreferred) {
			preferredSize = Vec2i(-1, maxSnapHeight + static_cast<int>(2 * border));
		}
	}

	void HorizontalListLayout::computeModelMatrix()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(
			Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f),
			Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
	}

	void HorizontalListLayout::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		if (backgroundColor.alpha != 0.0f)
		{
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
			backgroundShader.setUniform<Color3f>("u_color", Color3f(backgroundColor.r, backgroundColor.g, backgroundColor.b));
			Renderer::draw(guiManager->getModelSquare());
		}
		pushClippingBox(parentPosition);
		GuiElement::draw(parentPosition);
		popClippingBox();
	}

	void HorizontalListLayout::onSizeChange()
	{
		GuiLayout::onSizeChange();
		computeModelMatrix();
	}

	HorizontalListLayout::HorizontalListLayout(const unsigned& border, const bool& snapHeightToPreferred, AlignmentHorizontal alignmentHorizontal, AlignmentVertical alignmentVertical)
		: border(border), snapHeightToPreferred(snapHeightToPreferred), alignmentHorizontal(alignmentHorizontal),
		alignmentVertical(alignmentVertical), backgroundColor(0.0f, 0.0f),
		modelMatrixBackground{}, backgroundShader("shaders/gui/simpleColor.shader") {}

	HorizontalListLayout::HorizontalListLayout(const HorizontalListLayout& other) noexcept
		: GuiLayout(other), border(other.border), snapHeightToPreferred(other.snapHeightToPreferred),
		alignmentHorizontal(other.alignmentHorizontal), alignmentVertical(other.alignmentVertical),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground),
		backgroundShader(other.backgroundShader) {}

	HorizontalListLayout& HorizontalListLayout::operator=(const HorizontalListLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		border = other.border;
		snapHeightToPreferred = other.snapHeightToPreferred;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = other.backgroundShader;
		return *this;
	}

	HorizontalListLayout::HorizontalListLayout(HorizontalListLayout&& other) noexcept
		: GuiLayout(std::move(other)), border(other.border), snapHeightToPreferred(other.snapHeightToPreferred),
		alignmentHorizontal(other.alignmentHorizontal), alignmentVertical(other.alignmentVertical),
		backgroundColor(other.backgroundColor), modelMatrixBackground(other.modelMatrixBackground),
		backgroundShader(other.backgroundShader) {}

	HorizontalListLayout& HorizontalListLayout::operator=(HorizontalListLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		border = other.border;
		snapHeightToPreferred = other.snapHeightToPreferred;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = other.backgroundShader;
		return *this;
	}

	void HorizontalListLayout::setSnapheightToPreferred(const bool& snapHeightToPreferred)
	{
		if (snapHeightToPreferred != this->snapHeightToPreferred) {
			this->snapHeightToPreferred = snapHeightToPreferred;
			enforceLayout();
		}
	}

	void HorizontalListLayout::setBackgroundColor(const Color3f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}

}