#include "Gui\Layouts\HorizontalListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout::GuiHorizontalListLayout(Color4f backgroundColor)
		: GuiHorizontalLayout(), backgroundColor(backgroundColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiHorizontalListLayout::GuiHorizontalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiHorizontalLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(alignmentHorizontal, "alignmentHorizontal", json, data, parameterNames);
		parseJsonOrReadFromData(horizontalBorder, "horizontalBorder", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundColor, "backgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundShader, "backgroundShader", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::setHorizontalBorder(unsigned horizontalBorder)
	{
		if (horizontalBorder != this->horizontalBorder) {
			this->horizontalBorder = horizontalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::computeModelMatrix()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-getHeight()), 0.0f),
			Vec3f(static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::drawBackground(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (backgroundColor.alpha != 0.0f)
		{
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
			backgroundShader.setUniform<Color4f>("u_color", backgroundColor);
			Renderer::draw(guiManager->getModelSquare());
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::draw(const Vec2i& worldPosition)
	{
		drawBackground(worldPosition);
		pushClippingBox(worldPosition);
		GuiElement::draw(worldPosition);
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::onRegister()
	{
		GuiLayout::onRegister();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::onSizeChange()
	{
		GuiLayout::onSizeChange();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiHorizontalListLayout::enforceLayout()
	{
		GuiHorizontalLayout::enforceLayout();
		const auto& childIDs = getChildren();
		std::vector<int> widths{};
		int totalMinWidth = 0;
		int totalPreferredWidth = 0;
		for (const auto& childID : childIDs) {
			GuiElement* child = getElement(childID);
			if (child) {
				widths.push_back(child->getMinWidth());
				totalMinWidth += child->getMinWidth();
				totalPreferredWidth += child->getPreferredWidth() == -1 ? child->getMinWidth() : child->getPreferredWidth();
			}
			else widths.push_back(0);
		}
		int remainingWidth = getWidth() - static_cast<int>(horizontalBorder * (getChildren().size() + 1)) - totalMinWidth;
		if (remainingWidth > 0) {
			// If the total min Width is smaller than the available Width, we can afford to make
			// elements larger!
			if (totalPreferredWidth - totalMinWidth >= remainingWidth) {
				// If the totalPreferredWidth is larger than the available width, we need to distribute the available width
				// between the elements
				// First, create a vector of indices of children where the width is still undecided, and where there is
				// actually a preferredWidth larger than the minWidth
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < childIDs.size(); ++i) {
					GuiElement* child = getElement(childIDs[i]);
					if (child && child->getPreferredWidth() > child->getMinWidth()) indicesOfChildrenLeftToResize.push_back(i);
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingWidth > 0) {
					problemOccured = false;
					int widthSlice = remainingWidth / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstWidthSlice = widthSlice + remainingWidth % indicesOfChildrenLeftToResize.size();
					// Handle first child
					GuiElement* child = getElement(childIDs[indicesOfChildrenLeftToResize[0]]);
					if (child) {
						if (child->getPreferredWidth() < child->getMinWidth() + firstWidthSlice) {
							widths[indicesOfChildrenLeftToResize[0]] = child->getPreferredWidth();
							remainingWidth -= child->getPreferredWidth() - child->getMinWidth();
							indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
							problemOccured = true;
							continue;
						}
						else {
							widths[indicesOfChildrenLeftToResize[0]] = child->getMinWidth() + firstWidthSlice;
						}
					}
					else {
						indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
						bool problemOccured = true;
						continue;
					}
					// Handle remaining childs
					for (auto it = indicesOfChildrenLeftToResize.begin() + 1; it != indicesOfChildrenLeftToResize.end(); ++it) {
						GuiElement* child = getElement(childIDs[*it]);
						if (child->getPreferredWidth() < child->getMinWidth() + widthSlice) {
							widths[*it] = child->getPreferredWidth();
							remainingWidth -= child->getPreferredWidth() - child->getMinWidth();
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							widths[*it] = child->getMinWidth() + widthSlice;
						}
					}
				}
			}
			else {
				// If instead the total preferred width is smaller than the available width, we can set all children to their
				// preferred width! We also make a list of children who still want to have a larger width, i.e. who dont have a
				// preferred width but are still not at their max width, or don't have a max width as well.
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < childIDs.size(); ++i) {
					GuiElement* child = getElement(childIDs[i]);
					if (child) {
						if (child->getPreferredWidth() != -1) {
							widths[i] = child->getPreferredWidth();
							remainingWidth -= child->getPreferredWidth() - child->getMinWidth();
						}
						else if (child->getMaxWidth() == -1 || child->getMaxWidth() > child->getMinWidth()) indicesOfChildrenLeftToResize.push_back(i);
					}
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingWidth > 0)
				{
					problemOccured = false;
					int widthSlice = remainingWidth / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstWidthSlice = widthSlice + remainingWidth % indicesOfChildrenLeftToResize.size();
					// Handle first child
					GuiElement* child = getElement(childIDs[indicesOfChildrenLeftToResize[0]]);
					if (child) {
						if (child->getMaxWidth() != -1 && child->getMaxWidth() < child->getMinWidth() + firstWidthSlice) {
							remainingWidth -= child->getMaxWidth() - child->getMinWidth();
							widths[indicesOfChildrenLeftToResize[0]] = child->getMaxWidth();
							indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
							problemOccured = true;
							continue;
						}
						else {
							widths[indicesOfChildrenLeftToResize[0]] = child->getMinWidth() + firstWidthSlice;
						}
					}
					else {
						indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
						bool problemOccured = true;
						continue;
					}
					// Handle remaining childs
					for (auto it = indicesOfChildrenLeftToResize.begin() + 1; it != indicesOfChildrenLeftToResize.end(); ++it) {
						GuiElement* child = getElement(childIDs[*it]);
						if (child->getMaxWidth() != -1 && child->getMaxWidth() < child->getMinWidth() + widthSlice) {
							remainingWidth -= child->getMaxWidth() - child->getMinWidth();
							widths[*it] = child->getMaxWidth();
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							widths[*it] = child->getMinWidth() + widthSlice;
						}
					}
				}
			}
		}
		// Now we can actually position and resize the elements
		if (alignmentHorizontal == AlignmentHorizontal::LEFT || alignmentHorizontal == AlignmentHorizontal::CENTER) {
			int position = horizontalBorder;
			if (alignmentHorizontal == AlignmentHorizontal::CENTER) {
				int totalWidth = horizontalBorder * (static_cast<int>(childIDs.size()) - 1);
				for (auto width : widths) totalWidth += width;
				position = (getWidth() - totalWidth) / 2;
			}
			for (unsigned i = 0; i < childIDs.size(); ++i) {
				GuiElement* child = getElement(childIDs[i]);
				if (child) {
					child->setPositionX(position);
					child->setWidth(widths[i]);
				}
				position += widths[i] + horizontalBorder;
			}
		}
		else {
			int position = getWidth() - horizontalBorder;
			for (int i = static_cast<int>(childIDs.size()) - 1; i >= 0; --i) {
				GuiElement* child = getElement(childIDs[i]);
				position -= widths[i];
				if (child) {
					child->setPositionX(position);
					child->setWidth(widths[i]);
				}
				position -= horizontalBorder;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
}