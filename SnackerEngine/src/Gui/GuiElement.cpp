#include "Gui/GuiElement.h"
#include "Gui/GuiManager.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOff()
	{
		parentID = -1;
		guiID = -1;
		if (!guiManager) return;
		for (const auto& childID : children) {
			guiManager->signOffWithoutNotifyingParent(childID);
		}
		guiManager = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		Vec2i nextPosition = parentPosition + position;
		for (const auto& childID : children) {
			guiManager->getElement(childID).draw(nextPosition);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::removeChild(GuiElement& guiElement)
	{
		auto result = std::find(children.begin(), children.end(), guiElement.guiID);
		if (result != children.end()) {
			if (guiManager) guiManager->signOffWithoutNotifyingParent(*result);
			children.erase(result);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::enforceLayout()
	{
		if (!guiManager) return;
		// Set size and position of each child that has resizeMode == SAME_AS_PARENT
		for (const auto& childID : children) {
			GuiElement& childElement = guiManager->getElement(childID);
			if (childElement.getResizeMode() == ResizeMode::SAME_AS_PARENT) {
				childElement.position = Vec2i();
				childElement.onPositionChange();
				childElement.size = size;
				childElement.onSizeChange();
				childElement.enforceLayout();
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::registerElementAsChild(GuiElement& guiElement)
	{
		if (guiManager) guiManager->registerElementAsChild(*this, guiElement);
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getChildOffset(const GuiID& childID)
	{
		return getPosition(childID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signUpHandle(GuiHandle& guiHandle, const GuiHandle::GuiHandleID& handleID)
	{
		guiHandle.registerHandle(handleID, *this);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOffHandle(GuiHandle& guiHandle)
	{
		guiHandle.signOff(*this);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::notifyHandleOnGuiElementMove(GuiElement* oldElement, GuiHandle& guiHandle)
	{
		guiHandle.onMove(oldElement, this);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::activate(GuiEventHandle& guiEventHandle)
	{
		guiEventHandle.activate();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiElement::isColliding(const Vec2i& position)
	{
		return IsCollidingResult::COLLIDE_CHILD;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiID GuiElement::getCollidingChild(const IsCollidingResult& collidingResult, const GuiID& childID, const Vec2i& position)
	{
		switch (collidingResult)
		{
		case IsCollidingResult::COLLIDE_CHILD:
		{
			const auto& childCollision = guiManager->getElement(childID).getCollidingChild(position - this->position);
			if (childCollision != 0) {
				return childCollision;
			}
			break;
		}
		case IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT:
		{
			const auto& childCollision = guiManager->getElement(childID).getCollidingChild(position - this->position);
			if (childCollision != 0) {
				return childCollision;
			}
			else {
				return childID;
			}
		}
		case IsCollidingResult::COLLIDE_STRONG:
		{
			return childID;
		}
		case IsCollidingResult::NOT_COLLIDING:
		{
			break;
		}
		default:
			break;
		}
		return -1;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiID GuiElement::getCollidingChild(const Vec2i& position)
	{
		if (!guiManager) return 0;
		for (const auto& childID : children) {
			const auto& result = guiManager->getElement(childID).isColliding(position - this->position);
			const auto& childCollision = getCollidingChild(result, childID, position);
			if (childCollision >= 0) return childCollision;
		}
		return 0;
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
	void GuiElement::pushClippingBox(const Vec4i& clippingBox)
	{
		if (guiManager) guiManager->pushClippingBox(clippingBox);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::pushClippingBox(const Vec2i& parentPosition)
	{
		pushClippingBox(Vec4i(parentPosition.x + position.x, parentPosition.y + position.y, size.x, size.y));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::popClippingBox()
	{
		if (guiManager) guiManager->popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getPosition(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).position;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getPositionX(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).position.x;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getPositionY(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).position.y;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).size;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getWidth(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).size.x;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getHeight(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).size.y;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiID GuiElement::getParentID(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).parentID;
		return -1;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getMouseOffset(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getMouseOffset(guiID);
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::ResizeMode GuiElement::getResizeMode(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).resizeMode;
		return ResizeMode::DO_NOT_RESIZE;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getMinSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).minSize;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getMaxSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).maxSize;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getPreferredSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).preferredSize;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiElement::isColliding(const GuiID& guiID, const Vec2i& parentPosition)
	{
		if (guiManager) return guiManager->getElement(guiID).isColliding(parentPosition);
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPosition(const GuiID& guiID, const Vec2i& position)
	{
		if (guiManager) guiManager->getElement(guiID).setPosition(position);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSize(const GuiID& guiID, const Vec2i& size)
	{
		if (guiManager) guiManager->getElement(guiID).setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionInternal(const Vec2i& position)
	{
		this->position = position;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSizeInternal(const Vec2i& size)
	{
		this->size = size;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionAndSize(const GuiID& guiID, const Vec2i& position, const Vec2i& size)
	{
		if (guiManager) guiManager->getElement(guiID).setPositionAndSize(position, size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& position)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position = position;
		guiElement.onPositionChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionXWithoutEnforcingLayouts(const GuiID& guiID, const int& x)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position.x = x;
		guiElement.onPositionChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionYWithoutEnforcingLayouts(const GuiID& guiID, const int& y)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position.y = y;
		guiElement.onPositionChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSizeWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& size)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.size = size;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setWidthWithoutEnforcingLayouts(const GuiID& guiID, const int& width)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.size.x = width;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setHeightWithoutEnforcingLayouts(const GuiID& guiID, const int& height)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.size.y = height;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionAndSizeWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& position, const Vec2i& size)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position = position;
		guiElement.onPositionChange();
		guiElement.size = size;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::enforceLayoutOnElement(const GuiID& guiID)
	{
		if (guiManager) guiManager->getElement(guiID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::drawElement(const GuiID& guiID, const Vec2i& newParentPosition)
	{
		if (guiManager) guiManager->getElement(guiID).draw(newParentPosition);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::registerChildWithoutEnforcingLayouts(GuiElement& guiElement)
	{
		if (guiManager) {
			if (guiManager->registerElementAsChild(*this, guiElement)) {
				children.push_back(guiElement.guiID);
				return true;
			}
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode)
		: guiManager(nullptr), guiID(-1), parentID(-1), position(position), size(size),
		resizeMode(resizeMode), minSize(0, 0), maxSize(-1, -1), preferredSize(-1, -1), children{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::~GuiElement()
	{
		if (guiManager) guiManager->signOff(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const GuiElement& other) noexcept
		: guiManager(nullptr), guiID(-1), parentID(-1), position(other.position), size(other.size),
		resizeMode(other.resizeMode), minSize(other.minSize), maxSize(other.maxSize), 
		preferredSize(other.preferredSize), children{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(const GuiElement& other) noexcept
	{
		if (guiManager) guiManager->signOff(guiID);
		guiManager = nullptr;
		guiID = -1;
		parentID = -1;
		position = other.position;
		size = other.size;
		resizeMode = other.resizeMode;
		minSize = other.minSize;
		maxSize = other.maxSize;
		preferredSize = other.preferredSize;
		children.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(GuiElement&& other) noexcept
		: guiManager(other.guiManager), guiID(other.guiID), parentID(other.parentID), position(other.position), size(other.size),
		resizeMode(other.resizeMode), minSize(other.minSize), maxSize(other.maxSize), 
		preferredSize(other.preferredSize), children(other.children)
	{
		if (guiManager) guiManager->updateMoved(*this);
		other.children.clear();
		other.signOff();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(GuiElement&& other) noexcept
	{
		if (guiManager) guiManager->signOff(guiID);
		guiManager = other.guiManager;
		guiID = other.guiID;
		parentID = other.parentID;
		position = other.position;
		size = other.size;
		resizeMode = other.resizeMode;
		minSize = other.minSize;
		maxSize = other.maxSize;
		preferredSize = other.preferredSize;
		children = other.children;
		if (guiManager) guiManager->updateMoved(*this);
		other.children.clear();
		other.signOff();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::registerChild(GuiElement& guiElement)
	{
		if (registerChildWithoutEnforcingLayouts(guiElement)) {
			enforceLayout();
			return true;
		}
		return false;
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
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
		else enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSize(const Vec2i& size)
	{
		this->size = size;
		onSizeChange();
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
		else enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionAndSize(const Vec2i& position, const Vec2i& size)
	{
		this->position = position;
		onPositionChange();
		this->size = size;
		onSizeChange();
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
		else enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinSize(const Vec2i& minSize)
	{
		this->minSize = minSize;
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinWidth(const int& minWidth)
	{
		minSize.x = minWidth;
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinHeight(const int& minHeight)
	{
		minSize.y = minHeight;
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxSize(const Vec2i& maxSize)
	{
		this->maxSize = maxSize;
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxWidth(const int& maxWidth)
	{
		maxSize.x = maxWidth;
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxHeight(const int& maxHeight)
	{
		maxSize.y = maxHeight;
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPreferredSize(const Vec2i& preferredSize)
	{
		this->preferredSize = preferredSize;
		if (parentID > 0 && guiManager) guiManager->getElement(parentID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
}