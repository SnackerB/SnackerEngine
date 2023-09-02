#include "GuiLayout.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	std::vector<int> GuiLayout::distributeSizeBetweenChildren(const std::vector<int>& minSizes, const std::vector<int>& preferredSizes, int remainingSize)
	{
		std::vector<int> sizes = minSizes;
		std::vector<unsigned> indicesOfChildrenLeftToResize;
		for (unsigned i = 0; i < minSizes.size(); ++i) {
			if (preferredSizes[i] > minSizes[i]) indicesOfChildrenLeftToResize.push_back(i);
		}
		bool problemOccured = true;
		while (problemOccured && !indicesOfChildrenLeftToResize.empty() && remainingSize > 0) {
			problemOccured = false;
			int widthSlice = remainingSize / static_cast<int>(indicesOfChildrenLeftToResize.size());
			int firstWidthSlice = widthSlice + remainingSize % indicesOfChildrenLeftToResize.size();
			// Handle first child
			int childIndex = indicesOfChildrenLeftToResize[0];
			if (preferredSizes[childIndex] != SIZE_HINT_AS_LARGE_AS_POSSIBLE && preferredSizes[childIndex] < minSizes[childIndex] + firstWidthSlice) {
				sizes[childIndex] = preferredSizes[childIndex];
				remainingSize -= preferredSizes[childIndex] - minSizes[childIndex];
				indicesOfChildrenLeftToResize.erase(indicesOfChildrenLeftToResize.begin());
				problemOccured = true;
				continue;
			}
			else {
				sizes[childIndex] = minSizes[childIndex] + firstWidthSlice;
			}
			// Handle remaining childs
			for (auto it = indicesOfChildrenLeftToResize.begin() + 1; it != indicesOfChildrenLeftToResize.end(); ++it) {
				if (preferredSizes[*it] != SIZE_HINT_AS_LARGE_AS_POSSIBLE && preferredSizes[*it] < minSizes[*it] + widthSlice) {
					sizes[*it] = preferredSizes[*it];
					remainingSize -= preferredSizes[*it] - minSizes[*it];
					indicesOfChildrenLeftToResize.erase(it);
					problemOccured = true;
					break;
				}
				else {
					sizes[*it] = minSizes[*it] + widthSlice;
				}
			}
		}
		return sizes;
	}
	//--------------------------------------------------------------------------------------------------
}
