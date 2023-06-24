#include "Gui/GuiElement.h"
#include "Gui/GuiManager.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode)
		: GuiElement(defaultConstructor, position, size, resizeMode)
	{
		initialize();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const nlohmann::json& json, const nlohmann::json* data)
		: GuiElement(defaultConstructor)
	{
		parseFromJSON(json, data);
		initialize();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::~GuiElement()
	{
		if (guiManager) guiManager->signOff(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const GuiElement& other) noexcept
		: GuiElement(defaultConstructor, other)
	{
		initialize();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(const GuiElement& other) noexcept
	{
		copyFromWithoutInitializing(other);
		initialize();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(GuiElement&& other) noexcept
		: GuiElement(defaultConstructor, std::move(other))
	{
		initialize();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(GuiElement&& other) noexcept
	{
		moveFromWithoutInitializing(std::move(other));
		initialize();
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
	void GuiElement::deleteChildren()
	{
		if (!guiManager) return;
		while (!children.empty())
		{
			guiManager->signOff(children.front());
		}
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
	GuiElement::GuiElement(defaultConstructor_t, const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode)
		: guiManager(nullptr), name(""), guiID(-1), parentID(-1), depth(0), position(position), size(size),
		resizeMode(resizeMode), minSize(0, 0), maxSize(-1, -1), preferredSize(-1, -1), children{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(defaultConstructor_t, const GuiElement& other) noexcept
		: guiManager(nullptr), name(""), guiID(-1), parentID(-1), depth(0), position(other.position), size(other.size),
		resizeMode(other.resizeMode), minSize(other.minSize), maxSize(other.maxSize),
		preferredSize(other.preferredSize), children{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(defaultConstructor_t, GuiElement&& other) noexcept
		: guiManager(other.guiManager), name(other.name), guiID(other.guiID), parentID(other.parentID), depth(other.depth), position(other.position), size(other.size),
		resizeMode(other.resizeMode), minSize(other.minSize), maxSize(other.maxSize),
		preferredSize(other.preferredSize), children(other.children)
	{
		if (guiManager) guiManager->updateMoved(*this);
		other.children.clear();
		other.signOff();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::copyFromWithoutInitializing(const GuiElement& other)
	{
		if (guiManager) guiManager->signOff(guiID);
		guiManager = nullptr;
		name = "";
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
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::moveFromWithoutInitializing(GuiElement&& other)
	{
		if (guiManager) guiManager->signOff(guiID);
		name = other.name;
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
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::initialize()
	{
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOffWithoutNotifyingParents(const GuiID& guiID)
	{
		if (guiManager) guiManager->signOffWithoutNotifyingParent(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::draw(const Vec2i& worldPosition)
	{
		if (!guiManager) return;
		for (const auto& childID : children) {
			auto child = getElement(childID);
			if (child) child->draw(worldPosition + child->getPosition());
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
			auto child = getElement(childID);
			if (!child) continue;
			if (child->getResizeMode() == ResizeMode::SAME_AS_PARENT) {
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
	Vec2i GuiElement::getChildOffset(const GuiID& childID) const
	{
		auto child = getElement(childID);
		return child ? child->getPosition() : Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement* GuiElement::getElement(GuiID guiID) const
	{
		if (guiManager) return guiManager->getElement(guiID);
		return nullptr;
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
	void GuiElement::registerEnforceLayoutDown()
	{
		if (guiManager) guiManager->registerForEnforcingLayoutsDown(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::parseFromJSON(const nlohmann::json& json, const nlohmann::json* data)
	{
		if (json.contains("name") && json["name"].is_string()) name = json["name"];
		if (json.contains("position")) parseJsonOrReadFromData(position, json["position"], data);
		if (json.contains("size")) parseJsonOrReadFromData(size, json["size"], data);
		if (json.contains("resizeMode") && json["resizeMode"].is_string()) {
			if (json["resizeMode"] == "DO_NOT_RESIZE") {
				resizeMode = ResizeMode::DO_NOT_RESIZE;
			}
			else if (json["resizeMode"] == "RESIZE_RANGE") {
				resizeMode = ResizeMode::RESIZE_RANGE;
			}
			else if (json["resizeMode"] == "SAME_AS_PARENT") {
				resizeMode = ResizeMode::SAME_AS_PARENT;
			}
		}
		if (json.contains("minSize")) parseJsonOrReadFromData(minSize, json["minSize"], data);
		if (json.contains("maxSize")) parseJsonOrReadFromData(maxSize, json["maxSize"], data);
		if (json.contains("preferredSize")) parseJsonOrReadFromData(preferredSize, json["preferredSize"], data);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionAndSizeOfChild(const GuiID& guiID, const Vec2i& position, const Vec2i& size)
	{
		if (!guiManager) return;
		auto child = guiManager->getElement(guiID);
		if (!child) return;
		if (child->position != position)
		{
			child->position = position;
			child->onPositionChange();
		}
		if (child->size != size)
		{
			child->size = size;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child->onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionOfChild(const GuiID& guiID, const Vec2i& position)
	{
		if (!guiManager) return;
		auto child = guiManager->getElement(guiID);
		if (!child) return;
		if (child->position != position)
		{
			child->position = position;
			child->onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionXOfChild(const GuiID& guiID, const int& positionX)
	{
		if (!guiManager) return;
		auto child = guiManager->getElement(guiID);
		if (!child) return;
		if (child->position.x != positionX)
		{
			child->position.x = positionX;
			child->onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionYOfChild(const GuiID& guiID, const int& positionY)
	{
		if (!guiManager) return;
		auto child = guiManager->getElement(guiID);
		if (!child) return;
		if (child->position.y != positionY)
		{
			child->position.y = positionY;
			child->onPositionChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setSizeOfChild(const GuiID& guiID, const Vec2i& size)
	{
		if (!guiManager) return;
		auto child = guiManager->getElement(guiID);
		if (!child) return;
		if (child->size != size)
		{
			child->size = size;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child->onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setWidthOfChild(const GuiID& guiID, const int& width)
	{
		if (!guiManager) return;
		auto child = guiManager->getElement(guiID);
		if (!child) return;
		if (child->size.x != size.x)
		{
			child->size.x = width;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child->onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setHeightOfChild(const GuiID& guiID, const int& height)
	{
		if (!guiManager) return;
		auto child = guiManager->getElement(guiID);
		if (!child) return;
		if (child->size.y != size.y)
		{
			child->size.y = height;
			guiManager->registerForEnforcingLayoutsDown(guiID);
			child->onSizeChange();
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiElement::isColliding(const Vec2i& offset) const
	{
		return IsCollidingResult::COLLIDE_CHILD;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiID GuiElement::getCollidingChild(const IsCollidingResult& collidingResult, const GuiID& childID, const Vec2i& offset) const
	{
		switch (collidingResult)
		{
		case IsCollidingResult::COLLIDE_CHILD:
		{
			auto child = getElement(childID);
			GuiID childCollision = child ? child->getCollidingChild(offset - child->getPosition()) : -1;
			if (childCollision != 0) {
				return childCollision;
			}
			break;
		}
		case IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT:
		{
			auto child = getElement(childID);
			GuiID childCollision = child ? child->getCollidingChild(offset - child->getPosition()) : -1;
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
	GuiElement::GuiID GuiElement::getCollidingChild(const Vec2i& offset) const
	{
		if (!guiManager) return 0;
		for (const auto& childID : children) {
			auto child = getElement(childID);
			IsCollidingResult result = child ? child->isColliding(offset - child->getPosition()) : IsCollidingResult::NOT_COLLIDING;
			GuiID childCollision = getCollidingChild(result, childID, offset);
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
	void GuiElement::pushClippingBox(const Vec2i& worldPosition)
	{
		pushClippingBox(Vec4i(worldPosition.x, worldPosition.y, size.x, size.y));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::popClippingBox()
	{
		if (guiManager) guiManager->popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
}