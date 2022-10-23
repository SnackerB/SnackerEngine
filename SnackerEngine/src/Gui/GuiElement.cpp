#include "Gui/GuiElement.h"
#include "Gui/GuiManager.h"
#include "Gui/GuiEventHandles/GuiHandle.h"
#include "Gui/GuiEventHandles/GuiEventHandle.h"
#include "Gui/GuiLayout.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiInteractable::signOff()
	{
		guiManager = nullptr;
		parentID = 0;
	}
	//--------------------------------------------------------------------------------------------------
	GuiInteractable::IsCollidingResult GuiInteractable::isColliding(const Vec2i& position)
	{
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiInteractable::signUpEvent(const CallbackType& callbackType)
	{
		if (guiManager) guiManager->signUpEvent(*this, callbackType);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiInteractable::signOffEvent(const CallbackType& callbackType)
	{
		if (guiManager) guiManager->signOffEvent(*this, callbackType);
	}
	//--------------------------------------------------------------------------------------------------
	GuiInteractable::GuiInteractable(const GuiInteractableType& type)
		: guiManager(nullptr), guiID(0), parentID(0), type(type) {}
	//--------------------------------------------------------------------------------------------------
	GuiInteractable::~GuiInteractable() {}
	//--------------------------------------------------------------------------------------------------
	GuiInteractable::GuiInteractable(const GuiInteractable& other) noexcept
		: GuiInteractable(other.type) {}
	//--------------------------------------------------------------------------------------------------
	GuiInteractable& GuiInteractable::operator=(const GuiInteractable& other) noexcept
	{
		signOff();
		guiManager = nullptr;
		guiID = 0;
		parentID = 0;
		type = other.type;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiInteractable::GuiInteractable(GuiInteractable&& other) noexcept
		: guiManager(other.guiManager), guiID(other.guiID), parentID(other.parentID), type(other.type)
	{
		other.signOff();
	}
	//--------------------------------------------------------------------------------------------------
	GuiInteractable& GuiInteractable::operator=(GuiInteractable&& other) noexcept
	{
		signOff();
		guiManager = other.guiManager;
		guiID = other.guiID;
		parentID = other.parentID;
		type = other.type;
		other.signOff();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiInteractable::isValid()
	{
		return guiManager != nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOffElement()
	{
		guiID = 0;
		layouts.clear();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::draw(const Vec2i& parentPosition)
	{
		drawChildren(parentPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::drawChildren(const Vec2i& parentPosition)
	{
		for (auto& layout : layouts) {
			layout->draw(parentPosition + position);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::removeChild(GuiElement& guiElement)
	{
		auto result = layoutIDToIndex.find(guiElement.parentLayoutID);
		if (result != layoutIDToIndex.end()) {
			layouts[result->second]->removeChild(guiElement);
		}
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
	void GuiElement::addChild(const GuiID& childID)
	{
		auto result = layoutIDToIndex.find(1);
		if (result == layoutIDToIndex.end()) {
			warningLogger << LOGGER::BEGIN << "Tried to add guiElement to layout using an invalid layoutReference!" << LOGGER::ENDL;
			return;
		}
		static_cast<GuiLayout*>(layouts[result->second].get())->addChild(childID, GuiLayoutOptions{});
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::enforceLayouts()
	{
		for (auto& layout : layouts) {
			layout->enforceLayout();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::enforceLayout(const GuiID& layoutID)
	{
		auto result = layoutIDToIndex.find(layoutID);
		if (result != layoutIDToIndex.end()) {
			layouts[result->second]->enforceLayout();
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiLayout& GuiElement::getLayout(const GuiID& layoutID)
	{
		auto result = layoutIDToIndex.find(layoutID);
		if (result != layoutIDToIndex.end()) {
			return *layouts[result->second];
		}
		// Return standard layout
		return *layouts[0];
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const Vec2i& position, const Vec2i& size)
		: GuiInteractable(GuiInteractableType::GUI_ELEMENT), parentLayoutID(0), nextLayoutID(1), position(position), size(size), 
		preferredSize{}, preferredMinSize{}, preferredMaxSize{}, layouts(), layoutIDToIndex{} 
	{
		// Standard layout
		registerLayout<GuiLayout>(std::move(GuiLayout()));
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const GuiElement& other) noexcept
		: GuiElement(other.position, other.size) {}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(const GuiElement& other) noexcept
	{
		GuiInteractable::operator=(other);
		parentLayoutID = 0;
		nextLayoutID = 1;
		position = other.position;
		size = other.size;
		preferredSize = other.preferredSize;
		preferredMinSize = other.preferredMinSize;
		preferredMaxSize = other.preferredMaxSize;
		layouts.clear();
		// Standard layout
		registerLayout<GuiLayout>(std::move(GuiLayout()));
		layoutIDToIndex.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(GuiElement&& other) noexcept
		: GuiInteractable(std::move(other)), parentLayoutID(other.parentLayoutID), nextLayoutID(other.nextLayoutID), position(other.position), size(other.size),
		preferredSize(other.preferredSize), preferredMinSize(other.preferredMinSize), preferredMaxSize(other.preferredMaxSize),
		layouts(std::move(other.layouts)), layoutIDToIndex(std::move(other.layoutIDToIndex)) 
	{
		other.signOffElement();
		if (guiManager) guiManager->updateMoved(*this);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(GuiElement&& other) noexcept
	{
		GuiInteractable::operator=(std::move(other));
		signOffElement();
		parentLayoutID = other.parentLayoutID;
		nextLayoutID = other.nextLayoutID;
		position = other.position;
		size = other.size;
		preferredSize = other.preferredSize;
		preferredMinSize = other.preferredMinSize;
		preferredMaxSize = other.preferredMaxSize;
		parentID = other.parentID;
		layouts = std::move(other.layouts);
		layoutIDToIndex = std::move(other.layoutIDToIndex);
		other.signOffElement();
		if (guiManager) guiManager->updateMoved(*this);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPosition(const Vec2i& position)
	{
		this->position = position;
		onPositionChange();
		if (parentID != 0) guiManager->getElement(parentID).enforceLayout(parentLayoutID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSize(const Vec2i& size)
	{
		this->size = size;
		onSizeChange();
		if (parentID != 0) guiManager->getElement(parentID).enforceLayout(parentLayoutID);
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