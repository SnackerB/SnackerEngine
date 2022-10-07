#include "Gui/GuiElement.h"
#include "Gui/GuiManager.h"
#include "Gui/GuiEventHandles/GuiHandle.h"
#include "Gui/GuiEventHandles/GuiEventHandle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOff()
	{
		guiID = 0;
		guiManager = nullptr;
		parentID = 0;
		childrenIDs.clear();
		layouts.clear();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::drawChildren(const Vec2i& parentPosition)
	{
		guiManager->drawElements(childrenIDs, parentPosition + position);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::removeChild(const GuiElement& guiElement)
	{
		auto result = std::find(childrenIDs.begin(), childrenIDs.end(), guiElement.guiID);
		if (result == childrenIDs.end()) {
			warningLogger << LOGGER::BEGIN << "Tried to remove a child that was not the child of this guiElement" << LOGGER::ENDL;
		}
		else {
			childrenIDs.erase(result);
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiElement::isColliding(const Vec2i& position)
	{
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2f GuiElement::getMouseOffset()
	{
		if (guiManager) return guiManager->getMouseOffset(guiID);
		return Vec2f();
	}
	//--------------------------------------------------------------------------------------------------
	Vec2f GuiElement::getParentMouseOffset()
	{
		if (guiManager) return guiManager->getMouseOffset(parentID);
		return Vec2f();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signUpEvent(const CallbackType& callbackType)
	{
		if (guiManager) guiManager->signUpEvent(*this, callbackType);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOffEvent(const CallbackType& callbackType)
	{
		if (guiManager) guiManager->signOffEvent(*this, callbackType);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signUpHandle(GuiHandle& guiHandle, const GuiHandle::GuiHandleID& handleID)
	{
		guiHandle.registerHandle(handleID, *this);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOffHandle(GuiHandle& guiHandle)
	{
		guiHandle.signOff();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::notifyHandleOnGuiElementMove(GuiHandle& guiHandle)
	{
		guiHandle.onMove(*this);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::activate(GuiEventHandle& guiEventHandle)
	{
		guiEventHandle.activate();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::enforceLayouts()
	{
		for (auto& layout : layouts) {
			layout->enforceLayout();
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const Vec2i& position, const Vec2i& size)
		: guiID(0), guiManager(nullptr), position(position), size(size), preferredSize{}, preferredMinSize{}, preferredMaxSize{}, parentID(0), childrenIDs{}, layouts() {}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const GuiElement& other) noexcept
		: GuiElement(other.position, other.size) {}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(const GuiElement& other) noexcept
	{
		if (guiManager) {
			guiManager->signOff(*this);
		}
		guiID = 0;
		guiManager = nullptr;
		position = other.position;
		size = other.size;
		parentID = 0;
		childrenIDs = {};
		layouts.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(GuiElement&& other) noexcept
		: guiID(other.guiID), guiManager(other.guiManager), position(other.position), size(other.size), 
		preferredSize(other.preferredSize), preferredMinSize(other.preferredMinSize), preferredMaxSize(other.preferredMaxSize),
		parentID(other.parentID), childrenIDs(other.childrenIDs), layouts(std::move(other.layouts))
	{
		other.signOff();
		if (guiManager)
			guiManager->updateMoved(*this);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(GuiElement&& other) noexcept
	{
		if (guiManager) {
			guiManager->signOff(*this);
		}
		guiID = other.guiID;
		guiManager = other.guiManager;
		position = other.position;
		size = other.size;
		parentID = other.parentID;
		childrenIDs = other.childrenIDs;
		layouts = std::move(other.layouts);
		other.signOff();
		if (guiManager)
			guiManager->updateMoved(*this);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::isValid()
	{
		return guiManager != nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPosition(const Vec2i& position)
	{
		this->position = position;
		onPositionChange();
		if (parentID != 0) guiManager->getElement(parentID).enforceLayouts();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSize(const Vec2i& size)
	{
		this->size = size;
		onSizeChange();
		if (parentID != 0) guiManager->getElement(parentID).enforceLayouts();
		else enforceLayouts();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::~GuiElement()
	{
		if (guiManager) {
			guiManager->signOff(*this);
		}
	}
	//--------------------------------------------------------------------------------------------------
}