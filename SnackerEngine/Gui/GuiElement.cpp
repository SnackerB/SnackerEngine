#include "Gui/GuiElement.h"
#include "Gui/GuiManager.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	double GuiElement::defaultFontSizeSmall{};
	double GuiElement::defaultFontSizeNormal{};
	double GuiElement::defaultFontSizeBig{};
	double GuiElement::defaultFontSizeHuge{};
	Font GuiElement::defaultFont{};
	int GuiElement::defaultBorderSmall{};
	int GuiElement::defaultBorderNormal{};
	int GuiElement::defaultBorderLarge{};
	int GuiElement::defaultBorderHuge{};
	//--------------------------------------------------------------------------------------------------
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiElement::ResizeMode> tag)
	{
		return (json.is_string() && (
			json == "SAME_AS_PARENT" ||
			json == "RESIZE_RANGE"));
	}
	//--------------------------------------------------------------------------------------------------
	template<> GuiElement::ResizeMode parseJSON(const nlohmann::json& json, JsonTag<GuiElement::ResizeMode> tag)
	{
		if (json == "SAME_AS_PARENT") return GuiElement::ResizeMode::SAME_AS_PARENT;
		else if (json == "RESIZE_RANGE") return GuiElement::ResizeMode::RESIZE_RANGE;
		else return GuiElement::ResizeMode::RESIZE_RANGE;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode)
		: position(position), size(size), resizeMode(resizeMode)
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiElement()
	{
		parseJsonOrReadFromData(name, "name", json, data, parameterNames);
		parseJsonOrReadFromData(position.x, "positionX", json, data, parameterNames);
		parseJsonOrReadFromData(position.y, "positionY", json, data, parameterNames);
		parseJsonOrReadFromData(position, "position", json, data, parameterNames);
		parseJsonOrReadFromData(size.x, "width", json, data, parameterNames);
		parseJsonOrReadFromData(size.y, "height", json, data, parameterNames);
		parseJsonOrReadFromData(size, "size", json, data, parameterNames);
		parseJsonOrReadFromData(resizeMode, "resizeMode", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.minSize.x, "minWidth", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.minSize.y, "minHeight", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.minSize, "minSize", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.maxSize.x, "maxWidth", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.maxSize.y, "maxHeight", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.maxSize, "maxSize", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.preferredSize.x, "preferredWidth", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.preferredSize.y, "preferredHeight", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHints.preferredSize, "preferredSize", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::~GuiElement()
	{
		signOffAllAnimatables();
		if (guiManager) guiManager->signOff(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(const GuiElement& other) noexcept
		: guiManager(nullptr), name(""), guiID(-1), parentID(-1), depth(0), position(other.position), size(other.size),
		resizeMode(other.resizeMode), sizeHints{ other.sizeHints.minSize, other.sizeHints.maxSize, other.sizeHints.preferredSize },
		children{}, sortedChildren{}, guiElementAnimatables{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(const GuiElement& other) noexcept
	{
		signOffAllAnimatables();
		if (guiManager) guiManager->signOff(guiID);
		guiManager = nullptr;
		name = "";
		guiID = -1;
		parentID = -1;
		depth = 0;
		position = other.position;
		size = other.size;
		resizeMode = other.resizeMode;
		sizeHints = { other.sizeHints.minSize, other.sizeHints.maxSize, other.sizeHints.preferredSize };
		children.clear();
		sortedChildren.clear();
		guiElementAnimatables.clear();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiElement(GuiElement&& other) noexcept
		: guiManager(other.guiManager), name(other.name), guiID(other.guiID), parentID(other.parentID), depth(other.depth), position(other.position), size(other.size),
		resizeMode(other.resizeMode), sizeHints{ other.sizeHints.minSize, other.sizeHints.maxSize, other.sizeHints.preferredSize }, 
		children(std::move(other.children)), sortedChildren(std::move(other.sortedChildren)), guiElementAnimatables{ std::move(other.guiElementAnimatables) }
	{
		if (guiManager) guiManager->updateMoved(*this);
		for (const auto& animatable : guiElementAnimatables) {
			animatable->element = this;
		}
		other.children.clear();
		other.sortedChildren.clear();
		other.guiElementAnimatables.clear();
		other.signOff();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiElement::operator=(GuiElement&& other) noexcept
	{
		signOffAllAnimatables();
		if (guiManager) guiManager->signOff(guiID);
		name = other.name;
		guiManager = other.guiManager;
		guiID = other.guiID;
		parentID = other.parentID;
		depth = other.depth;
		position = other.position;
		size = other.size;
		resizeMode = other.resizeMode;
		sizeHints = { other.sizeHints.minSize, other.sizeHints.maxSize, other.sizeHints.preferredSize };
		children = std::move(other.children);
		sortedChildren = std::move(other.sortedChildren);
		guiElementAnimatables = std::move(other.guiElementAnimatables);
		for (const auto& animatable : guiElementAnimatables) {
			animatable->element = this;
		}
		if (guiManager) guiManager->updateMoved(*this);
		other.children.clear();
		other.sortedChildren.clear();
		other.guiElementAnimatables.clear();
		other.signOff();
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::registerChild(GuiElement& guiElement)
	{
		if (guiManager) {
			if (guiManager->registerElementAsChild(*this, guiElement)) {
				children.push_back(guiElement.guiID);
				sortedChildren.push_back(guiElement.guiID);
				return true;
			}
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		return GuiElement::registerChild(guiElement);
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
	void GuiElement::bringToForeground()
	{
		if (guiManager) guiManager->bringToForeground(*this);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOffAtNextUpdate()
	{
		if (guiManager) guiManager->signOffAtNextUpdate(guiID);
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
	Vec2i GuiElement::clampToMinMaxSize(const Vec2i& size) const
	{
		return Vec2i(clampToMinMaxWidth(size.x), clampToMinMaxHeight(size.y));
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::clampToMinMaxWidth(int width) const
	{
		width = std::max(width, sizeHints.minSize.x);
		if (sizeHints.maxSize.x >= 0) width = std::min(width, sizeHints.maxSize.x);
		return width;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::clampToMinMaxHeight(int height) const
	{
		height = std::max(height, sizeHints.minSize.y);
		if (sizeHints.maxSize.y >= 0) height = std::min(height, sizeHints.maxSize.y);
		return height;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinSize(const Vec2i& minSize)
	{
		if (sizeHints.minSize != minSize) {
			sizeHints.minSize = minSize;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinWidth(const int& minWidth)
	{
		if (sizeHints.minSize.x != minWidth) {
			sizeHints.minSize.x = minWidth;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMinHeight(const int& minHeight)
	{
		if (sizeHints.minSize.y != minHeight) {
			sizeHints.minSize.y = minHeight;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxSize(const Vec2i& maxSize)
	{
		if (sizeHints.maxSize != maxSize) {
			sizeHints.maxSize = maxSize;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxWidth(const int& maxWidth)
	{
		if (sizeHints.maxSize.x != maxWidth) {
			sizeHints.maxSize.x = maxWidth;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setMaxHeight(const int& maxHeight)
	{
		if (sizeHints.maxSize.y != maxHeight) {
			sizeHints.maxSize.y = maxHeight;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPreferredSize(const Vec2i& preferredSize)
	{
		if (sizeHints.preferredSize != preferredSize) {
			sizeHints.preferredSize = preferredSize;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPreferredWidth(const int& preferredWidth)
	{
		if (sizeHints.preferredSize.x != preferredWidth) {
			sizeHints.preferredSize.x = preferredWidth;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPreferredHeight(const int& preferredHeight)
	{
		if (sizeHints.preferredSize.y != preferredHeight) {
			sizeHints.preferredSize.y = preferredHeight;
			if (guiManager) guiManager->registerForEnforcingLayoutsUp(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2i GuiElement::getWorldPosition() const
	{
		if (parentID > 0) return getElement(parentID)->getWorldPosition() + position;
		else return position;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getWorldPositionX() const
	{
		if (parentID > 0) return getElement(parentID)->getWorldPositionX() + position.x;
		else return position.x;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getWorldPositionY() const
	{
		if (parentID > 0) return getElement(parentID)->getWorldPositionY() + position.y;
		else return position.y;
	}
	//--------------------------------------------------------------------------------------------------
	const Vec2i GuiElement::getMouseOffset() const
	{
		if (guiManager) return guiManager->getMouseOffset(guiID);
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getMouseOffsetX() const
	{
		if (guiManager) return guiManager->getMouseOffsetX(guiID);
		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiElement::getMouseOffsetY() const
	{
		if (guiManager) return guiManager->getMouseOffsetY(guiID);
		return 0;
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
	void GuiElement::signOffWithoutNotifyingParents(const GuiID& guiID)
	{
		if (guiManager) guiManager->signOffWithoutNotifyingParent(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::draw(const Vec2i& worldPosition)
	{
		if (!guiManager) return;
		for (const auto& childID : sortedChildren) {
			auto child = getElement(childID);
			if (child) child->draw(worldPosition + child->getPosition());
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<unsigned> GuiElement::removeChild(GuiID guiElement)
	{
		std::optional<unsigned> index = getIndexIntoChildrenVector(guiElement);
		if (index.has_value())
		{
			if (guiManager) guiManager->signOffWithoutNotifyingParent(children[index.value()]);
			children.erase(children.begin() + index.value());
			for (auto it = sortedChildren.begin(); it != sortedChildren.end(); ++it) {
				if (*it == guiElement) {
					sortedChildren.erase(it);
					return index;
				}
			}
		}
		return index;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::enforceLayout()
	{
		if (!guiManager) return;
		// Set size and position of each child that has resizeMode == SAME_AS_PARENT
		for (const auto& childID : children) {
			auto child = getElement(childID);
			if (!child) continue;
			switch (child->getResizeMode())
			{
			case ResizeMode::SAME_AS_PARENT: setPositionAndSizeOfChild(childID, Vec2i(), size); break;
			case ResizeMode::RESIZE_RANGE:
			{
				Vec2i childSize = Vec2i(0, 0);
				if (child->getPreferredWidth() >= 0) childSize.x = child->getPreferredWidth();
				else {
					if (child->getPreferredWidth() == SIZE_HINT_ARBITRARY) childSize.x = child->clampToMinMaxWidth(child->getWidth());
					else if (child->getPreferredWidth() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
						if (child->getMaxWidth() >= 0) childSize.x = child->getMaxWidth();
						else childSize.x = std::max(child->getMinWidth(), getWidth());
					}
				}
				if (child->getPreferredHeight() >= 0) childSize.y = child->getPreferredHeight();
				else {
					if (child->getPreferredHeight() == SIZE_HINT_ARBITRARY) childSize.y = child->clampToMinMaxHeight(child->getHeight());
					else if (child->getPreferredHeight() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
						if (child->getMaxHeight() >= 0) childSize.y = child->getMaxHeight();
						else childSize.y = std::max(child->getMinHeight(), getHeight());
					}
				}
				setSizeOfChild(childID, childSize);
				break;
			}
			default: break;
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
	std::optional<unsigned> GuiElement::getIndexIntoChildrenVector(GuiID childID) const
	{
		for (unsigned i = 0; i < children.size(); ++i) {
			if (children[i] == childID) return i;
		}
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement* GuiElement::getElement(GuiID guiID) const
	{
		if (guiManager) return guiManager->getGuiElement(guiID);
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
	void GuiElement::bringChildToForeground(GuiID childID)
	{
		for (auto it = sortedChildren.begin(); it != sortedChildren.end(); it++) {
			if (*it == childID) {
				sortedChildren.erase(it);
				sortedChildren.push_back(childID);
				return;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::registerEnforceLayoutDown()
	{
		if (guiManager) guiManager->registerForEnforcingLayoutsDown(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::drawElement(GuiID element, Vec2i worldPosition)
	{
		GuiElement* guiElement = getElement(element);
		if (guiElement) guiElement->draw(worldPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::setPositionAndSizeOfChild(const GuiID& guiID, const Vec2i& position, const Vec2i& size)
	{
		if (!guiManager) return;
		auto child = guiManager->getGuiElement(guiID);
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
		auto child = guiManager->getGuiElement(guiID);
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
		auto child = guiManager->getGuiElement(guiID);
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
		auto child = guiManager->getGuiElement(guiID);
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
		auto child = guiManager->getGuiElement(guiID);
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
		auto child = guiManager->getGuiElement(guiID);
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
		auto child = guiManager->getGuiElement(guiID);
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
		if (offset.x >= 0 && offset.x <= getWidth() &&
			offset.y >= 0 && offset.y <= getHeight()) {
			return IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT;
		}
		else return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::isCollidingBoundingBox(const Vec2i& offset) const
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (offset.x > 0 && offset.x < mySize.x
			&& offset.y > 0 && offset.y < mySize.y);
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
			if (childCollision > 0) {
				return childCollision;
			}
			break;
		}
		case IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT:
		{
			auto child = getElement(childID);
			GuiID childCollision = child ? child->getCollidingChild(offset - child->getPosition()) : -1;
			if (childCollision > 0) {
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
		for (auto it = sortedChildren.rbegin(); it != sortedChildren.rend(); ++it) {
			auto child = getElement(*it);
			IsCollidingResult result = child ? child->isColliding(offset - child->getPosition()) : IsCollidingResult::NOT_COLLIDING;
			GuiID childCollision = getCollidingChild(result, *it, offset);
			if (childCollision > 0) return childCollision;
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
	void GuiElement::signOffAllAnimatables()
	{
		for (const auto& animatable : guiElementAnimatables) {
			animatable->element = nullptr;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signUpAnimatable(GuiElementAnimatable& animatable)
	{
		guiElementAnimatables.push_back(&animatable);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::onGuiAnimatableMove(GuiElementAnimatable* previous, GuiElementAnimatable& animatable)
	{
		for (std::size_t i = 0; i < guiElementAnimatables.size(); ++i) {
			if (guiElementAnimatables[i] == previous) {
				guiElementAnimatables[i] = &animatable;
				return;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signOffAnimatable(GuiElementAnimatable& animatable)
	{
		for (std::size_t i = 0; i < guiElementAnimatables.size(); ++i) {
			if (guiElementAnimatables[i] == &animatable) {
				guiElementAnimatables.erase(guiElementAnimatables.begin() + i);
				return;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::joinGroup(GuiGroupID groupID)
	{
		if (guiManager) return guiManager->joinGroup(guiID, groupID);
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiGroupID> GuiElement::createGroup(std::unique_ptr<GuiGroup>&& group)
	{
		if (guiManager) return guiManager->createGroup(this->guiID, std::move(group));
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiGroupID> GuiElement::groupExists(const std::string& groupName)
	{
		if (guiManager) return guiManager->groupExists(groupName);
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiElement::groupExists(GuiGroupID groupID)
	{
		if (guiManager) return guiManager->groupExists(groupID);
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	GuiGroup* GuiElement::getGroup(const std::string& groupName)
	{
		if (guiManager) return guiManager->getGroup(groupName);
		return nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	GuiGroup* GuiElement::getGroup(GuiGroupID groupID)
	{
		if (guiManager) return guiManager->getGroup(groupID);
		return nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::leaveGroup(GuiGroupID groupID)
	{
		if (guiManager) guiManager->leaveGroup(guiID, groupID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiElement::signUpAnimatable(std::unique_ptr<GuiElementAnimatable>&& animatable)
	{
		if (animatable->element == this && guiManager) guiManager->signUpAnimatable(std::move(animatable));
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animatePosition(const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementPositionAnimatable : public GuiElementValueAnimatable<Vec2i>
		{
			virtual void onAnimate(const Vec2i& currentVal) override { if (element) element->setPosition(currentVal); };
		public:
			GuiElementPositionAnimatable(GuiElement& element, const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Vec2i>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementPositionAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animatePositionX(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementPositionXAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setPositionX(currentVal); };
		public:
			GuiElementPositionXAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementPositionXAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animatePositionY(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementPositionYAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setPositionY(currentVal); };
		public:
			GuiElementPositionYAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementPositionYAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateSize(const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementSizeAnimatable : public GuiElementValueAnimatable<Vec2i>
		{
			virtual void onAnimate(const Vec2i& currentVal) override { if (element) element->setSize(currentVal); };
		public:
			GuiElementSizeAnimatable(GuiElement& element, const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Vec2i>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementSizeAnimatable >(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateWidth(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementWidthAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setWidth(currentVal); };
		public:
			GuiElementWidthAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementWidthAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateHeight(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementHeightAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setHeight(currentVal); };
		public:
			GuiElementHeightAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementHeightAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateMinSize(const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementMinSizeAnimatable : public GuiElementValueAnimatable<Vec2i>
		{
			virtual void onAnimate(const Vec2i& currentVal) override { if (element) element->setMinSize(currentVal); };
		public:
			GuiElementMinSizeAnimatable(GuiElement& element, const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Vec2i>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementMinSizeAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateMinWidth(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementMinWidthAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setMinWidth(currentVal); };
		public:
			GuiElementMinWidthAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementMinWidthAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateMinHeight(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementMinHeightAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setMinHeight(currentVal); };
		public:
			GuiElementMinHeightAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementMinHeightAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateMaxSize(const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementMaxSizeAnimatable : public GuiElementValueAnimatable<Vec2i>
		{
			virtual void onAnimate(const Vec2i& currentVal) override { if (element) element->setMaxSize(currentVal); };
		public:
			GuiElementMaxSizeAnimatable(GuiElement& element, const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Vec2i>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementMaxSizeAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable>  GuiElement::animateMaxWidth(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementMaxWidthAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setMaxWidth(currentVal); };
		public:
			GuiElementMaxWidthAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementMaxWidthAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animateMaxHeight(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementMaxHeightAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setMaxHeight(currentVal); };
		public:
			GuiElementMaxHeightAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementMaxHeightAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animatePreferredSize(const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementPreferredSizeAnimatable : public GuiElementValueAnimatable<Vec2i>
		{
			virtual void onAnimate(const Vec2i& currentVal) override { if (element) element->setPreferredSize(currentVal); };
		public:
			GuiElementPreferredSizeAnimatable(GuiElement& element, const Vec2i& startVal, const Vec2i& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Vec2i>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementPreferredSizeAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animatePreferredWidth(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementPreferredWidthAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setPreferredWidth(currentVal); };
		public:
			GuiElementPreferredWidthAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementPreferredWidthAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiElement::animatePreferredHeight(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiElementPreferredHeightAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) element->setPreferredHeight(currentVal); };
		public:
			GuiElementPreferredHeightAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiElementPreferredHeightAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
}