#include "Gui/Layouts/VerticalListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	/*
	GuiVerticalListLayout::GuiVerticalListLayout(Color4f backgroundColor)
		: GuiLayout(), backgroundColor(backgroundColor) {}

	GuiVerticalListLayout::GuiVerticalListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(border, "border", json, data, parameterNames);
		parseJsonOrReadFromData(snapHeight, "snapHeight", json, data, parameterNames);
		parseJsonOrReadFromData(structureMultilines, "structureMultilines", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundColor, "backgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(backgroundShader, "backgroundShader", json, data, parameterNames);
	}

	GuiVerticalListLayout::GuiVerticalListLayout(const GuiVerticalListLayout& other) noexcept
		: GuiLayout(other), border(other.border), snapHeight(other.snapHeight),
		structureMultilines(other.structureMultilines), backgroundColor(other.backgroundColor),
		modelMatrixBackground(), backgroundShader(other.backgroundShader), rows{} {}

	GuiVerticalListLayout& GuiVerticalListLayout::operator=(const GuiVerticalListLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		border = other.border;
		snapHeight = other.snapHeight;
		structureMultilines = other.structureMultilines;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = Mat4f();
		backgroundShader = other.backgroundShader;
		rows.clear();
		return *this;
	}

	GuiVerticalListLayout::GuiVerticalListLayout(GuiVerticalListLayout&& other) noexcept
		: GuiLayout(std::move(other)), border(other.border), snapHeight(other.snapHeight),
		structureMultilines(other.structureMultilines), backgroundColor(other.backgroundColor),
		modelMatrixBackground(other.modelMatrixBackground), backgroundShader(std::move(other.backgroundShader)), 
		rows(std::move(other.rows)) 
	{
		other.rows.clear();
	}

	GuiVerticalListLayout& GuiVerticalListLayout::operator=(GuiVerticalListLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		border = other.border;
		snapHeight = other.snapHeight;
		structureMultilines = other.structureMultilines;
		backgroundColor = other.backgroundColor;
		modelMatrixBackground = other.modelMatrixBackground;
		backgroundShader = std::move(other.backgroundShader);
		rows = std::move(other.rows);
		other.rows.clear();
		return *this;
	}

	bool GuiVerticalListLayout::registerChild(GuiElement& guiElement)
	{
		return registerChild(guiElement, true);
	}

	bool GuiVerticalListLayout::registerChild(GuiElement& guiElement, bool newRow)
	{
		bool result = GuiLayout::registerChild(guiElement);
		if (!result) return false;
		if (newRow || rows.empty()) rows.push_back({});
		rows.back().push_back(guiElement.getGuiID());
	}
	
	bool GuiVerticalListLayout::registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		bool newRow = true;
		parseJsonOrReadFromData(newRow, "newRow", json, data, parameterNames);
		return registerChild(guiElement, newRow);
	}

	void GuiVerticalListLayout::setBorder(unsigned border)
	{
		if (this->border != border) {
			this->border = border;
			registerEnforceLayoutDown();
		}
	}

	void GuiVerticalListLayout::setSnapHeight(bool snapHeight)
	{
		if (this->snapHeight != snapHeight) {
			this->snapHeight = snapHeight;
			registerEnforceLayoutDown();
		}
	}

	void GuiVerticalListLayout::setStructureMultilines(bool structureMultilines)
	{
		if (this->structureMultilines != structureMultilines) {
			this->structureMultilines = structureMultilines;
			registerEnforceLayoutDown();
		}
	}

	GuiSizeHints GuiVerticalListLayout::combineSizeHints(const GuiSizeHints& sizeHints1, const GuiSizeHints& sizeHints2)
	{
		GuiSizeHints result;
		// Compute new minSize
		result.minSize.x = std::max(sizeHints1.minSize.x, sizeHints2.minSize.x);
		result.minSize.y = std::max(sizeHints1.minSize.y, sizeHints2.minSize.y);
		// Compute new maxSize
		if (result.maxSize.x != -1) {
			if (sizeHints1.maxSize.x == -1 || sizeHints2.maxSize.x == -1) result.maxSize.x = -1;
			else result.maxSize.x = std::max(result.maxSize.x, std::max(sizeHints1.maxSize.x, sizeHints2.maxSize.x));
		}
		if (result.maxSize.y != -1) {
			if (sizeHints1.maxSize.y == -1 || sizeHints2.maxSize.y == -1) result.maxSize.y = -1;
			else result.maxSize.y = std::max(result.maxSize.y, std::max(sizeHints1.maxSize.y, sizeHints2.maxSize.y));
		}
		// Compute new preferredSize (largest preferredSize)
		result.preferredSize.x = std::max(result.preferredSize.x, std::max(sizeHints1.preferredSize.x, sizeHints2.preferredSize.x));
		result.preferredSize.y = std::max(result.preferredSize.y, std::max(sizeHints1.preferredSize.y, sizeHints2.preferredSize.y));
		return result;
	}

	void GuiVerticalListLayout::computeModelMatrix()
	{
		modelMatrixBackground = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-getHeight()), 0.0f),
			Vec3f(static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.0f));
	}

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
	
	void GuiVerticalListLayout::draw(const Vec2i& worldPosition)
	{
		drawBackground(worldPosition);
		pushClippingBox(worldPosition);
		GuiElement::draw(worldPosition);
		popClippingBox();
	}

	void GuiVerticalListLayout::onRegister()
	{
		GuiLayout::onRegister();
		computeModelMatrix();
	}
	
	void GuiVerticalListLayout::onSizeChange()
	{
		GuiLayout::onSizeChange();
		computeModelMatrix();
	}
	
	std::optional<unsigned> GuiVerticalListLayout::removeChild(GuiID guiElement)
	{
		std::optional<unsigned> index = GuiLayout::removeChild(guiElement);
		for (unsigned rowIndex = 0; rowIndex < rows.size(); ++rowIndex) {
			for (unsigned int i = 0; i < rows[rowIndex].size(); ++i) {
				if (rows[rowIndex][i] == guiElement) {
					rows[rowIndex].erase(rows[rowIndex].begin() + i);
					if (rows[rowIndex].empty()) {
						rows.erase(rows.begin() + rowIndex);
					}
					return index;
				}
			}
		}
		return index;
	}

	unsigned GuiVerticalListLayout::enforceLayoutStructureMultilines()
	{
		// First we need to gather data about the best structure of each row
		// map mapping from number of elements in a row to vector of updated sizeHints
		struct rowInfo {
			std::vector<GuiSizeHints> updateSizeHints;
			std::vector<unsigned> widths;
			unsigned height;
		};
		std::unordered_map<unsigned, rowInfo> rowInfoMap;
		for (const auto& row : rows)
		{
			unsigned numberOfElements = row.size();
			auto it = rowInfoMap.find(numberOfElements);
			if (it != rowInfoMap.end()) {
				for (unsigned int i = 0; i < numberOfElements; ++i) {
					GuiElement* element = getElement(row[i]);
					if (element) it->second.updateSizeHints[i] = combineSizeHints(it->second.updateSizeHints[i], element->getSizeHints());
				}
			}
			else {
				auto it = rowInfoMap.insert({ numberOfElements, {std::vector<GuiSizeHints>(numberOfElements), {}} });
				for (unsigned int i = 0; i < numberOfElements; ++i) {
					GuiElement* element = getElement(row[i]);
					if (element) it.first->second.updateSizeHints[i] = element->getSizeHints();
				}

			}
		}
		// We must now turn this into a strategy on how the space can be
		// distributed on the different rows
		for (auto& it : rowInfoMap) {
			unsigned totalMinWidth = (it.second.updateSizeHints.size() + 1) * border;
			int largestMinHeight = 0;
			int largestPreferredHeight = 0;
			for (const auto& sizeHints : it.second.updateSizeHints) {
				totalMinWidth += sizeHints.minSize.x;
				largestMinHeight = std::max(largestMinHeight, sizeHints.minSize.y);
				largestPreferredHeight = std::max(largestPreferredHeight, sizeHints.preferredSize.y);
			}
			// Compute widths
			if (totalMinWidth > getWidth()) {
				// Every element will be set to its min width!
				for (const auto& sizeHints : it.second.updateSizeHints)
					it.second.widths.push_back(sizeHints.minSize.x);
			}
			else {
				// First, set all elements to there respective minwidths
				int elementsNotYetAtMaxWidth = 0;
				for (const auto& sizeHints : it.second.updateSizeHints) {
					it.second.widths.push_back(sizeHints.minSize.x);
					if (sizeHints.maxSize.x == -1 || sizeHints.maxSize.x > sizeHints.minSize.x) elementsNotYetAtMaxWidth++;
				}
				// Now, distribute the remaining width evenly between all elements
				int remainingWidth = getWidth() - totalMinWidth;
				while (remainingWidth > 0 && elementsNotYetAtMaxWidth > 0) {
					int widthSlice = remainingWidth / elementsNotYetAtMaxWidth;
					int lastWidthSlice = remainingWidth - (widthSlice * (elementsNotYetAtMaxWidth - 1));
					for (unsigned int i = 0; i < it.second.updateSizeHints.size(); ++i) {
						if (it.second.updateSizeHints[i].maxSize.x == -1) {
							int widthIncrement = i == it.second.updateSizeHints.size() - 1 ? lastWidthSlice : widthSlice;
							it.second.widths[i] += widthIncrement;
							remainingWidth -= widthIncrement;
						}
						else if (it.second.updateSizeHints[i].maxSize.x > it.second.widths[i]) {
							int widthIncrement = i == it.second.updateSizeHints.size() - 1 ? lastWidthSlice : widthSlice;
							if (widthIncrement > it.second.updateSizeHints[i].maxSize.x) {
								elementsNotYetAtMaxWidth--;
								widthIncrement = it.second.updateSizeHints[i].maxSize.x - it.second.widths[i];
							}
							it.second.widths[i] += widthIncrement;
							remainingWidth -= widthIncrement;
						}
					}
				}
				/// // Go from left to right and set every element to the maximum
				/// // possible remaining width!
				/// int remainingWidth = getWidth() - totalMinWidth - (it.second.updateSizeHints.size() - 1) * border;
				/// for (const auto& sizeHints : it.second.updateSizeHints) {
				/// 	if (sizeHints.maxSize.x == -1) {
				/// 		// Take all of the remaining width
				/// 		it.second.widths.push_back(sizeHints.minSize.x + remainingWidth);
				/// 		remainingWidth = 0;
				/// 	}
				/// 	else {
				/// 		int extraWidth = std::min(remainingWidth, sizeHints.maxSize.x - sizeHints.minSize.x);
				/// 		it.second.widths.push_back(sizeHints.minSize.x + extraWidth);
				/// 		remainingWidth -= extraWidth;
				/// 	}
				/// }
			}
			// Compute height
			it.second.height = std::max(largestMinHeight, largestPreferredHeight);
		}
		// Now we can actually set the position of the children!
		Vec2i position = Vec2i(border, border);
		for (unsigned rowIndex = 0; rowIndex < rows.size(); ++rowIndex) {
			const auto& row = rows[rowIndex];
			const auto& rowInfo = rowInfoMap.find(row.size());
			position.x = border;
			for (unsigned int i = 0; i < row.size(); ++i) {
				GuiElement* child = getElement(row[i]);
				if (child) {
					const auto& sizeHints = child->getSizeHints();
					Vec2i childSize = clampSize(Vec2i(rowInfo->second.widths[i], rowInfo->second.height), sizeHints);
					Vec2i offset = Vec2i(0, (rowInfo->second.height - childSize.y) / 2);
					setPositionAndSizeOfChild(row[i], position + offset, childSize);
				}
				position.x += rowInfo->second.widths[i] + border;
			}
			position.y += rowInfo->second.height + border;
		}
		return position.y;
	}
	
	void GuiVerticalListLayout::enforceLayout()
	{
		if (structureMultilines) {
			enforceLayoutStructureMultilines();
		}
		else {
			// TODO
		}
	}
	*/

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