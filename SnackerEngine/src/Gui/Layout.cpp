#include "Layout.h"
#include "Gui/GuiManager.h"

namespace SnackerEngine
{

	void Layout::setPosition(const GuiID& guiID, const Vec2i& position)
	{
		if (guiManager) {
			guiManager->getElement(guiID).position = position;
		}
	}

	void Layout::setSize(const GuiID& guiID, const Vec2i& size)
	{
		if (guiManager) {
			guiManager->getElement(guiID).size = size;
		}
	}

	const Vec2i& Layout::getPosition(const GuiID& guiID)
	{
		if (guiManager) {
			return guiManager->getElement(guiID).position;
		}
		return Vec2i();
	}

	const Vec2i& Layout::getSize(const GuiID& guiID)
	{
		if (guiManager) {
			return guiManager->getElement(guiID).size;
		}
		return Vec2i();
	}

	const Layout::GuiID& Layout::getParentID(const GuiID& guiID)
	{
		if (guiManager) {
			return guiManager->getElement(guiID).parentID;
		}
		return GuiID();
	}

	void Layout::enforceLayouts(const GuiID& guiID)
	{
		if (guiManager) {
			guiManager->getElement(guiID).enforceLayouts();
		}
	}

	void Layout::notifyPositionChange(const GuiID& guiID)
	{
		if (guiManager) {
			guiManager->getElement(guiID).onPositionChange();
		}
	}

	void Layout::notifySizeChange(const GuiID& guiID)
	{
		if (guiManager) {
			guiManager->getElement(guiID).onSizeChange();
		}
	}

}

