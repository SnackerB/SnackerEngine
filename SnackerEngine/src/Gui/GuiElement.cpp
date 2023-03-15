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
	void GuiElement::signOffWithoutNotifyingParents(const GuiID& guiID)
	{
		if (guiManager) guiManager->signOffWithoutNotifyingParent(guiID);
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
	void GuiElement::removeChild(GuiID guiElement)
	{
		auto result = std::find(children.begin(), children.end(), guiElement);
		if (result != children.end())
		{
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
				setPositionAndSizeOfChild(childID, Vec2i(), size);
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
	int GuiElement::getMinWidth(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).minSize.x;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getMinHeight(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).minSize.y;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getMaxSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).maxSize;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getMaxWidth(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).maxSize.x;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getMaxHeight(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).maxSize.y;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiElement::getPreferredSize(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).preferredSize;
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getPreferredWidth(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).preferredSize.x;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getPreferredHeight(const GuiID& guiID)
	{
		if (guiManager) return guiManager->getElement(guiID).preferredSize.y;
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiElement::isColliding(const GuiID& guiID, const Vec2i& parentPosition)
	{
		if (guiManager) return guiManager->getElement(guiID).isColliding(parentPosition);
		return IsCollidingResult::NOT_COLLIDING;
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
	void GuiElement::setPositionAndSizeOfChild(const GuiID& guiID, const Vec2i& position, const Vec2i& size)
	{
		if (!guiManager) return;
		auto& child = guiManager->getElement(guiID);
		if (child.position != position)
		{
			child.position = position;
			child.onPositionChange();
		}
		if (child.size != size)
		{
			child.size = size;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child.onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionOfChild(const GuiID& guiID, const Vec2i& position)
	{
		if (!guiManager) return;
		auto& child = guiManager->getElement(guiID);
		if (child.position != position)
		{
			child.position = position;
			child.onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionXOfChild(const GuiID& guiID, const int& positionX)
	{
		if (!guiManager) return;
		auto& child = guiManager->getElement(guiID);
		if (child.position.x != positionX)
		{
			child.position.x = positionX;
			child.onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionYOfChild(const GuiID& guiID, const int& positionY)
	{
		if (!guiManager) return;
		auto& child = guiManager->getElement(guiID);
		if (child.position.y != positionY)
		{
			child.position.y = positionY;
			child.onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSizeOfChild(const GuiID& guiID, const Vec2i& position)
	{
		if (!guiManager) return;
		auto& child = guiManager->getElement(guiID);
		if (child.size != size)
		{
			child.size = size;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child.onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setWidthOfChild(const GuiID& guiID, const int& width)
	{
		if (!guiManager) return;
		auto& child = guiManager->getElement(guiID);
		if (child.size.x != size.x)
		{
			child.size.x = width;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child.onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setHeightOfChild(const GuiID& guiID, const int& height)
	{
		if (!guiManager) return;
		auto& child = guiManager->getElement(guiID);
		if (child.size.y != size.y)
		{
			child.size.y = height;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child.onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::drawElement(const GuiID& guiID, const Vec2i& newParentPosition)
	{
		if (guiManager) guiManager->getElement(guiID).draw(newParentPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::registerEnforceLayoutDown()
	{
		if (guiManager) guiManager->registerForEnforcingLayoutsDown(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::animatePosition(const Vec2i& initial, const Vec2i & final, const double& duration, AnimationFuncT animationFunction)
	{
		if (guiManager)
			guiManager->registerAnimation(std::move(PositionAnimatable(*this, initial, final, duration, animationFunction)));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::animatePositionX(const int& initial, const int& final, const double& duration, AnimationFuncT animationFunction)
	{
		if (guiManager)
			guiManager->registerAnimation(std::move(PositionXAnimatable(*this, initial, final, duration, animationFunction)));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::animatePositionY(const int& initial, const int& final, const double& duration, AnimationFuncT animationFunction)
	{
		if (guiManager)
			guiManager->registerAnimation(std::move(PositionYAnimatable(*this, initial, final, duration, animationFunction)));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::animateSize(const Vec2i& initial, const Vec2i & final, const double& duration, AnimationFuncT animationFunction)
	{
		if (guiManager)
			guiManager->registerAnimation(std::move(SizeAnimatable(*this, initial, final, duration, animationFunction)));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::animateWidth(const int& initial, const int& final, const double& duration, AnimationFuncT animationFunction)
	{
		if (guiManager)
			guiManager->registerAnimation(std::move(WidthAnimatable(*this, initial, final, duration, animationFunction)));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::animateHeight(const int& initial, const int& final, const double& duration, AnimationFuncT animationFunction)
	{
		if (guiManager)
			guiManager->registerAnimation(std::move(HeightAnimatable(*this, initial, final, duration, animationFunction)));
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode)
		: guiManager(nullptr), guiID(-1), parentID(-1), depth(0), position(position), size(size),
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
		: guiManager(nullptr), guiID(-1), parentID(-1), depth(0), position(other.position), size(other.size),
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
		depth = 0;
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
		: guiManager(other.guiManager), guiID(other.guiID), parentID(other.parentID), depth(other.depth), position(other.position), size(other.size),
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
		depth = other.depth;
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
		if (guiManager) {
			if (guiManager->registerElementAsChild(*this, guiElement)) {
				children.push_back(guiElement.guiID);
				return true;
			}
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
		if (this->position != position) {
			this->position = position;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
			onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionX(const int& positionX)
	{
		if (this->position.x != positionX) {
			this->position.x = positionX;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
			onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionY(const int& positionY)
	{
		if (this->position.y != positionY) {
			this->position.y = positionY;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
			onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSize(const Vec2i& size)
	{
		if (this->size != size) {
			this->size = size;
			if (guiManager) guiManager->registerForEnforcingLayoutsUpAndDown(guiID);
			onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setWidth(const int& width)
	{
		if (this->size.x != width) {
			this->size.x = width;
			if (guiManager) guiManager->registerForEnforcingLayoutsUpAndDown(guiID);
			onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setHeight(const int& height)
	{
		if (this->size.y != height) {
			this->size.y = height;
			if (guiManager) guiManager->registerForEnforcingLayoutsUpAndDown(guiID);
			onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionAndSize(const Vec2i& position, const Vec2i& size)
	{
		setPosition(position);
		setSize(size);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinSize(const Vec2i& minSize)
	{
		if (this->minSize != minSize) {
			this->minSize = minSize;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinWidth(const int& minWidth)
	{
		if (this->minSize.x != minWidth) {
			this->minSize.x = minWidth;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinHeight(const int& minHeight)
	{
		if (this->minSize.y != minHeight) {
			this->minSize.y = minHeight;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxSize(const Vec2i& maxSize)
	{
		if (this->maxSize != maxSize) {
			this->maxSize = maxSize;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxWidth(const int& maxWidth)
	{
		if (this->maxSize.x != maxWidth) {
			this->maxSize.x = maxWidth;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxHeight(const int& maxHeight)
	{
		if (this->maxSize.y != maxHeight) {
			this->maxSize.y = maxHeight;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPreferredSize(const Vec2i& preferredSize)
	{
		if (this->preferredSize != preferredSize) {
			this->preferredSize = preferredSize;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPreferredWidth(const int& preferredWidth)
	{
		if (this->preferredSize.x != preferredWidth) {
			this->preferredSize.x = preferredWidth;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPreferredHeight(const int& preferredHeight)
	{
		if (this->preferredSize.y != preferredHeight) {
			this->preferredSize.y = preferredHeight;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
}