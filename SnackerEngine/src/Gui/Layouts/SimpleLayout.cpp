#include "Gui/Layouts/SimpleLayout.h"

namespace SnackerEngine
{

	void SimpleLayout::enforceLayout()
	{
		const std::vector<GuiID>& children = getChildren();
		if (children.empty()) return;
		GuiID child = children.front();
		const Vec2i& mySize = getSize();
		Vec2i childSize = Vec2i();
		// Compute child width
		childSize.x = getPreferredWidth(child);
		if (childSize.x == -1) {
			childSize.x = getMaxWidth(child);
			if (childSize.x == -1) childSize.x = getMinWidth(child);
		}
		childSize.x = std::max(std::min(mySize.x, childSize.x), getMinWidth(child));
		// Compute child height
		childSize.y = getPreferredHeight(child);
		if (childSize.y == -1) {
			childSize.y = getMaxHeight(child);
			if (childSize.y == -1) childSize.y = getMinHeight(child);
		}
		childSize.y = std::max(std::min(mySize.y, childSize.y), getMinHeight(child));
		// Compute position
		Vec2i childPosition;
		switch (mode)
		{
		case SnackerEngine::SimpleLayout::Mode::CENTER:
			childPosition.x = (mySize.x - childSize.x) / 2;
			childPosition.y = (mySize.y - childSize.y) / 2;
			break;
		case SnackerEngine::SimpleLayout::Mode::LEFT:
			childPosition.x = 0;
			childPosition.y = (mySize.y - childSize.y) / 2;
			break;
		case SnackerEngine::SimpleLayout::Mode::TOP_LEFT:
			childPosition.x = 0;
			childPosition.y = 0;
			break;
		case SnackerEngine::SimpleLayout::Mode::TOP:
			childPosition.x = (mySize.x - childSize.x) / 2;
			childPosition.y = 0;
			break;
		case SnackerEngine::SimpleLayout::Mode::TOP_RIGHT:
			childPosition.x = mySize.x - childSize.x;
			childPosition.y = 0;
			break;
		case SnackerEngine::SimpleLayout::Mode::RIGHT:
			childPosition.x = mySize.x - childSize.x;
			childPosition.y = (mySize.y - childSize.y) / 2;
			break;
		case SnackerEngine::SimpleLayout::Mode::BOTTOM_RIGHT:
			childPosition.x = mySize.x - childSize.x;
			childPosition.y = mySize.y - childSize.y;
			break;
		case SnackerEngine::SimpleLayout::Mode::BOTTOM:
			childPosition.x = (mySize.x - childSize.x) / 2;
			childPosition.y = mySize.y - childSize.y;
			break;
		case SnackerEngine::SimpleLayout::Mode::BOTTOM_LEFT:
			childPosition.x = 0;
			childPosition.y = mySize.y - childSize.y;
			break;
		default:
			break;
		}
		// Enforce Layout
		setPositionAndSizeOfChild(child, childPosition, childSize);
	}
	
	SimpleLayout::SimpleLayout(Mode mode)
		: GuiLayout(), mode(mode) {}

	SimpleLayout::SimpleLayout(const SimpleLayout& other) noexcept
		: GuiLayout(other), mode(other.mode) {}

	SimpleLayout& SimpleLayout::operator=(const SimpleLayout& other) noexcept
	{
		GuiLayout::operator=(other);
		mode = other.mode;
		return *this;
	}

	SimpleLayout::SimpleLayout(SimpleLayout&& other) noexcept
		: GuiLayout(std::move(other)), mode(other.mode) {}

	SimpleLayout& SimpleLayout::operator=(SimpleLayout&& other) noexcept
	{
		GuiLayout::operator=(std::move(other));
		mode = other.mode;
		return *this;
	}

	void SimpleLayout::setMode(Mode mode)
	{
		if (this->mode != mode) {
			this->mode = mode;
			registerEnforceLayoutDown();
		}
	}
}