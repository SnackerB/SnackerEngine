#include "GuiLayout.h"
#include "Gui/GuiManager.h"

namespace SnackerEngine
{

	void GuiLayout::handleLayoutOnParentRegister(GuiID parentID)
	{
		if (!guiManager) return;
		for (auto& childID : children) {
			guiManager->getElement(childID).parentID = parentID;
			guiManager->getElement(childID).parentLayoutID = guiID;
			guiManager->handleLayoutsOnGuiElementRegister(guiManager->getElement(childID));
		}
		onRegister();
		enforceLayout();
	}

	std::size_t GuiLayout::removeChild(GuiElement& guiElement)
	{
		if (!guiManager) return 0;
		auto result = std::find(children.begin(), children.end(), guiElement.guiID);
		std::size_t diff  = result - children.begin();
		if (result != children.end()) {
			children.erase(result);
			guiManager->signOffWithoutNotifyingParent(guiElement);
		}
		return diff;
	}

	void GuiLayout::addChild(const GuiID& childID, const GuiLayoutOptions& options)
	{
		children.push_back(childID);
		if (guiManager) {
			guiManager->getElement(childID).parentLayoutID = guiID;
		}
	}

	void GuiLayout::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		// Just draw children
		for (auto& child : children) {
			guiManager->getElement(child).draw(parentPosition);
		}
	}

	GuiLayout::IsCollidingResult GuiLayout::isColliding(const Vec2i& position)
	{
		return IsCollidingResult::WEAK_COLLIDING;
	}

	std::optional<std::pair<GuiLayout::GuiID, GuiLayout::IsCollidingResult>> GuiLayout::getFirstCollidingChild(const Vec2i& position)
	{
		for (auto it = children.rbegin(); it != children.rend(); it++) {
			auto result = guiManager->getGuiInteractable({ *it, 0 }).isColliding(position);
			if (result != GuiElement::IsCollidingResult::NOT_COLLIDING) {
				return { { *it, result } };
			}
		}
		return {};
	}

	GuiLayout::GuiLayout()
		: GuiInteractable(GuiInteractableType::GUI_LAYOUT), children{} {}

	Vec2i GuiLayout::getSize(const GuiID& guiID)
	{
		return guiManager->getElement(guiID).size;
	}

	Vec2i GuiLayout::getPosition(const GuiID& guiID)
	{
		return guiManager->getElement(guiID).position;
	}

	Vec2i GuiLayout::getPreferredSize(const GuiID& guiID)
	{
		return guiManager->getElement(guiID).preferredSize;
	}

	Vec2i GuiLayout::getPreferredMinSize(const GuiID& guiID)
	{
		return guiManager->getElement(guiID).preferredMinSize;
	}

	Vec2i GuiLayout::getPreferredMaxSize(const GuiID& guiID)
	{
		return guiManager->getElement(guiID).preferredMaxSize;
	}

	Vec2f GuiLayout::getMouseOffset(const GuiID& guiID)
	{
		return guiManager->getMouseOffset(guiID);
	}

	void GuiLayout::setSize(const GuiID& guiID, const Vec2f& size)
	{
		guiManager->getElement(guiID).size = size;
	}

	void GuiLayout::setWidth(const GuiID& guiID, const int width)
	{
		guiManager->getElement(guiID).size.x = width;
	}

	void GuiLayout::setHeight(const GuiID& guiID, const int height)
	{
		guiManager->getElement(guiID).size.y = height;
	}

	void GuiLayout::setPosition(const GuiID& guiID, const Vec2f& position)
	{
		guiManager->getElement(guiID).position = position;
	}

	void GuiLayout::setPreferredSize(const GuiID& guiID, const Vec2f& preferredSize)
	{
		guiManager->getElement(guiID).preferredSize = preferredSize;
	}

	void GuiLayout::setPreferredMinSize(const GuiID& guiID, const Vec2f& preferredMinSize)
	{
		guiManager->getElement(guiID).preferredMinSize = preferredMinSize;
	}

	void GuiLayout::setPreferredMaxSize(const GuiID& guiID, const Vec2f& preferredMaxSize)
	{
		guiManager->getElement(guiID).preferredMaxSize = preferredMaxSize;
	}

	void GuiLayout::notifyPositionChange(const GuiID& guiID)
	{
		guiManager->getElement(guiID).onPositionChange();
	}

	void GuiLayout::notifySizeChange(const GuiID& guiID)
	{
		guiManager->getElement(guiID).onSizeChange();
	}

	void GuiLayout::enforceLayouts(const GuiID& guiID)
	{
		guiManager->getElement(guiID).enforceLayouts();
	}

	void GuiLayout::drawElement(const GuiID& guiID, const Vec2i& parentPosition)
	{
		guiManager->getElement(guiID).draw(parentPosition);
	}

	GuiLayout::~GuiLayout() noexcept
	{
		// Sign off all children
		if (!guiManager) return;
		for (auto& childID : children) {
			guiManager->signOffWithoutNotifyingParent(guiManager->getElement(childID));
		}
	}

}

