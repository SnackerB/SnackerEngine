#include "Gui/Layouts/VerticalListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	unsigned GuiVerticalListLayout::defaultVerticalBorder = 10;
	Color4f GuiVerticalListLayout::defaultBackgroundColor = Color4f(0.0f, 0.0f);
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout::GuiVerticalListLayout(Color4f backgroundColor)
		: GuiVerticalLayout(), backgroundColor(backgroundColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalListLayout::GuiVerticalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVerticalLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(alignmentVertical, "alignmentVertical", json, data, parameterNames);
		parseJsonOrReadFromData(verticalBorder, "verticalBorder", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundColor, "backgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundShader, "backgroundShader", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::setVerticalBorder(unsigned verticalBorder)
	{
		if (verticalBorder != this->verticalBorder) {
			this->verticalBorder = verticalBorder;
			registerEnforceLayoutDown();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::computeModelMatrix()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-getHeight()), 0.0f),
			Vec3f(static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::drawBackground(const Vec2i& worldPosition)
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
	void GuiVerticalListLayout::draw(const Vec2i& worldPosition)
	{
		drawBackground(worldPosition);
		pushClippingBox(worldPosition);
		GuiElement::draw(worldPosition);
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::onRegister()
	{
		GuiLayout::onRegister();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::onSizeChange()
	{
		GuiLayout::onSizeChange();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalListLayout::enforceLayout()
	{
		GuiVerticalLayout::enforceLayout();
		const auto& childIDs = getChildren();
		std::vector<int> heights{};
		int totalMinHeight = 0;
		int totalPreferredHeight = 0;
		for (const auto& childID : childIDs) {
			GuiElement* child = getElement(childID);
			if (child) {
				heights.push_back(child->getMinHeight());
				totalMinHeight += child->getMinHeight();
				totalPreferredHeight += child->getPreferredHeight() == -1 ? child->getMinHeight() : child->getPreferredHeight();
			}
			else heights.push_back(0);
		}
		int remainingHeight = getHeight() - static_cast<int>(verticalBorder * (getChildren().size() + 1)) - totalMinHeight;
		if (remainingHeight > 0) {
			// If the total min height is smaller than the available height, we can afford to make
			// elements larger!
			if (totalPreferredHeight - totalMinHeight >= remainingHeight) {
				// If the totalPreferredHeight is larger than the available height, we need to distribute the available height
				// between the elements
				// First, create a vector of indices of children where the height is still undecided, and where there is
				// actually a preferredHeight larger than the minHeight
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < childIDs.size(); ++i) {
					GuiElement* child = getElement(childIDs[i]);
					if (child && child->getPreferredHeight() > child->getMinHeight()) indicesOfChildrenLeftToResize.push_back(i);
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingHeight > 0) {
					problemOccured = false;
					int heightSlice = remainingHeight / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstHeightSlice = heightSlice + remainingHeight % static_cast<int>(indicesOfChildrenLeftToResize.size());
					// Handle first child
					GuiElement* child = getElement(childIDs[indicesOfChildrenLeftToResize[0]]);
					if (child) {
						if (child->getPreferredHeight() < child->getMinHeight() + firstHeightSlice) {
							heights[indicesOfChildrenLeftToResize[0]] = child->getPreferredHeight();
							remainingHeight -= child->getPreferredHeight() - child->getMinHeight();
							indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
							problemOccured = true;
							continue;
						}
						else {
							heights[indicesOfChildrenLeftToResize[0]] = child->getMinHeight() + firstHeightSlice;
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
						if (child->getPreferredHeight() < child->getMinHeight() + heightSlice) {
							heights[*it] = child->getPreferredHeight();
							remainingHeight -= child->getPreferredHeight() - child->getMinHeight();
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							heights[*it] = child->getMinHeight() + heightSlice;
						}
					}
				}
			}
			else {
				// If instead the total preferred height is smaller than the available height, we can set all children to their
				// preferred height! We also make a list of children who still want to have a larger height, i.e. who dont have a
				// preferred height but are still not at their max height, or don't have a max height as well.
				std::vector<unsigned> indicesOfChildrenLeftToResize;
				for (unsigned i = 0; i < childIDs.size(); ++i) {
					GuiElement* child = getElement(childIDs[i]);
					if (child) {
						if (child->getPreferredHeight() != -1) {
							heights[i] = child->getPreferredHeight();
							remainingHeight -= child->getPreferredHeight() - child->getMinHeight();
						}
						else if (child->getMaxHeight() == -1 || child->getMaxHeight() > child->getMinHeight()) indicesOfChildrenLeftToResize.push_back(i);
					}
				}
				bool problemOccured = true;
				while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingHeight > 0) 
				{
					problemOccured = false;
					int heightSlice = remainingHeight / static_cast<int>(indicesOfChildrenLeftToResize.size());
					int firstHeightSlice = heightSlice + remainingHeight % static_cast<int>(indicesOfChildrenLeftToResize.size());
					// Handle first child
					GuiElement* child = getElement(childIDs[indicesOfChildrenLeftToResize[0]]);
					if (child) {
						if (child->getMaxHeight() != -1 && child->getMaxHeight() < child->getMinHeight() + firstHeightSlice) {
							remainingHeight -= child->getMaxHeight() - child->getMinHeight();
							heights[indicesOfChildrenLeftToResize[0]] = child->getMaxHeight();
							indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
							problemOccured = true;
							continue;
						}
						else {
							heights[indicesOfChildrenLeftToResize[0]] = child->getMinHeight() + firstHeightSlice;
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
						if (child->getMaxHeight() != -1 && child->getMaxHeight() < child->getMinHeight() + heightSlice) {
							remainingHeight -= child->getMaxHeight() - child->getMinHeight();
							heights[*it] = child->getMaxHeight();
							indicesOfChildrenLeftToResize.erase(it);
							problemOccured = true;
							break;
						}
						else {
							heights[*it] = child->getMinHeight() + heightSlice;
						}
					}
				}
			}
		}
		// Now we can actually position and resize the elements
		if (alignmentVertical == AlignmentVertical::TOP || alignmentVertical == AlignmentVertical::CENTER) {
			int position = verticalBorder;
			if (alignmentVertical == AlignmentVertical::CENTER) {
				int totalHeight = static_cast<int>(verticalBorder) * static_cast<int>((childIDs.size() - 1));
				for (auto height : heights) totalHeight += height;
				position = (getHeight() - totalHeight) / 2;
			}
			for (unsigned i = 0; i < childIDs.size(); ++i) {
				GuiElement* child = getElement(childIDs[i]);
				if (child) {
					child->setPositionY(position);
					child->setHeight(heights[i]);
				}
				position += heights[i] + verticalBorder;
			}
		}
		else {
			int position = getHeight() - verticalBorder;
			for (int i = static_cast<int>(childIDs.size()) - 1; i >= 0; --i) {
				GuiElement* child = getElement(childIDs[i]);
				position -= heights[i];
				if (child) {
					child->setPositionY(position);
					child->setHeight(heights[i]);
				}
				position -= verticalBorder;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
}