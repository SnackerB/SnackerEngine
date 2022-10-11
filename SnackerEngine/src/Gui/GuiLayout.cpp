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
		}
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

	void GuiLayout::setSize(const GuiID& guiID, const Vec2f& size)
	{
		guiManager->getElement(guiID).size = size;
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

	GuiLayout::~GuiLayout() noexcept
	{
		// Sign off all children
		if (!guiManager) return;
		for (auto& childID : children) {
			guiManager->signOffWithoutNotifyingParent(guiManager->getElement(childID));
		}
	}

}

