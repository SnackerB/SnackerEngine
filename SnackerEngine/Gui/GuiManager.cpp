#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Graphics/Meshes/Square.h"
#include "Graphics/Meshes/Triangle.h"
#include "Core/Engine.h"

#include "GuiElements/GuiPanel.h"
#include "GuiElements/GuiTextBox.h"
#include "Gui/Layouts/PositioningLayout.h"
#include "Gui/Layouts/HorizontalLayout.h"
#include "Gui/Layouts/HorizontalWeightedLayout.h"
#include "Gui/Layouts/GridLayout.h"
#include "Gui/Layouts/VerticalLayout.h"
#include "Gui/Layouts/VerticalWeightedLayout.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiElements/GuiSlider.h"
#include "Gui/Layouts/VerticalListLayout.h"
#include "Gui\Layouts\HorizontalListLayout.h"
#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui\GuiElements\GuiWindow.h"
#include "Gui\GuiElements\GuiEditBox.h"
#include "Gui\GuiElements\GuiCheckBox.h"
#include "Gui\GuiElements\GuiImage.h"
#include "Gui\GuiElements\GuiTextVariable.h"
#include "Gui\GuiElements\GuiEditVariable.h"
#include "Gui\GuiElements\VectorElements\GuiEditVariableVec.h"
#include "Gui\GuiElements\VectorElements\GuiSliderVec.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	std::unordered_map<std::string, GuiManager::parseFunction> GuiManager::elementParsingMap = std::unordered_map<std::string, GuiManager::parseFunction>();
	//--------------------------------------------------------------------------------------------------
	void GuiManager::processSignOffQueue()
	{
		for (const auto& element : signOffQueue) {
			switch (element.second)
			{
			case GuiElement::CallbackType::MOUSE_BUTTON: eventSetMouseButton.erase(element.first); break;
			case GuiElement::CallbackType::MOUSE_MOTION: eventSetMouseMotion.erase(element.first); break;
			case GuiElement::CallbackType::KEYBOARD: eventSetKeyboard.erase(element.first); break;
			case GuiElement::CallbackType::CHARACTER_INPUT: eventSetCharacterInput.erase(element.first); break;
			case GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT: eventSetMouseButtonOnElement.erase(element.first); break;
			case GuiElement::CallbackType::MOUSE_SCROLL_ON_ELEMENT: eventSetMouseScrollOnElement.erase(element.first); break;
			case GuiElement::CallbackType::MOUSE_ENTER: eventSetMouseEnter.erase(element.first); break;
			case GuiElement::CallbackType::MOUSE_LEAVE: eventSetMouseLeave.erase(element.first); break;
			case GuiElement::CallbackType::UPDATE: eventSetUpdate.erase(element.first); break;
			case GuiElement::CallbackType::DRAW_ON_TOP: eventVectorDrawOnTop.erase(std::remove(eventVectorDrawOnTop.begin(), eventVectorDrawOnTop.end(), element.first), eventVectorDrawOnTop.end()); break;
			default:
				break;
			}
		}
		signOffQueue.clear();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signUpEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType)
	{
		processSignOffQueue();
		if (guiElement.guiID <= 0) return;
		switch (callbackType)
		{
		case GuiElement::CallbackType::MOUSE_BUTTON: eventSetMouseButton.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::MOUSE_MOTION: eventSetMouseMotion.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::KEYBOARD: eventSetKeyboard.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::CHARACTER_INPUT: eventSetCharacterInput.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT: eventSetMouseButtonOnElement.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::MOUSE_SCROLL_ON_ELEMENT: eventSetMouseScrollOnElement.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::MOUSE_ENTER: eventSetMouseEnter.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::MOUSE_LEAVE: eventSetMouseLeave.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::UPDATE: eventSetUpdate.insert(guiElement.guiID); break;
		case GuiElement::CallbackType::DRAW_ON_TOP: eventVectorDrawOnTop.push_back(guiElement.guiID); break;
		default:
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signOffEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType)
	{
		// push into signOffQueue. Event will be signed off the next time update() is called.
		signOffQueue.push_back(std::make_pair(guiElement.guiID, callbackType));
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiID GuiManager::getCollidingElement()
	{
		return registeredGuiElements[parentElement]->getCollidingChild(currentMousePosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::bringToForeground(const GuiElement& guiElement)
	{
		if (guiElement.parentID < 0) return;
		GuiElement* parent = getGuiElement(guiElement.parentID);
		if (parent) {
			parent->bringChildToForeground(guiElement.guiID);
			bringToForeground(*parent);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::pushClippingBox(const Vec4i& clippingBox)
	{
		Vec4i alteredClippingBox = clippingBox;
		alteredClippingBox.y = screenDims.y - clippingBox.y - clippingBox.w;
		alteredClippingBox.z = std::max(alteredClippingBox.z, 0);
		alteredClippingBox.w = std::max(alteredClippingBox.w, 0);
		if (!clippingBoxStack.empty()) {
			const Vec4i& previousClippingBox = clippingBoxStack.back();
			if (alteredClippingBox.x < previousClippingBox.x) {
				alteredClippingBox.z -= (previousClippingBox.x - alteredClippingBox.x);
				alteredClippingBox.x = previousClippingBox.x;
			}
			if (alteredClippingBox.y < previousClippingBox.y) {
				alteredClippingBox.w -= (previousClippingBox.y - alteredClippingBox.y);
				alteredClippingBox.y = previousClippingBox.y;
			}
			alteredClippingBox.z = std::max(0, std::min(alteredClippingBox.z, previousClippingBox.x + previousClippingBox.z - alteredClippingBox.x));
			alteredClippingBox.w = std::max(0, std::min(alteredClippingBox.w, previousClippingBox.y + previousClippingBox.w - alteredClippingBox.y));
		}
		clippingBoxStack.push_back(alteredClippingBox);
		if (doClipping) Renderer::enableScissorTest(alteredClippingBox);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::popClippingBox()
	{
		if (!clippingBoxStack.empty()) {
			clippingBoxStack.pop_back();
			if (!clippingBoxStack.empty()) {
				if (doClipping) Renderer::enableScissorTest(clippingBoxStack.back());
			}
			else {
				Renderer::disableScissorTest();
			}
		}
		else {
			Renderer::disableScissorTest();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::removeFromEnforceLayoutQueues(const GuiID guiID)
	{
		if (guiID >= registeredGuiElements.size() || guiID < 0 || !registeredGuiElements[guiID])
		{
			return;
		}
		auto it = enforceLayoutQueueUp.find(registeredGuiElements[guiID]->depth);
		if (it != enforceLayoutQueueUp.end()) {
			it->second.erase(guiID);
		}
		auto it2 = enforceLayoutQueueDown.find(registeredGuiElements[guiID]->depth);
		if (it2 != enforceLayoutQueueDown.end()) {
			it2->second.erase(guiID);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::registerForEnforcingLayoutsUpAndDown(const GuiID& guiID)
	{
		registerForEnforcingLayoutsUp(guiID);
		registerForEnforcingLayoutsDown(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::registerForEnforcingLayoutsUp(const GuiID& guiID)
	{
		if (guiID >= registeredGuiElements.size() || guiID < 0 || !registeredGuiElements[guiID])
		{
			warningLogger << LOGGER::BEGIN << "Tried to register invalid guiElement to EnforceLayoutQueue." << LOGGER::ENDL;
			return;
		}
		auto it = enforceLayoutQueueUp.find(registeredGuiElements[guiID]->depth);
		if (it != enforceLayoutQueueUp.end()) {
			it->second.insert(guiID);
		}
		else {
			enforceLayoutQueueUp.insert(std::make_pair(registeredGuiElements[guiID]->depth, std::set<GuiID>({ guiID })));
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::registerForEnforcingLayoutsDown(const GuiID& guiID)
	{
		if (guiID >= registeredGuiElements.size() || guiID < 0 || !registeredGuiElements[guiID])
		{
			warningLogger << LOGGER::BEGIN << "Tried to register invalid guiElement to EnforceLayoutQueue." << LOGGER::ENDL;
			return;
		}
		auto it = enforceLayoutQueueDown.find(registeredGuiElements[guiID]->depth);
		if (it != enforceLayoutQueueDown.end()) {
			it->second.insert(guiID);
		}
		else {
			enforceLayoutQueueDown.insert(std::make_pair(registeredGuiElements[guiID]->depth, std::set<GuiID>({ guiID })));
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::enforceLayouts()
	{
		/// First go from bottom to top and enforce all layouts
		for (auto rit = enforceLayoutQueueUp.rbegin(); rit != enforceLayoutQueueUp.rend(); ++rit)
		{
			for (const GuiID& guiID : rit->second)
			{
				if (guiID < registeredGuiElements.size() && guiID >= 0 && registeredGuiElements[guiID]) {
					GuiID parentID = registeredGuiElements[guiID]->parentID;
					if (parentID < registeredGuiElements.size() && parentID >= 0 && registeredGuiElements[parentID]) {
						registeredGuiElements[parentID]->enforceLayout();
					}
				}
			}
		}
		enforceLayoutQueueUp.clear();
		/// Now go from the top down and enforce all layouts
		for (const auto& it : enforceLayoutQueueDown)
		{
			for (const GuiID& guiID : it.second)
			{
				if (guiID < registeredGuiElements.size() && guiID >= 0 && registeredGuiElements[guiID]) {
					registeredGuiElements[guiID]->enforceLayout();
				}
			}
		}
		enforceLayoutQueueDown.clear();
		// Callback mouseMotion, because we could collide with moved elements!
		callbackMouseMotion(currentMousePosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signUpAnimatable(std::unique_ptr<GuiElementAnimatable>&& animatable)
	{
		guiElementAnimatables.emplace_back(std::move(animatable));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::updateAnimatables(double dt)
	{
		auto it = guiElementAnimatables.begin();
		while (it != guiElementAnimatables.end()) {
			(*it)->update(dt);
			if ((*it)->isFinished() || (*it)->isNull()) {
				it = guiElementAnimatables.erase(it);
			}
			else {
				it++;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiElement::GuiID> GuiManager::getGuiElement(const std::string& name)
	{
		auto it = namedElements.find(name);
		if (it == namedElements.end()) return {};
		return it->second;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::clearNamedElements()
	{
		for (const auto& it : namedElements) {
			if (registeredGuiElements[it.second]) registeredGuiElements[it.second]->name = "";
		}
		namedElements.clear();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::deletElementName(const std::string& name)
	{
		auto it = namedElements.find(name);
		if (it == namedElements.end()) {
			warningLogger << LOGGER::BEGIN << "Could not find element with name \"" << name << "\"." << LOGGER::ENDL;
		}
		else {
			registeredGuiElements[it->second]->name = "";
			namedElements.erase(it);
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiGroupID GuiManager::getNewGroupID()
	{
		if (registeredGuiGroupsCount >= maxGuiGroups)
		{
			// Resize vector and add new available GuiGroupID slots accordingly. For now: double size everytime this happens and send warning!
			guiGroups.reserve(static_cast<std::size_t>(maxGuiGroups) * 2);
			for (GuiID id = maxGuiGroups; id < 2 * maxGuiGroups; ++id)
			{
				availableGuiGroupIDs.push(id);
				guiGroups.push_back(nullptr);
			}
			warningLogger << LOGGER::BEGIN << "maximum amount of guiGroups exceeded. Resizing guiManager to be able to store "
				<< 2 * maxGuiGroups << " guiGroups!" << LOGGER::ENDL;
			maxGuiGroups *= 2;
		}
		// Take ID from the front of the queue
		GuiID id = availableGuiGroupIDs.front();
		availableGuiGroupIDs.pop();
		return id;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiGroupID> GuiManager::joinGroup(GuiID guiID, const std::string& groupName)
	{
		auto it = guiGroupMap.find(groupName);
		if (it != guiGroupMap.end()) {
			if (joinGroup(guiID, it->second)) return it->second;
		}
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiManager::joinGroup(GuiID guiID, GuiGroupID groupID)
	{
		if (groupID >= 0 && groupID < guiGroups.size() && guiGroups[groupID] != nullptr) {
			return guiGroups[groupID]->join(guiID);
		}
		return false;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiGroupID> GuiManager::createGroup(GuiID guiID, std::unique_ptr<GuiGroup>&& group)
	{
		if (!group->name.empty() && groupExists(group->name)) return {};
		GuiGroupID groupID = getNewGroupID();
		if (!group->name.empty()) guiGroupMap.insert(std::make_pair(group->name, groupID));
		guiGroups[groupID] = std::move(group);
		guiGroups[groupID]->guiManager = this;
		registeredGuiGroupsCount++;
		guiGroups[groupID]->join(guiID);
		return groupID;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiGroupID> GuiManager::groupExists(const std::string& groupName)
	{
		auto it = guiGroupMap.find(groupName);
		if (it != guiGroupMap.end()) return it->second;
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiManager::groupExists(GuiGroupID groupID)
	{
		return groupID >= 0 && groupID < guiGroups.size() && guiGroups[groupID] != nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	GuiGroup* GuiManager::getGroup(const std::string& groupName)
	{
		auto it = guiGroupMap.find(groupName);
		if (it != guiGroupMap.end()) return getGroup(it->second);
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	GuiGroup* GuiManager::getGroup(GuiGroupID groupID)
	{
		if (groupExists(groupID)) return guiGroups[groupID].get();
		else return {};
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::leaveGroup(GuiID guiID, const std::string& groupName)
	{
		auto it = guiGroupMap.find(groupName);
		if (it != guiGroupMap.end()) leaveGroup(guiID, it->second);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::leaveGroup(GuiID guiID, GuiGroupID groupID)
	{
		if (!groupExists(groupID)) return;
		guiGroups[groupID]->leave(guiID);
		if (guiGroups[groupID]->elements.empty()) {
			if (!guiGroups[groupID]->name.empty()) guiGroupMap.erase(guiGroups[groupID]->name);
			guiGroups[groupID] = nullptr;
			availableGuiGroupIDs.push(groupID);
			registeredGuiGroupsCount--;
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiID GuiManager::getNewGuiID()
	{
		if (registeredGuiElementsCount >= maxGuiElements)
		{
			// Resize vector and add new available GuiID slots accordingly. For now: double size everytime this happens and send warning!
			registeredGuiElements.resize(static_cast<std::size_t>(maxGuiElements) * 2 + 1, nullptr);
			ownedGuiElements.reserve(static_cast<std::size_t>(maxGuiElements) * 2 + 1);
			for (unsigned int i = maxGuiElements + 1; i < static_cast<std::size_t>(maxGuiElements) * 2 + 1; ++i) {
				ownedGuiElements.push_back(nullptr);
			}
			//ownedGuiElements.resize(static_cast<std::size_t>(maxGuiElements) * 2 + 1, nullptr);
			for (GuiID id = maxGuiElements + 1; id <= 2 * maxGuiElements; ++id)
			{
				availableGuiIDs.push(id);
			}
			warningLogger << LOGGER::BEGIN << "maximum amount of guiElements exceeded. Resizing guiManager to be able to store "
				<< 2 * maxGuiElements << " guiElements!" << LOGGER::ENDL;
			maxGuiElements *= 2;
		}
		// Take ID from the front of the queue
		GuiID id = availableGuiIDs.front();
		availableGuiIDs.pop();
		return id;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::computeViewAndProjection()
	{
		screenDims = Renderer::getScreenDimensions();
		viewMatrix = Mat4f::Identity();
		projectionMatrix = Mat4f::TranslateAndScale({ -1.0f, 1.0f, 0.0f }, { 2.0f / static_cast<float>(screenDims.x), 2.0f / static_cast<float>(screenDims.y), 0.0f });
		// TODO: For now this is only for GUI on screen. Later adapt this so that gui can be displayed anywhere!
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::clearEventQueues(const GuiID& guiID)
	{
		eventSetMouseButton.erase(guiID);
		eventSetMouseMotion.erase(guiID);
		eventSetKeyboard.erase(guiID);
		eventSetCharacterInput.erase(guiID);
		eventSetMouseButtonOnElement.erase(guiID);
		eventSetMouseScrollOnElement.erase(guiID);
		eventSetMouseEnter.erase(guiID);
		eventSetMouseLeave.erase(guiID);
		eventSetUpdate.erase(guiID);
		eventVectorDrawOnTop.erase(std::remove(eventVectorDrawOnTop.begin(), eventVectorDrawOnTop.end(), guiID), eventVectorDrawOnTop.end());
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signOff(const GuiID& guiElement)
	{
		if (guiElement <= 0) return;
		if (guiElement > maxGuiElements || registeredGuiElements[guiElement] == nullptr)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		GuiElement& element = *registeredGuiElements[guiElement];
		/// Remove element from parent
		if (element.parentID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement with invalid parent element" << LOGGER::ENDL;
			return;
		}
		if (registeredGuiElements[element.parentID]) registeredGuiElements[element.parentID]->removeChild(element.getGuiID());
		else signOffWithoutNotifyingParent(guiElement);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signOffWithoutNotifyingParent(const GuiID guiElement)
	{
		if (guiElement <= 0) return;
		if (guiElement > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		GuiElement& element = *registeredGuiElements[guiElement];
		// If the element is in any groups, it should leave them
		element.leaveAllGroups();
		// Sign off element which will in turn sign off its children
		element.signOff();
		registeredGuiElements[guiElement] = nullptr;
		registeredGuiElementsCount--;
		// Clear event queues
		clearEventQueues(guiElement);
		if (lastMouseHoverElement == guiElement) lastMouseHoverElement = 0;
		/// Sign off from EnforceLayoutQueues
		removeFromEnforceLayoutQueues(guiElement);
		// Sign off from animations
		// deleteAnimations(guiElement); TODO: Uncomment
		// If the element has a name, delete from namedElements map
		if (!element.name.empty()) {
			namedElements.erase(element.name);
		}
		// Delete element if it is stored by guiManager
		if (ownedGuiElements[guiElement]) {
			ownedGuiElements[guiElement] = nullptr;
			ownedGuiElementsCount--;
		}
		// Add guiID back to queue!
		availableGuiIDs.push(guiElement);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::updateMoved(GuiElement& guiElement)
	{
		if (guiElement.guiID <= 0) return;
		if (guiElement.guiID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to update guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		registeredGuiElements[guiElement.guiID] = &guiElement;
		// auto it = animations.find(guiElement.guiID); TODO: Uncomment
		//if (it != animations.end()) {
		//	for (auto& animatablePtr : it->second)
		//	{
		//		animatablePtr->onGuiElementMove(guiElement);
		//	}
		//}
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement* GuiManager::getGuiElement(const GuiID& guiID)
	{
		if (guiID < 0 || guiID >= registeredGuiElements.size() || !registeredGuiElements[guiID]) {
			errorLogger << LOGGER::BEGIN << "Tried to access invalid guiElement with guiID " << guiID << LOGGER::ENDL;
			return nullptr;
		}
		return registeredGuiElements[guiID];
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiManager::registerElementAsChild(GuiElement& parent, GuiElement& child)
	{
		if (!parent.isValid()) {
			warningLogger << LOGGER::BEGIN << "Tried to set guiElement as child of an invalid guiElement!" << LOGGER::ENDL;
			return false;
		}
		GuiID newGuiID = getNewGuiID();
		registeredGuiElements[newGuiID] = &child;
		registeredGuiElementsCount++;
		child.guiID = newGuiID;
		child.guiManager = this;
		child.parentID = parent.guiID;
		child.depth = parent.depth + 1;
		child.onRegister();
		registerForEnforcingLayoutsUpAndDown(child.guiID);
		// Callback mouseMotion, because we could collide with the new element!
		callbackMouseMotion(currentMousePosition);
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiManager::getMouseOffset(GuiID guiID)
	{
		Vec2i mouseOffset = currentMousePosition;
		while (guiID >= 0) {
			GuiID parentID = registeredGuiElements[guiID]->parentID;
			if (parentID >= 0) {
				mouseOffset -= registeredGuiElements[parentID]->getChildOffset(guiID);
			}
			else {
				mouseOffset -= registeredGuiElements[guiID]->position;
			}
			guiID = parentID;
		}
		return mouseOffset;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiManager::getMouseOffsetX(GuiID guiID)
	{
		int mouseOffsetX = currentMousePosition.x;
		while (guiID >= 0) {
			GuiID parentID = registeredGuiElements[guiID]->parentID;
			if (parentID >= 0) {
				mouseOffsetX -= registeredGuiElements[parentID]->getChildOffset(guiID).x;
			}
			else {
				mouseOffsetX -= registeredGuiElements[guiID]->position.x;
			}
			guiID = parentID;
		}
		return mouseOffsetX;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiManager::getMouseOffsetY(GuiID guiID)
	{
		int mouseOffsetY = currentMousePosition.y;
		while (guiID >= 0) {
			GuiID parentID = registeredGuiElements[guiID]->parentID;
			if (parentID >= 0) {
				mouseOffsetY -= registeredGuiElements[parentID]->getChildOffset(guiID).y;
			}
			else {
				mouseOffsetY -= registeredGuiElements[guiID]->position.y;
			}
			guiID = parentID;
		}
		return mouseOffsetY;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiManager::getWorldOffset(GuiID guiID)
	{
		Vec2i offset(0, 0);
		while (guiID > 0) {
			GuiID parentID = registeredGuiElements[guiID]->parentID;
			if (parentID >= 0) {
				offset += registeredGuiElements[parentID]->getChildOffset(guiID);
			}
			else {
				offset += registeredGuiElements[guiID]->position;
			}
			guiID = parentID;
		}
		return offset;
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiElement::GuiID> GuiManager::getLowestCollidingElementInEventSet(const std::unordered_set<GuiID>& eventSet)
	{
		if (eventSet.empty()) return {};
		return getLowestCollidingChildInEventSet(parentElement, currentMousePosition, eventSet);
	}
	//--------------------------------------------------------------------------------------------------
	std::optional<GuiElement::GuiID> GuiManager::getLowestCollidingChildInEventSet(const GuiID& parentID, const Vec2i& offset, const std::unordered_set<GuiID>& eventSet)
	{
		auto parent = getGuiElement(parentID);
		if (!parent) return {};
		for (auto it = parent->children.rbegin(); it != parent->children.rend(); ++it) {
			auto child = getGuiElement(*it);
			if (!child) continue;
			switch (child->isColliding(offset - parent->getChildOffset(*it))) {
			case GuiElement::IsCollidingResult::COLLIDE_CHILD:
			case GuiElement::IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT:
			{
				auto result = getLowestCollidingChildInEventSet(*it, offset - parent->getChildOffset(*it), eventSet);
				if (result.has_value()) return result;
				if (eventSet.find(*it) != eventSet.end()) return { *it };
				break;
			}
			case GuiElement::IsCollidingResult::COLLIDE_STRONG:
			{
				if (eventSet.find(*it) != eventSet.end()) return { *it };
				return {};
			}
			case GuiElement::IsCollidingResult::NOT_COLLIDING:
			default:
			{
				break;
			}
			}
		}
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::initialize()
	{
		// Initialize parser map by registering all standard GuiElement derived classes
		registerGuiElementType<GuiElement>();
		registerGuiElementType<GuiPanel>();
		registerGuiElementType<GuiTextBox>();
		registerGuiElementType<GuiPositioningLayout>();
		registerGuiElementType<GuiHorizontalLayout>();
		registerGuiElementType<GuiHorizontalListLayout>();
		registerGuiElementType<GuiHorizontalWeightedLayout>();
		registerGuiElementType<GuiVerticalWeightedLayout>();
		registerGuiElementType<GuiGridLayout>();
		registerGuiElementType<GuiVerticalLayout>();
		registerGuiElementType<GuiButton>();
		registerGuiElementType<GuiSliderFloat>("_FLOAT");
		registerGuiElementType<GuiSliderDouble>("_DOUBLE");
		registerGuiElementType<GuiSliderInt>("_INT");
		registerGuiElementType<GuiSliderUnsignedInt>("_UNSIGNED_INT");
		registerGuiElementType<GuiVerticalListLayout>();
		registerGuiElementType<GuiVerticalScrollingListLayout>();
		registerGuiElementType<GuiWindow>();
		registerGuiElementType<GuiEditBox>();
		registerGuiElementType<GuiCheckBox>();
		registerGuiElementType<GuiImage>();
		registerGuiElementType<GuiTextVariableFloat>("_FLOAT");
		registerGuiElementType<GuiTextVariableDouble>("_DOUBLE");
		registerGuiElementType<GuiTextVariableInt>("_INT");
		registerGuiElementType<GuiTextVariableUnsignedInt>("_UNSIGNED_INT");
		registerGuiElementType<GuiTextVariableVec2f>("_VEC2_FLOAT");
		registerGuiElementType<GuiTextVariableVec2d>("_VEC2_DOUBLE");
		registerGuiElementType<GuiTextVariableVec2i>("_VEC2_INT");
		registerGuiElementType<GuiTextVariableVec3f>("_VEC3_FLOAT");
		registerGuiElementType<GuiTextVariableVec3d>("_VEC3_DOUBLE");
		registerGuiElementType<GuiTextVariableVec3i>("_VEC3_INT");
		registerGuiElementType<GuiTextVariableVec4f>("_VEC4_FLOAT");
		registerGuiElementType<GuiTextVariableVec4d>("_VEC4_DOUBLE");
		registerGuiElementType<GuiTextVariableVec4i>("_VEC4_INT");
		registerGuiElementType<GuiEditVariableFloat>("_FLOAT");
		registerGuiElementType<GuiEditVariableDouble>("_DOUBLE");
		registerGuiElementType<GuiEditVariableInt>("_INT");
		registerGuiElementType<GuiEditVariableUnsignedInt>("_UNSIGNED_INT");
		registerGuiElementType<GuiEditVariableVec2f>("_FLOAT");
		registerGuiElementType<GuiEditVariableVec2d>("_DOUBLE");
		registerGuiElementType<GuiEditVariableVec2i>("_INT");
		registerGuiElementType<GuiEditVariableVec3f>("_FLOAT");
		registerGuiElementType<GuiEditVariableVec3d>("_DOUBLE");
		registerGuiElementType<GuiEditVariableVec3i>("_INT");
		registerGuiElementType<GuiEditVariableVec4f>("_FLOAT");
		registerGuiElementType<GuiEditVariableVec4d>("_DOUBLE");
		registerGuiElementType<GuiEditVariableVec4i>("_INT");
		registerGuiElementType<GuiSliderVec2f>("_FLOAT");
		registerGuiElementType<GuiSliderVec2d>("_DOUBLE");
		registerGuiElementType<GuiSliderVec2i>("_INT");
		registerGuiElementType<GuiSliderVec3f>("_FLOAT");
		registerGuiElementType<GuiSliderVec3d>("_DOUBLE");
		registerGuiElementType<GuiSliderVec3i>("_INT");
		registerGuiElementType<GuiSliderVec4f>("_FLOAT");
		registerGuiElementType<GuiSliderVec4d>("_DOUBLE");
		registerGuiElementType<GuiSliderVec4i>("_INT");
		// Initialize static default font
		GuiElement::defaultFont = Font("fonts/Arial.ttf");
		// Initialize static default background shader
		GuiPanel::defaultBackgroundShader = Shader("shaders/gui/simpleAlphaColor.shader");
		// Initialize static default image shader
		GuiImage::defaultImageShader = Shader("shaders/gui/basicTexture.shader");
		/// Initialize static default checkmark texture
		auto result = Texture::Load2D("gui/textures/checkmark.png");
		if (result.second) GuiCheckBox::defaultCheckMarkTexture = result.first;
		/// Initialize static default checkmark shader
		GuiCheckBox::defaultCheckMarkShader = Shader("shaders/gui/msdfShader.shader");
		/// Compute default parameters based on GuiScale
		double guiScale = Engine::getDPI().y / 250.0;
		recomputeDefaultGuiElementValues(guiScale);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::terminate()
	{
		// Cleanup
		GuiElement::defaultFont = Font();
		GuiPanel::defaultBackgroundShader = Shader();
		GuiImage::defaultImageShader = Shader();
		GuiCheckBox::defaultCheckMarkShader = Shader();
		GuiCheckBox::defaultCheckMarkTexture = Texture();
	}
	//--------------------------------------------------------------------------------------------------
	GuiManager::GuiManager(const unsigned int& startingSize)
		: registeredGuiElements(startingSize + 1, nullptr), ownedGuiElements{},
		availableGuiIDs{}, maxGuiElements(startingSize), registeredGuiElementsCount(0),
		ownedGuiElementsCount(0), viewMatrix{}, projectionMatrix{}, parentElement(0), currentMousePosition{},
		lastMouseHoverElement(0), eventSetMouseButton{}, eventSetMouseMotion{}, eventSetKeyboard{},
		eventSetCharacterInput{}, eventSetMouseButtonOnElement{}, eventSetMouseScrollOnElement{},
		eventSetMouseEnter{}, eventSetMouseLeave{}, eventSetUpdate{}, eventVectorDrawOnTop{}, signOffQueue{}, squareModel{},
		triangleModel{}, clippingBoxStack{}, doClipping(true), enforceLayoutQueueUp{}, enforceLayoutQueueDown{}, // animations{}, TODO: Uncomment
		screenDims{}, namedElements{}, guiGroupMap{}, guiGroups{},
		registeredGuiGroupsCount(0), availableGuiGroupIDs{}, maxGuiGroups(startingSize)
	{
		/// Initializes the vector of owned GuiElements
		ownedGuiElements.reserve(std::size_t(startingSize) + 1);
		for (unsigned int i = 0; i < startingSize + 1; ++i) {
			ownedGuiElements.push_back(nullptr);
		}
		// Initializes queue with all possible GuiIDs. GuiID = 0 is reserved for invalid guiElements.
		for (GuiID id = 1; id <= static_cast<int>(startingSize); ++id)
		{
			availableGuiIDs.push(id);
		}
		// Initializes group vector and the queue with all possible GuiGroupIDs
		guiGroups.reserve(startingSize);
		for (GuiGroupID id = 0; id < static_cast<int>(startingSize); ++id)
		{
			guiGroups.push_back(nullptr);
			availableGuiGroupIDs.push(id);
		}
		// Compute view and projection matrix
		computeViewAndProjection();
		// Initialize parent GuiElement
		ownedGuiElements[parentElement] = std::make_unique<GuiElement>(Vec2i(0, 0), Renderer::getScreenDimensions(), GuiElement::ResizeMode::RESIZE_RANGE);
		ownedGuiElements[parentElement]->guiID = 0;
		ownedGuiElements[parentElement]->parentID = -1;
		ownedGuiElements[parentElement]->guiManager = this;
		registeredGuiElements[parentElement] = ownedGuiElements[parentElement].get();
		// Computes screen dims
		screenDims = Renderer::getScreenDimensions();
	}
	//--------------------------------------------------------------------------------------------------
	GuiManager::~GuiManager()
	{
		clear();
		signOff(parentElement);
		ownedGuiElements.clear();
		namedElements.clear();
		guiGroups.clear();
		guiGroupMap.clear();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::registerElement(GuiElement& guiElement)
	{
		GuiID newGuiID = getNewGuiID();
		registeredGuiElements[newGuiID] = &guiElement;
		registeredGuiElementsCount++;
		guiElement.guiID = newGuiID;
		guiElement.guiManager = this;
		guiElement.parentID = 0;
		guiElement.depth = 1;
		registeredGuiElements[parentElement]->children.push_back(guiElement.guiID);
		registeredGuiElements[parentElement]->sortedChildren.push_back(guiElement.guiID);
		guiElement.onRegister();
		registerForEnforcingLayoutsUpAndDown(guiElement.guiID);
		// Callback mouseMotion, because we could collide with the new element!
		callbackMouseMotion(currentMousePosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::registerAndMoveElementPtr(std::unique_ptr<GuiElement>&& guiElement)
	{
		if (guiElement->isValid()) {
			if (guiElement->guiManager == this) {
				GuiID guiID = guiElement->guiID;
				ownedGuiElements[guiID] = std::move(guiElement);
				ownedGuiElementsCount++;
				registeredGuiElements[guiID] = ownedGuiElements[guiID].get();
				auto& element = *ownedGuiElements[guiID];
				if (element.parentID < 0) {
					element.parentID = 0;
					registeredGuiElements[parentElement]->children.push_back(element.guiID);
					registeredGuiElements[parentElement]->sortedChildren.push_back(element.guiID);
					element.depth = 1;
				}
				element.onRegister();
				registerForEnforcingLayoutsUpAndDown(element.guiID);
				// Callback mouseMotion, because we could collide with the new element!
				callbackMouseMotion(currentMousePosition);
				return;
			}
			else {
				warningLogger << LOGGER::BEGIN << "Tried to register GuiElement that was already registered at a different GuiManager!" << LOGGER::ENDL;
				return;
			}
		}
		GuiID newGuiID = getNewGuiID();
		ownedGuiElements[newGuiID] = std::move(guiElement);
		ownedGuiElementsCount++;
		registeredGuiElements[newGuiID] = ownedGuiElements[newGuiID].get();
		registeredGuiElementsCount++;
		auto& element = *ownedGuiElements[newGuiID];
		element.guiID = newGuiID;
		element.guiManager = this;
		element.parentID = 0;
		element.depth = 1;
		registeredGuiElements[parentElement]->children.push_back(element.guiID);
		registeredGuiElements[parentElement]->sortedChildren.push_back(element.guiID);
		element.onRegister();
		registerForEnforcingLayoutsUpAndDown(element.guiID);
		// Callback mouseMotion, because we could collide with the new element!
		callbackMouseMotion(currentMousePosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::moveElementPtr(std::unique_ptr<GuiElement>&& guiElement)
	{
		if (!guiElement->isValid()) {
			warningLogger << LOGGER::BEGIN << "Tried to move an invalid guiElement to a guiManager. Try to register the element first!" << LOGGER::ENDL;
			return;
		}
		GuiID guiID = guiElement->guiID;
		ownedGuiElements[guiID] = std::move(guiElement);
		registeredGuiElements[guiID] = ownedGuiElements[guiID].get();
		ownedGuiElementsCount++;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::clear()
	{
		for (unsigned int i = 1; i < ownedGuiElements.size(); ++i) {
			if (ownedGuiElements[i] != nullptr) {
				signOff(ownedGuiElements[i]->guiID);
			}
		}
		for (unsigned int i = 1; i < registeredGuiElements.size(); ++i) {
			if (registeredGuiElements[i] != nullptr) {
				signOff(registeredGuiElements[i]->guiID);
			}
		}
		namedElements.clear();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::setUniformViewAndProjectionMatrices(const Shader& shader)
	{
		shader.setUniform<Mat4f>("u_view", viewMatrix);
		shader.setUniform<Mat4f>("u_projection", projectionMatrix);
	}
	//--------------------------------------------------------------------------------------------------
	const Mat4f& GuiManager::getViewMatrix() const
	{
		return viewMatrix;
	}
	//--------------------------------------------------------------------------------------------------
	const Mat4f& GuiManager::getProjectionMatrix() const
	{
		return projectionMatrix;
	}
	//--------------------------------------------------------------------------------------------------
	Model GuiManager::getModelSquare()
	{
		if (squareModel.isValid())
			return squareModel;
		squareModel = Model(createMeshSquare());
		return squareModel;
	}
	//--------------------------------------------------------------------------------------------------
	Model GuiManager::getModelTriangle()
	{
		if (triangleModel.isValid())
			return triangleModel;
		triangleModel = Model(createMeshTriangle());
		return triangleModel;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiManager::clipSizeToMinMaxSize(const Vec2i& size, const GuiID& guiElement)
	{
		if (guiElement > registeredGuiElements.size() || !registeredGuiElements[guiElement]) return Vec2i();
		GuiElement& element = *registeredGuiElements[guiElement];
		Vec2i maxSize = element.getMaxSize();
		Vec2i minSize = element.getMinSize();
		Vec2i result = size;
		if (maxSize.x != -1) result.x = std::min(result.x, maxSize.x);
		if (maxSize.y != -1) result.y = std::min(result.y, maxSize.y);
		result.x = std::max(result.x, minSize.x);
		result.y = std::max(result.y, minSize.y);
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	int GuiManager::clipWidthToMinMaxWidth(const int& width, const GuiID& guiElement)
	{
		if (guiElement > registeredGuiElements.size() || !registeredGuiElements[guiElement]) return 0;
		GuiElement& element = *registeredGuiElements[guiElement];
		int maxWidth = element.getMaxSize().x;
		if (maxWidth == -1)
			return std::max(width, element.getMinSize().x);
		else
			return std::max(std::min(width, maxWidth), element.getMinSize().x);
	}
	//--------------------------------------------------------------------------------------------------
	int GuiManager::clipHeightToMinMaxHeight(const int& height, const GuiID& guiElement)
	{
		if (guiElement > registeredGuiElements.size() || !registeredGuiElements[guiElement]) return 0;
		GuiElement& element = *registeredGuiElements[guiElement];
		int maxHeight = element.getMaxSize().y;
		if (maxHeight == -1)
			return std::max(height, element.getMinSize().y);
		else
			return std::max(std::min(height, maxHeight), element.getMinSize().y);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::disableClippingBoxes()
	{
		doClipping = false;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::enableClippingBoxes()
	{
		doClipping = true;
	}
	//--------------------------------------------------------------------------------------------------
	std::set<std::string> GuiManager::extractParameterNames(const nlohmann::json& json)
	{
		std::set<std::string> result;
		for (const auto& item : json.items()) {
			result.insert(item.key());
		}
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::printWarningsForUnusedParameterNames(const std::set<std::string>& parameterNames)
	{
		for (const std::string& name : parameterNames) {
			warningLogger << LOGGER::BEGIN << "Unused option \"" << name << "\"" << LOGGER::ENDL;
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElement> GuiManager::loadGuiElement(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		if (!json.is_object()) {
			warningLogger << LOGGER::BEGIN << "Error while parsing JSON file: expected type object, but was " << json.type_name() << LOGGER::ENDL;
			return nullptr;
		}
		if (!json.contains("typeID")) {
			warningLogger << LOGGER::BEGIN << "Error while parsing JSON file: All GuiElements must contain field \"typeID\"" << LOGGER::ENDL;
			return nullptr;
		}
		if (!json["typeID"].is_string()) {
			warningLogger << LOGGER::BEGIN << "Error while parsing JSON file: field \"typeID\" must be of type string" << LOGGER::ENDL;
			return nullptr;
		}
		auto it = elementParsingMap.find(json["typeID"]);
		if (it == elementParsingMap.end()) {
			warningLogger << LOGGER::BEGIN << "Tried to parse GuiElement with unknown typeID: " << json["typeID"] << LOGGER::ENDL;
			return nullptr;
		}
		if (parameterNames) parameterNames->erase("typeID");
		return std::move(it->second(json, data, parameterNames));
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiManager::loadGuiElementAndRegisterAsChild(const nlohmann::json& json, const nlohmann::json* data, GuiElement& parentElement)
	{
		std::set<std::string> parameterNames;
		if (checkForUnusedParameters) parameterNames = extractParameterNames(json);
		std::unique_ptr<GuiElement> element = loadGuiElement(json, data, &parameterNames);
		if (!element) return false;
		if (json.contains("layoutOptions")) {
			if (json["layoutOptions"].is_object()) {
				if (checkForUnusedParameters) {
					std::set<std::string> layoutParameterNames = extractParameterNames(json["layoutOptions"]);
					parentElement.registerChild(*element, json["layoutOptions"], data, &layoutParameterNames);
					printWarningsForUnusedParameterNames(layoutParameterNames);
				}
				else {
					parentElement.registerChild(*element, json["layoutOptions"], data);
				}
			}
			else {
				warningLogger << LOGGER::BEGIN << "\"layoutOptions\" field must be an object containing the layout options, but was of type"
					<< json["layoutOptions"].type_name() << LOGGER::ENDL;
				parentElement.registerChild(*element);
			}
			if (checkForUnusedParameters) parameterNames.erase("layoutOptions");
		}
		else {
			parentElement.registerChild(*element);
		}
		// Parse potential children
		bool result = true;
		if (json.contains("children")) {
			result = loadGuiAndRegisterAsChildren(json["children"], data, *element);
			if (checkForUnusedParameters) parameterNames.erase("children");
		}
		// Check if this element has a name
		if (json.contains("name")) {
			if (json["name"].is_string()) {
				auto it = namedElements.find("name");
				if (it != namedElements.end()) {
					warningLogger << LOGGER::BEGIN << "An element with name \"" << json["name"] << "\" was already registered." << LOGGER::ENDL;
				}
				else {
					element->name = json["name"];
					namedElements.insert({ element->name, element->guiID });
				}
			}
			else {
				warningLogger << LOGGER::BEGIN << "field \"name\" must be of type string!" << LOGGER::ENDL;
			}
			if (checkForUnusedParameters) parameterNames.erase("name");
		}
		// Move element to GuiManager
		moveElementPtr(std::move(element));
		if (checkForUnusedParameters) printWarningsForUnusedParameterNames(parameterNames);
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiManager::loadGuiAndRegisterAsChildren(const nlohmann::json& json, const nlohmann::json* data, GuiElement& parentElement)
	{
		if (json.is_object()) return loadGuiElementAndRegisterAsChild(json, data, parentElement);
		if (json.is_array()) {
			bool result = true;
			for (const nlohmann::json& child : json) {
				result &= loadGuiElementAndRegisterAsChild(child, data, parentElement);
			}
			return result;
		}
		else {
			warningLogger << LOGGER::BEGIN << "field \"children\" of a GuiElement must be either a GuiElement or an array of GuiElements." << LOGGER::ENDL;
			return false;
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElement> GuiManager::loadGuiElement(const nlohmann::json& json, const nlohmann::json* data)
	{
		if (checkForUnusedParameters) {
			std::set<std::string> parameterNames = extractParameterNames(json);
			std::unique_ptr<GuiElement> element = std::move(loadGuiElement(json, data, &parameterNames));
			printWarningsForUnusedParameterNames(parameterNames);
			return std::move(element);
		}
		else {
			return std::move(loadGuiElement(json, data, nullptr));
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::vector<std::unique_ptr<GuiElement>> GuiManager::loadGui(const nlohmann::json& json, const nlohmann::json* data)
	{
		if (json.is_object()) {
			std::unique_ptr<GuiElement> guiElement = std::move(loadGuiElement(json, data));
			if (guiElement == nullptr) return std::move(std::vector<std::unique_ptr<GuiElement>>());
			else {
				std::vector<std::unique_ptr<GuiElement>> result;
				result.emplace_back(std::move(guiElement));
				return result;
			}
		}
		else if (json.is_array()) {
			std::vector<std::unique_ptr<GuiElement>> result;
			for (const auto& element : json) {
				if (element.is_object()) {
					std::unique_ptr<GuiElement> guiElement = loadGuiElement(json, data);
					if (guiElement == nullptr) {
						warningLogger << LOGGER::BEGIN << "Failed to parse one of the GuiElements in the array." << LOGGER::ENDL;
					}
					else {
						result.emplace_back(std::move(guiElement));
					}
				}
				else {
					warningLogger << LOGGER::BEGIN << "Failed to parse one of the GuiElements in the array." << LOGGER::ENDL;
				}
			}
			return result;
		}
		else {
			warningLogger << LOGGER::BEGIN << "Failed to parse GUI: Json file was neither an object nor an array." << LOGGER::ENDL;
			return std::move(std::vector<std::unique_ptr<GuiElement>>());
		}
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiManager::loadAndRegisterJSON(const nlohmann::json& json, const nlohmann::json* data)
	{
		return loadAndRegisterAsChildJSON(json, data, parentElement);
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiManager::loadAndRegisterAsChildJSON(const nlohmann::json& json, const nlohmann::json* data, GuiID parent)
	{
		GuiElement* parentElement = getGuiElement(parent);
		if (!parentElement) {
			warningLogger << LOGGER::BEGIN << "Tried to register GUI as child of parentElement which does not exist." << LOGGER::ENDL;
			return false;
		}
		return loadGuiAndRegisterAsChildren(json, data, *parentElement);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::recomputeDefaultGuiElementValues(double scale)
	{
		GuiElement::defaultFontSizeSmall = scale * 15.0;
		GuiElement::defaultFontSizeNormal = scale * 20.0;
		GuiElement::defaultFontSizeBig = scale * 30.0;
		GuiElement::defaultFontSizeHuge = scale * 50.0;
		GuiElement::defaultBorderSmall = std::max(1, static_cast<int>(scale * 5));
		GuiElement::defaultBorderNormal = std::max(1, static_cast<int>(scale * 10));
		GuiElement::defaultBorderLarge = std::max(1, static_cast<int>(scale * 15));
		GuiElement::defaultBorderHuge = std::max(1, static_cast<int>(scale * 20));
		GuiCheckBox::defaultCheckBoxSize = std::max(1, static_cast<int>(scale * 50));
		GuiEditBox::defaultCursorWidth = static_cast<float>(scale * 0.1f);
		GuiVerticalScrollingListLayout::defaultScrollBarWidth = std::max(1, static_cast<int>(scale * 23));
		GuiSliderFloat::defaultSliderButtonWidth = std::max(1, static_cast<int>(scale * 20));
		GuiSliderDouble::defaultSliderButtonWidth = std::max(1, static_cast<int>(scale * 20));
		GuiSliderInt::defaultSliderButtonWidth = std::max(1, static_cast<int>(scale * 20));
		GuiSliderUnsignedInt::defaultSliderButtonWidth = std::max(1, static_cast<int>(scale * 20));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::removeElement(GuiID guiID)
	{
		signOff(guiID);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		processSignOffQueue();
		for (const auto& guiID : eventSetKeyboard) {
			auto element = getGuiElement(guiID);
			if (!element) continue;
			element->callbackKeyboard(key, scancode, action, mods);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		processSignOffQueue();
		// Go from the lastMouseHoverElement to the parent elements, until an element is found which 
		// is registered for mouseButtonOnElement
		GuiID guiID = lastMouseHoverElement;
		auto element = getGuiElement(guiID);
		if (element && eventSetMouseButtonOnElement.find(guiID) != eventSetMouseButtonOnElement.end()) {
			element->callbackMouseButtonOnElement(button, action, mods);
		}
		for (const auto& guiID : eventSetMouseButton) {
			auto element = getGuiElement(guiID);
			if (!element) continue;
			element->callbackMouseButton(button, action, mods);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackMouseMotion(const Vec2d& position)
	{
		processSignOffQueue();
		currentMousePosition = position;
		for (const auto& guiID : eventSetMouseMotion) {
			auto element = getGuiElement(guiID);
			if (!element) continue;
			element->callbackMouseMotion(position);
		}
		auto newMouseHoverElement = getCollidingElement();
		if (newMouseHoverElement != lastMouseHoverElement) {
			if (eventSetMouseLeave.find(lastMouseHoverElement) != eventSetMouseLeave.end()) {
				auto element = getGuiElement(lastMouseHoverElement);
				if (element) element->callbackMouseLeave(position);
			}
			if (eventSetMouseEnter.find(newMouseHoverElement) != eventSetMouseEnter.end()) {
				auto element = getGuiElement(newMouseHoverElement);
				if (element) element->callbackMouseEnter(position);
			}
		}
		lastMouseHoverElement = newMouseHoverElement;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackWindowResize(const Vec2i& screenDims)
	{
		computeViewAndProjection();
		if (ownedGuiElements[0]) ownedGuiElements[0]->setSize(screenDims);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackMouseScroll(const Vec2d& offset)
	{
		processSignOffQueue();
		auto result = getLowestCollidingElementInEventSet(eventSetMouseScrollOnElement);
		if (result) {
			auto element = getGuiElement(result.value());
			if (element) element->callbackMouseScrollOnElement(offset);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackCharacterInput(const unsigned int& codepoint)
	{
		processSignOffQueue();
		for (auto& guiID : eventSetCharacterInput) {
			auto element = getGuiElement(guiID);
			if (element) element->callbackCharacterInput(codepoint);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::update(const double& dt)
	{
		processSignOffQueue();
		for (auto& guiID : eventSetUpdate) {
			auto element = getGuiElement(guiID);
			if (element) element->update(dt);
		}
		updateAnimatables(dt);
		enforceLayouts();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::draw()
	{
		Renderer::disableDepthTesting();
		Renderer::enableBlending();
		registeredGuiElements[parentElement]->draw(Vec2i(0, 0));
		Renderer::disableScissorTest();
		// Draw all elements that registered for the DRAW_ON_TOP callback
		for (GuiID guiID : eventVectorDrawOnTop)
		{
			registeredGuiElements[guiID]->callbackDrawOnTop(getWorldOffset(guiID));
		}
		Renderer::enableDepthTesting();
	}
	//--------------------------------------------------------------------------------------------------
}