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
				if (childCollision == 0) {
					return childID;
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
		return;
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
	GuiElement2::GuiElement2(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode)
		: guiManager(nullptr), guiID(-1), parentID(-1), position(position), size(size),
		resizeMode(resizeMode), minSize{}, maxSize{}, children{}
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
		other.signOff();
		return *this;
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
	void GuiElement2::addChild(GuiElement2& element)
	{
		if (!element.isValid()) 
		{
			warningLogger << LOGGER::BEGIN << "Tried to add invalid guiElement as child!" << LOGGER::ENDL;
			return;
		}
		children.push_back(element.guiID);
	}
	//--------------------------------------------------------------------------------------------------
}