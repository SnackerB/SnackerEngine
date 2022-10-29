#include "Gui/GuiElement2.h"
#include "Gui/GuiManager2.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::signOff()
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
	void GuiElement2::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;
		Vec2i nextPosition = parentPosition + position;
		for (const auto& childID : children) {
			guiManager->getElement(childID).draw(nextPosition);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::removeChild(GuiElement2& guiElement)
	{
		auto result = std::find(children.begin(), children.end(), guiElement.guiID);
		if (result != children.end()) {
			if (guiManager) guiManager->signOffWithoutNotifyingParent(*result);
			children.erase(result);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::enforceLayout()
	{
		if (!guiManager) return;
		// Set size and position of each child that has resizeMode == SAME_AS_PARENT
		for (const auto& childID : children) {
			GuiElement2& childElement = guiManager->getElement(childID);
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
	GuiElement2::IsCollidingResult GuiElement2::isColliding(const Vec2i& position)
	{
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement2::GuiID GuiElement2::getCollidingChild(const Vec2i& position)
	{
		if (!guiManager) return 0;
		for (const auto& childID : children) {
			const auto& result = guiManager->getElement(childID).isColliding(position - this->position);
			switch (result)
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
					return childCollision;
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
		}
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::signUpEvent(const CallbackType& callbackType)
	{
		if (guiManager) guiManager->signUpEvent(*this, callbackType);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::signOffEvent(const CallbackType& callbackType)
	{
		if (guiManager) guiManager->signOffEvent(*this, callbackType);
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2i& GuiElement2::getPosition(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).position;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	const int& GuiElement2::getPositionX(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).position.x;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	const int& GuiElement2::getPositionY(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).position.y;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2i& GuiElement2::getSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).size;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	const int& GuiElement2::getWidth(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).size.x;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	const int& GuiElement2::getHeight(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).size.y;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	const GuiElement2::GuiID& GuiElement2::getParentID(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).parentID;
		return -1;
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2i& GuiElement2::getMouseOffset(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getMouseOffset(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	const GuiElement2::ResizeMode& GuiElement2::getResizeMode(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).resizeMode;
		return ResizeMode::DO_NOT_RESIZE;
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2i& GuiElement2::getMinSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).minSize;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2i& GuiElement2::getMaxSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).maxSize;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPosition(const GuiID& guiID, const Vec2i& position)
	{
		if (guiManager) guiManager->getElement(guiID).setPosition(position);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setSize(const GuiID& guiID, const Vec2i& size)
	{
		if (guiManager) guiManager->getElement(guiID).setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPositionAndSize(const GuiID& guiID, const Vec2i& position, const Vec2i& size)
	{
		if (guiManager) guiManager->getElement(guiID).setPositionAndSize(position, size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPositionWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& position)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position = position;
		guiElement.onPositionChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPositionXWithoutEnforcingLayouts(const GuiID& guiID, const int& x)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position.x = x;
		guiElement.onPositionChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPositionYWithoutEnforcingLayouts(const GuiID& guiID, const int& y)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position.y = y;
		guiElement.onPositionChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setSizeWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& size)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.size = size;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setWidthWithoutEnforcingLayouts(const GuiID& guiID, const int& width)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.size.x = width;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setHeightWithoutEnforcingLayouts(const GuiID& guiID, const int& height)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.size.y = height;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPositionAndSizeWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& position, const Vec2i& size)
	{
		if (!guiManager) return;
		auto& guiElement = guiManager->getElement(guiID);
		guiElement.position = position;
		guiElement.onPositionChange();
		guiElement.size = size;
		guiElement.onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::enforceLayoutOnElement(const GuiID& guiID)
	{
		if (guiManager) guiManager->getElement(guiID).enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::drawElement(const GuiID& guiID, const Vec2i& newParentPosition)
	{
		if (guiManager) guiManager->getElement(guiID).draw(newParentPosition);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement2::registerChildWithoutEnforcingLayouts(GuiElement2& guiElement)
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
	GuiElement2::GuiElement2(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode)
		: guiManager(nullptr), guiID(-1), parentID(-1), position(position), size(size),
		resizeMode(resizeMode), minSize(0, 0), maxSize(-1, -1), children{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement2::~GuiElement2()
	{
		if (guiManager) guiManager->signOff(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement2::GuiElement2(const GuiElement2& other) noexcept
		: guiManager(nullptr), guiID(-1), parentID(-1), position(other.position), size(other.size),
		resizeMode(other.resizeMode), minSize(other.minSize), maxSize(other.maxSize), children{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement2& GuiElement2::operator=(const GuiElement2& other) noexcept
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
		children.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement2::GuiElement2(GuiElement2&& other) noexcept
		: guiManager(other.guiManager), guiID(other.guiID), parentID(other.parentID), position(other.position), size(other.size),
		resizeMode(other.resizeMode), minSize(other.minSize), maxSize(other.maxSize), children(other.children)
	{
		if (guiManager) guiManager->updateMoved(*this);
		other.children.clear();
		other.signOff();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement2& GuiElement2::operator=(GuiElement2&& other) noexcept
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
		children = other.children;
		if (guiManager) guiManager->updateMoved(*this);
		other.children.clear();
		other.signOff();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement2::registerChild(GuiElement2& guiElement)
	{
		if (registerChildWithoutEnforcingLayouts(guiElement)) {
			enforceLayout();
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement2::isValid()
	{
		return guiManager != nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPosition(const Vec2i& position)
	{
		this->position = position;
		onPositionChange();
		if (parentID && guiManager) guiManager->getElement(parentID).enforceLayout();
		else enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setSize(const Vec2i& size)
	{
		this->size = size;
		onSizeChange();
		if (parentID && guiManager) guiManager->getElement(parentID).enforceLayout();
		else enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement2::setPositionAndSize(const Vec2i& position, const Vec2i& size)
	{
		this->position = position;
		onPositionChange();
		this->size = size;
		onSizeChange();
		if (parentID && guiManager) guiManager->getElement(parentID).enforceLayout();
		else enforceLayout();
	}
	//--------------------------------------------------------------------------------------------------
}