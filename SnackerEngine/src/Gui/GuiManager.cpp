#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Graphics/Meshes/Square.h"
#include "Graphics/Meshes/Triangle.h"

namespace SnackerEngine
{

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
			default:
				break;
			}
		}
		signOffQueue.clear();
	}

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
		default:
			break;
		}
	}

	void GuiManager::signOffEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType)
	{
		// push into signOffQueue. Event will be signed off the next time update() is called.
		signOffQueue.push_back(std::make_pair(guiElement.guiID, callbackType));
	}

	GuiManager::GuiID GuiManager::getCollidingElement()
	{
		return registeredGuiElements[parentElement]->getCollidingChild(currentMousePosition);
	}

	GuiManager::GuiID GuiManager::getNewGuiID()
	{
		if (registeredGuiElementsCount >= maxGuiElements)
		{
			// Resize vector and add new available GuiID slots accordingly. For now: double size everytime this happens and send warning!
			registeredGuiElements.resize(maxGuiElements * 2 + 1, nullptr);
			ownedGuiElements.resize(maxGuiElements * 2 + 1, nullptr);
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

	void GuiManager::computeViewAndProjection()
	{
		const Vec2i& screenDims = Renderer::getScreenDimensions();
		viewMatrix = Mat4f::Identity();
		projectionMatrix = Mat4f::TranslateAndScale({ -1.0f, 1.0f, 0.0f }, { 2.0f / static_cast<float>(screenDims.x), 2.0f / static_cast<float>(screenDims.y), 0.0f });
		// TODO: For now this is only for GUI on screen. Later adapt this so that gui can be displayed anywhere!
	}

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
	}

	void GuiManager::signOff(const GuiID& guiElement)
	{
		if (guiElement <= 0) return;
		if (guiElement > maxGuiElements)
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
		if (registeredGuiElements[element.parentID]) registeredGuiElements[element.parentID]->removeChild(element);
	}

	void GuiManager::signOffWithoutNotifyingParent(const GuiID& guiElement)
	{
		if (guiElement <= 0) return;
		if (guiElement > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		GuiElement& element = *registeredGuiElements[guiElement];
		// Sign off element which will in turn sign off its children
		element.signOff();
		registeredGuiElements[guiElement] = nullptr;
		registeredGuiElementsCount--;
		// Clear event queues
		clearEventQueues(guiElement);
		// Delete element if it is stored by guiManager
		if (ownedGuiElements[guiElement]) {
			ownedGuiElements[guiElement] = nullptr;
			ownedGuiElementsCount--;
		}
	}

	void GuiManager::updateMoved(GuiElement& guiElement)
	{
		if (guiElement.guiID <= 0) return;
		if (guiElement.guiID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to update guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		registeredGuiElements[guiElement.guiID] = &guiElement;
	}

	GuiElement& GuiManager::getElement(const GuiID& guiID)
	{
		if (guiID < 0 || guiID >= registeredGuiElements.size() || !registeredGuiElements[guiID]) {
			errorLogger << LOGGER::BEGIN << "Tried to access invalid guiElement with guiID " << guiID << LOGGER::ENDL;
		}
		return *registeredGuiElements[guiID];
	}

	GuiManager::GuiManager(const unsigned int& startingSize)
		: registeredGuiElements(startingSize + 1, nullptr), ownedGuiElements(startingSize + 1, nullptr),
		availableGuiIDs{}, maxGuiElements(startingSize), registeredGuiElementsCount(0),
		ownedGuiElementsCount(0), viewMatrix{}, projectionMatrix{}, parentElement(0), currentMousePosition{},
		lastMouseHoverElement(0), eventSetMouseButton{}, eventSetMouseMotion{}, eventSetKeyboard{},
		eventSetCharacterInput{}, eventSetMouseButtonOnElement{}, eventSetMouseScrollOnElement{},
		eventSetMouseEnter{}, eventSetMouseLeave{}, eventSetUpdate{}, signOffQueue{}, squareModel{},
		triangleModel{}
	{
		// Initializes queue with all possible GuiIDs. GuiID = 0 is reserved for invalid guiElements.
		for (GuiID id = 1; id <= startingSize; ++id)
		{
			availableGuiIDs.push(id);
		}
		// Compute view and projection matrix
		computeViewAndProjection();
		// Initialize parent GuiElement
		ownedGuiElements[parentElement] = new GuiElement(Vec2i(0, 0), Renderer::getScreenDimensions(), GuiElement::ResizeMode::DO_NOT_RESIZE);
		ownedGuiElements[parentElement]->guiID = 0;
		ownedGuiElements[parentElement]->parentID = -1;
		ownedGuiElements[parentElement]->guiManager = this;
		registeredGuiElements[parentElement] = ownedGuiElements[parentElement];
	}

	GuiManager::~GuiManager()
	{
		for (unsigned int i = 0; i < ownedGuiElements.size(); ++i) {
			if (ownedGuiElements[i] != nullptr) {
				signOff(ownedGuiElements[i]->guiID);
			}
		}
		for (unsigned int i = 0; i < registeredGuiElements.size(); ++i) {
			if (registeredGuiElements[i] != nullptr) {
				signOff(registeredGuiElements[i]->guiID);
			}
		}
		signOff(parentElement);
		ownedGuiElements.clear();
	}

	void GuiManager::registerElement(GuiElement& guiElement)
	{
		GuiID newGuiID = getNewGuiID();
		registeredGuiElements[newGuiID] = &guiElement;
		registeredGuiElementsCount++;
		guiElement.guiID = newGuiID;
		guiElement.guiManager = this;
		guiElement.parentID = 0;
		registeredGuiElements[parentElement]->children.push_back(guiElement.guiID);
		guiElement.onRegister();
	}

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
		child.onRegister();
		return true;
	}

	Vec2i GuiManager::getMouseOffset(GuiID guiID)
	{
		Vec2f mouseOffset = currentMousePosition;
		while (guiID >= 0) {
			mouseOffset -= registeredGuiElements[guiID]->position;
			guiID = registeredGuiElements[guiID]->parentID;
		}
		return mouseOffset;
	}

	std::optional<GuiManager::GuiID> GuiManager::getLowestCollidingElementInEventSet(const std::unordered_set<GuiID>& eventSet)
	{
		if (eventSet.empty()) return {};
		return getLowestCollidingChildInEventSet(parentElement, currentMousePosition, eventSet);
	}

	std::optional<GuiManager::GuiID> GuiManager::getLowestCollidingChildInEventSet(const GuiID& parentID, const Vec2i& offset, const std::unordered_set<GuiID>& eventSet)
	{
		GuiElement& parent = getElement(parentID);
		for (const auto& childID : parent.children) {
			GuiElement& child = getElement(childID);
			switch (child.isColliding(offset)) {
			case GuiElement::IsCollidingResult::COLLIDE_CHILD:
			case GuiElement::IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT:
			{
				auto result = getLowestCollidingChildInEventSet(childID, offset - child.getPosition(), eventSet);
				if (result) return result;
				if (eventSet.find(childID) != eventSet.end()) return { childID };
				break;
			}
			case GuiElement::IsCollidingResult::COLLIDE_STRONG:
			{
				if (eventSet.find(childID) != eventSet.end()) return { childID };
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

	void GuiManager::setUniformViewAndProjectionMatrices(const Shader& shader)
	{
		shader.setUniform<Mat4f>("u_view", viewMatrix);
		shader.setUniform<Mat4f>("u_projection", projectionMatrix);
	}

	const Mat4f& GuiManager::getViewMatrix() const
	{
		return viewMatrix;
	}

	const Mat4f& GuiManager::getProjectionMatrix() const
	{
		return projectionMatrix;
	}

	Model GuiManager::getModelSquare()
	{
		if (squareModel.isValid())
			return squareModel;
		squareModel = Model(createMeshSquare());
		return squareModel;
	}

	Model GuiManager::getModelTriangle()
	{
		if (triangleModel.isValid())
			return triangleModel;
		triangleModel = Model(createMeshTriangle());
		return triangleModel;
	}

	void GuiManager::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		processSignOffQueue();
		for (const auto& guiID: eventSetKeyboard) {
			getElement(guiID).callbackKeyboard(key, scancode, action, mods);
		}
	}

	void GuiManager::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		processSignOffQueue();
		// Go from the lastMouseHoverElement to the parent elements, until an element is found which 
		// is registered for mouseButtonOnElement
		GuiID guiID = lastMouseHoverElement;
		while (guiID != 0) {
			if (eventSetMouseButtonOnElement.find(guiID) != eventSetMouseButtonOnElement.end()) {
				getElement(guiID).callbackMouseButtonOnElement(button, action, mods);
				break;
			}
			guiID = getElement(guiID).parentID;
		}
		for (const auto& guiID : eventSetMouseButton) {
			getElement(guiID).callbackMouseButton(button, action, mods);
		}
	}

	void GuiManager::callbackMouseMotion(const Vec2d& position)
	{
		processSignOffQueue();
		currentMousePosition = position;
		for (const auto& guiID : eventSetMouseMotion) {
			getElement(guiID).callbackMouseMotion(position);
		}
		auto newMouseHoverElement = getCollidingElement();
		if (newMouseHoverElement != lastMouseHoverElement) {
			if (eventSetMouseLeave.find(lastMouseHoverElement) != eventSetMouseLeave.end())
				getElement(lastMouseHoverElement).callbackMouseLeave(position);
			if (eventSetMouseEnter.find(newMouseHoverElement) != eventSetMouseEnter.end())
				getElement(newMouseHoverElement).callbackMouseEnter(position);
		}
		lastMouseHoverElement = newMouseHoverElement;
	}

	void GuiManager::callbackWindowResize(const Vec2i& screenDims)
	{
		computeViewAndProjection();
	}

	void GuiManager::callbackMouseScroll(const Vec2d& offset)
	{
		processSignOffQueue();
		auto result = getLowestCollidingElementInEventSet(eventSetMouseScrollOnElement);
		if (result) {
			getElement(result.value()).callbackMouseScrollOnElement(offset);
		}
	}

	void GuiManager::callbackCharacterInput(const unsigned int& codepoint)
	{
		processSignOffQueue();
		for (auto& guiID : eventSetCharacterInput) {
			getElement(guiID).callbackCharacterInput(codepoint);
		}
	}

	void GuiManager::update(const double& dt)
	{
		processSignOffQueue();
		for (auto& guiID: eventSetUpdate) {
			getElement(guiID).update(dt);
		}
	}

	void GuiManager::draw()
	{
		Renderer::disableDepthTesting();
		registeredGuiElements[parentElement]->draw(Vec2i(0, 0));
		Renderer::enableDepthTesting();
	}

}