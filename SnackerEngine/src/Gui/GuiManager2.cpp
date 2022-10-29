#include "Gui/GuiManager2.h"
#include "Graphics/Renderer.h"
#include "Graphics/Meshes/Square.h"
#include "Graphics/Meshes/Triangle.h"

namespace SnackerEngine
{

	void GuiManager2::processSignOffQueue()
	{
		for (const auto& element : signOffQueue) {
			switch (element.second)
			{
			case GuiElement2::CallbackType::MOUSE_BUTTON: eventSetMouseButton.erase(element.first); break;
			case GuiElement2::CallbackType::MOUSE_MOTION: eventSetMouseMotion.erase(element.first); break;
			case GuiElement2::CallbackType::KEYBOARD: eventSetKeyboard.erase(element.first); break;
			case GuiElement2::CallbackType::CHARACTER_INPUT: eventSetCharacterInput.erase(element.first); break;
			case GuiElement2::CallbackType::MOUSE_BUTTON_ON_ELEMENT: eventSetMouseButtonOnElement.erase(element.first); break;
			case GuiElement2::CallbackType::MOUSE_SCROLL_ON_ELEMENT: eventSetMouseScrollOnElement.erase(element.first); break;
			case GuiElement2::CallbackType::MOUSE_ENTER: eventSetMouseEnter.erase(element.first); break;
			case GuiElement2::CallbackType::MOUSE_LEAVE: eventSetMouseLeave.erase(element.first); break;
			case GuiElement2::CallbackType::UPDATE: eventSetUpdate.erase(element.first); break;
			default:
				break;
			}
		}
		signOffQueue.clear();
	}

	void GuiManager2::signUpEvent(const GuiElement2& guiElement, const GuiElement2::CallbackType& callbackType)
	{
		if (guiElement.guiID <= 0) return;
		switch (callbackType)
		{
		case GuiElement2::CallbackType::MOUSE_BUTTON: eventSetMouseButton.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::MOUSE_MOTION: eventSetMouseMotion.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::KEYBOARD: eventSetKeyboard.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::CHARACTER_INPUT: eventSetCharacterInput.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::MOUSE_BUTTON_ON_ELEMENT: eventSetMouseButtonOnElement.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::MOUSE_SCROLL_ON_ELEMENT: eventSetMouseScrollOnElement.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::MOUSE_ENTER: eventSetMouseEnter.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::MOUSE_LEAVE: eventSetMouseLeave.insert(guiElement.guiID); break;
		case GuiElement2::CallbackType::UPDATE: eventSetUpdate.insert(guiElement.guiID); break;
		default:
			break;
		}
	}

	void GuiManager2::signOffEvent(const GuiElement2& guiElement, const GuiElement2::CallbackType& callbackType)
	{
		// push into signOffQueue. Event will be signed off the next time update() is called.
		signOffQueue.push_back(std::make_pair(guiElement.guiID, callbackType));
	}

	GuiManager2::GuiID GuiManager2::getCollidingElement()
	{
		return registeredGuiElements[parentElement]->getCollidingChild(currentMousePosition);
	}

	GuiManager2::GuiID GuiManager2::getNewGuiID()
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

	void GuiManager2::computeViewAndProjection()
	{
		const Vec2i& screenDims = Renderer::getScreenDimensions();
		viewMatrix = Mat4f::Identity();
		projectionMatrix = Mat4f::TranslateAndScale({ -1.0f, 1.0f, 0.0f }, { 2.0f / static_cast<float>(screenDims.x), 2.0f / static_cast<float>(screenDims.y), 0.0f });
		// TODO: For now this is only for GUI on screen. Later adapt this so that gui can be displayed anywhere!
	}

	void GuiManager2::clearEventQueues(const GuiID& guiID)
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

	void GuiManager2::signOff(const GuiID& guiElement)
	{
		if (guiElement <= 0) return;
		if (guiElement > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		GuiElement2& element = *registeredGuiElements[guiElement];
		/// Remove element from parent
		if (element.parentID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement with invalid parent element" << LOGGER::ENDL;
			return;
		}
		registeredGuiElements[element.parentID]->removeChild(element);
		signOffWithoutNotifyingParent(guiElement);
	}

	void GuiManager2::signOffWithoutNotifyingParent(const GuiID& guiElement)
	{
		if (guiElement <= 0) return;
		if (guiElement > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		GuiElement2& element = *registeredGuiElements[guiElement];
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

	void GuiManager2::updateMoved(GuiElement2& guiElement)
	{
		if (guiElement.guiID <= 0) return;
		if (guiElement.guiID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to update guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		registeredGuiElements[guiElement.guiID] = &guiElement;
	}

	GuiElement2& GuiManager2::getElement(const GuiID& guiID)
	{
		return *registeredGuiElements[guiID];
	}

	void GuiManager2::registerElementWithoutParent(GuiElement2& guiElement)
	{
		GuiID newGuiID = getNewGuiID();
		registeredGuiElements[newGuiID] = &guiElement;
		guiElement.guiID = newGuiID;
		guiElement.guiManager = this;
		guiElement.parentID = -1;
		guiElement.onRegister();
	}

	GuiManager2::GuiManager2(const unsigned int& startingSize)
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
	}

	void GuiManager2::registerElement(GuiElement2& guiElement)
	{
		GuiID newGuiID = getNewGuiID();
		registeredGuiElements[newGuiID] = &guiElement;
		guiElement.guiID = newGuiID;
		guiElement.guiManager = this;
		guiElement.parentID = 0;
		registeredGuiElements[parentElement]->addChild(guiElement);
		guiElement.onRegister();
	}

	void GuiManager2::registerElementAsChild(GuiElement2& parent, GuiElement2& child)
	{
		if (!parent.isValid()) {
			warningLogger << LOGGER::BEGIN << "Tried to set guiElement as child of an invalid guiElement!" << LOGGER::ENDL;
			return;
		}
		GuiID newGuiID = getNewGuiID();
		registeredGuiElements[newGuiID] = &child;
		child.guiID = newGuiID;
		child.guiManager = this;
		child.parentID = parent.guiID;
		parent.addChild(child);
		child.onRegister();
	}

	void GuiManager2::setUniformViewAndProjectionMatrices(const Shader& shader)
	{
		shader.setUniform<Mat4f>("u_view", viewMatrix);
		shader.setUniform<Mat4f>("u_projection", projectionMatrix);
	}

	const Mat4f& GuiManager2::getViewMatrix() const
	{
		return viewMatrix;
	}

	const Mat4f& GuiManager2::getProjectionMatrix() const
	{
		return projectionMatrix;
	}

	Model GuiManager2::getModelSquare()
	{
		if (squareModel.isValid())
			return squareModel;
		squareModel = Model(createMeshSquare());
		return squareModel;
	}

	Model GuiManager2::getModelTriangle()
	{
		if (triangleModel.isValid())
			return triangleModel;
		triangleModel = Model(createMeshTriangle());
		return triangleModel;
	}

	void GuiManager2::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		for (const auto& guiID: eventSetKeyboard) {
			getElement(guiID).callbackKeyboard(key, scancode, action, mods);
		}
	}

	void GuiManager2::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		// Go from the lastMouseHoverElement to the parent elements, until an element is found which 
		// is registered for mouseButtonOnElement
		GuiID guiID = lastMouseHoverElement;
		while (guiID != 0) {
			if (eventSetMouseButtonOnElement.find(lastMouseHoverElement) != eventSetMouseButtonOnElement.end()) {
				getElement(guiID).callbackMouseButtonOnElement(button, action, mods);
				break;
			}
			guiID = getElement(guiID).parentID;
		}
		for (const auto& guiID : eventSetMouseButton) {
			getElement(guiID).callbackMouseButton(button, action, mods);
		}
	}

	void GuiManager2::callbackMouseMotion(const Vec2d& position)
	{
		currentMousePosition = position;
		auto newMouseHoverElement = getCollidingElement();
		if (newMouseHoverElement != lastMouseHoverElement) {
			if (eventSetMouseEnter.find(newMouseHoverElement) != eventSetMouseEnter.end())
				getElement(newMouseHoverElement).callbackMouseEnter(position);
			if (eventSetMouseLeave.find(lastMouseHoverElement) != eventSetMouseLeave.end())
				getElement(lastMouseHoverElement).callbackMouseLeave(position);
		}
		for (const auto& guiID: eventSetMouseMotion) {
			getElement(guiID).callbackMouseMotion(position);
		}
		lastMouseHoverElement = newMouseHoverElement;
	}

	void GuiManager2::callbackWindowResize(const Vec2i& screenDims)
	{
		computeViewAndProjection();
	}

	void GuiManager2::callbackMouseScroll(const Vec2d& offset)
	{
		// Go from the lastMouseHoverElement to the parent elements, until an element is found which 
		// is registered for mouseScroll
		GuiID guiID = lastMouseHoverElement;
		while (guiID != 0) {
			if (eventSetMouseScrollOnElement.find(lastMouseHoverElement) != eventSetMouseScrollOnElement.end()) {
				getElement(guiID).callbackMouseScrollOnElement(offset);
				return;
			}
			guiID = getElement(guiID).parentID;
		}
	}

	void GuiManager2::callbackCharacterInput(const unsigned int& codepoint)
	{
		for (auto& guiID : eventSetCharacterInput) {
			getElement(guiID).callbackCharacterInput(codepoint);
		}
	}

	void GuiManager2::update(const double& dt)
	{
		processSignOffQueue();
		for (auto& guiID: eventSetUpdate) {
			getElement(guiID).update(dt);
		}
	}

	void GuiManager2::draw()
	{
		Renderer::disableDepthTesting();
		registeredGuiElements[parentElement]->draw(Vec2i(0, 0));
		Renderer::enableDepthTesting();
	}

}