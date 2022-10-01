#include "Gui/GuiManager.h"
#include "Graphics/Meshes/Square.h"
#include "Graphics/Meshes/Triangle.h"
#include "Graphics/Renderer.h"
#include "core/Engine.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	GuiManager::GuiID GuiManager::getNewGuiID()
	{
		if (registeredGuiElementsCount >= maxGuiElements)
		{
			// Resize vector and add new available GuiID slots accordingly. For now: double size everytime this happens and send warning!
			guiElementPtrArray.resize(maxGuiElements * 2 + 1, nullptr);
			guiElementPtrOwnedArray.resize(maxGuiElements * 2 + 1, nullptr);
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
		const Vec2i& screenDims = Renderer::getScreenDimensions();
		viewMatrix = Mat4f::Identity();
		projectionMatrix = Mat4f::TranslateAndScale({ -1.0f, 1.0f, 0.0f }, { 2.0f / static_cast<float>(screenDims.x), 2.0f / static_cast<float>(screenDims.y), 0.0f});
		// TODO: For now this is only for GUI on screen. Later adapt this so that gui can be displayed anywhere!
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signOffWithoutNotifyingParent(GuiElement& guiElement)
	{
		if (guiElement.guiID == 0) return;
		if (guiElement.guiID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		guiElementPtrArray[guiElement.guiID] = nullptr;
		registeredGuiElementsCount--;
		guiElement.parentID = 0;
		// Sign off children
		for (const auto& childID : guiElement.childrenIDs)
		{
			signOffWithoutNotifyingParent(*guiElementPtrArray[childID]);
		}
		guiElement.childrenIDs.clear();
		// Clear event queues
		clearEventQueues(guiElement);
		// Delete element if it is stored by guiManager
		if (guiElementPtrOwnedArray[guiElement.guiID]) {
			GuiID tempGuiID = guiElement.guiID;
			guiElementPtrOwnedArray[tempGuiID]->guiID = 0;
			guiElementPtrOwnedArray[tempGuiID] = nullptr;
			ownedGuiElementsCount--;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::clearEventQueues(GuiElement& guiElement)
	{
		eventSetMouseButton.erase(guiElement.guiID);
		eventSetMouseMotion.erase(guiElement.guiID);
		eventSetKeyboard.erase(guiElement.guiID);
		eventSetCharacterInput.erase(guiElement.guiID);
		eventSetMouseButtonOnElement.erase(guiElement.guiID);
		eventSetMouseScrollOnElement.erase(guiElement.guiID);
		eventSetMouseEnter.erase(guiElement.guiID);
		eventSetMouseLeave.erase(guiElement.guiID);
		eventSetUpdate.erase(guiElement.guiID);
	}
	//--------------------------------------------------------------------------------------------------
	GuiManager::GuiID GuiManager::getCollidingElement(const Vec2i& position)
	{
		// TODO: There could be a more efficient implementation for some special layouts/guiElements.
		// Maybe we can do this with recursive function calls?
		for (auto it1 = parentGuiElements.rbegin(); it1 != parentGuiElements.rend(); it1++) {
			GuiElement::IsCollidingResult result = guiElementPtrArray[*it1]->isColliding(position);
			if (result == GuiElement::IsCollidingResult::WEAK_COLLIDING) {
				GuiID currentGuiID = *it1;
				Vec2i currentPosition = position - guiElementPtrArray[currentGuiID]->position;
				while (true) {
					bool foundCollidingChild = false;
					for (auto it2 = guiElementPtrArray[currentGuiID]->childrenIDs.rbegin();
						it2 != guiElementPtrArray[currentGuiID]->childrenIDs.rend(); it2++) {
						result = guiElementPtrArray[*it2]->isColliding(currentPosition);
						if (result == GuiElement::IsCollidingResult::WEAK_COLLIDING) {
							foundCollidingChild = true;
							currentGuiID = *it2;
							currentPosition -= guiElementPtrArray[currentGuiID]->position;
							break;
						}
						else if (result == GuiElement::IsCollidingResult::STRONG_COLLIDING) {
							return *it2;
						}
					}
					if (!foundCollidingChild)
						return currentGuiID;
				}
			}
			else if (result == GuiElement::IsCollidingResult::STRONG_COLLIDING) {
				return *it1;
			}
		}

		return 0;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2f GuiManager::getMouseOffset(GuiID guiID)
	{
		Vec2f mouseOffset = currentMousePosition;
		while (guiID != 0) {
			mouseOffset -= guiElementPtrArray[guiID]->position;
			guiID = guiElementPtrArray[guiID]->parentID;
		}
		return mouseOffset;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::handleLayoutsOnGuiElementRegister(GuiElement& guiElement)
	{
		for (auto& layout : guiElement.layouts) {
			layout->elementID = guiElement.guiID;
			layout->guiManager = this;
			layout->enforceLayout();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::handleChildrenOnGuiElementRegister(GuiElement& guiElement)
	{
		for (auto& childID : guiElement.childrenIDs) {
			guiElementPtrArray[childID]->parentID = guiElement.guiID;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signOff(GuiElement& guiElement)
	{
		if (guiElement.guiID == 0) return;
		if (guiElement.guiID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign off guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		if (guiElement.parentID == 0) {
			// Remove element from parentElements
			auto result = std::find(parentGuiElements.begin(), parentGuiElements.end(), guiElement.guiID);
			if (result == parentGuiElements.end()) {
				warningLogger << LOGGER::BEGIN << "signed off element with no parent that was not in parentGuiElements!" << LOGGER::ENDL;
			}
			else {
				parentGuiElements.erase(result);
			}
		}
		else {
			guiElementPtrArray[guiElement.parentID]->removeChild(guiElement);
		}
		signOffWithoutNotifyingParent(guiElement);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::updateMoved(GuiElement& guiElement)
	{
		if (guiElement.guiID == 0) return;
		if (guiElement.guiID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to update guiElement that was not valid!" << LOGGER::ENDL;
			return;
		}
		guiElementPtrArray[guiElement.guiID] = &guiElement;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::drawElements(const std::vector<GuiID>& guiIDs, const Vec2i& parentPosition)
	{
		// We do not have to enable depth testing, since this function is only called 
		// while were in the guiManager draw call anyways!
		for (const GuiID& guiID : guiIDs)
		{
			guiElementPtrArray[guiID]->draw(parentPosition);
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement& GuiManager::getElement(const GuiID& guiID)
	{
		return *guiElementPtrArray[guiID];
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signUpEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType)
	{
		if (guiElement.guiID == 0) return;
		if (guiElement.guiID > maxGuiElements)
		{
			warningLogger << LOGGER::BEGIN << "Tried to sign up guiElement that was not valid to event!" << LOGGER::ENDL;
			return;
		}
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
	//--------------------------------------------------------------------------------------------------
	void GuiManager::signOffEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType)
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiManager::GuiManager(const unsigned int& startingSize)
		: guiElementPtrArray(startingSize + 1, nullptr), guiElementPtrOwnedArray(startingSize + 1, nullptr),
		availableGuiIDs{}, maxGuiElements(startingSize), registeredGuiElementsCount(0),
		ownedGuiElementsCount(0), viewMatrix{}, projectionMatrix{}, squareModel{}
	{
		// Initializes queue with all possible GuiIDs. GuiID = 0 is reserved for invalid guiElements.
		for (GuiID id = 1; id <= startingSize; ++id)
		{
			availableGuiIDs.push(id);
		}
		// Compute view and projection matrix
		computeViewAndProjection();
		// Compute DPI
		DPI = Engine::getDPI().y;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::registerElement(GuiElement& guiElement)
	{
		if (guiElement.guiID != 0) {
			warningLogger << LOGGER::BEGIN << "Tried to register already registered guiElement!" << LOGGER::ENDL;
			return;
		}
		/// Get new GuiID
		guiElement.guiID = getNewGuiID();
		guiElement.guiManager = this;
		registeredGuiElementsCount++;
		guiElementPtrArray[guiElement.guiID] = &guiElement;
		// Put into back of parent array
		parentGuiElements.push_back(guiElement.guiID);
		// Update children
		handleChildrenOnGuiElementRegister(*guiElementPtrArray[guiElement.guiID]);
		// Update layouts
		handleLayoutsOnGuiElementRegister(*guiElementPtrArray[guiElement.guiID]);
		// Call onRegister()
		guiElement.onRegister();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::registerElement(GuiElement& parentElement, GuiElement& childElement)
	{
		if (childElement.guiID != 0) {
			warningLogger << LOGGER::BEGIN << "Tried to register already registered guiElement!" << LOGGER::ENDL;
			return;
		}
		if (childElement.parentID != 0) {
			warningLogger << LOGGER::BEGIN << "Tried to set element as child that already has a parent!" << LOGGER::ENDL;
			return; // TODO: Maybe dont display warning but just change parent!
		}
		/// Get new GuiID
		childElement.guiID = getNewGuiID();
		childElement.guiManager = this;
		registeredGuiElementsCount++;
		guiElementPtrArray[childElement.guiID] = &childElement;
		// Insert as child into parent element (in the back of the children vector)
		parentElement.childrenIDs.push_back(childElement.guiID);
		guiElementPtrArray[childElement.guiID]->parentID = parentElement.guiID;
		// Call onRegister()
		childElement.onRegister();
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
	unsigned int GuiManager::getDPI()
	{
		return DPI;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		for (auto& guiID : eventSetKeyboard) {
			guiElementPtrArray[guiID]->callbackKeyboard(key, scancode, action, mods);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (eventSetMouseButtonOnElement.find(lastMouseHoverElement) != eventSetMouseButtonOnElement.end()) {
			guiElementPtrArray[lastMouseHoverElement]->callbackMouseButtonOnElement(button, action, mods);
		}
		for (auto& guiID : eventSetMouseButton) {
			guiElementPtrArray[guiID]->callbackMouseButton(button, action, mods);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackMouseMotion(const Vec2d& position)
	{
		currentMousePosition = position;
		GuiID newMouseHoverElement = getCollidingElement(position);
		if (newMouseHoverElement != lastMouseHoverElement) {
			if (eventSetMouseEnter.find(newMouseHoverElement) != eventSetMouseEnter.end()) {
				guiElementPtrArray[newMouseHoverElement]->callbackMouseEnter(position);
			}
			if (eventSetMouseLeave.find(lastMouseHoverElement) != eventSetMouseLeave.end()) {
				guiElementPtrArray[lastMouseHoverElement]->callbackMouseLeave(position);
			}
		}
		for (auto& guiID : eventSetMouseMotion) {
			guiElementPtrArray[guiID]->callbackMouseMotion(position);
		}
		lastMouseHoverElement = newMouseHoverElement;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackWindowResize(const Vec2i& screenDims)
	{
		computeViewAndProjection();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackMouseScroll(const Vec2d& offset)
	{
		if (eventSetMouseScrollOnElement.find(lastMouseHoverElement) != eventSetMouseScrollOnElement.end()) {
			guiElementPtrArray[lastMouseHoverElement]->callbackMouseScrollOnElement(offset);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::callbackCharacterInput(const unsigned int& codepoint)
	{
		for (auto& guiID : eventSetCharacterInput) {
			guiElementPtrArray[guiID]->callbackCharacterInput(codepoint);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::update(const double& dt)
	{
		for (auto& guiID : eventSetUpdate) {
			guiElementPtrArray[guiID]->update(dt);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiManager::draw()
	{
		// Draw all elements
		Renderer::disableDepthTesting();
		for (const GuiID& guiID : parentGuiElements) 
		{
			guiElementPtrArray[guiID]->draw({0, 0});
		}
		Renderer::enableDepthTesting();
	}
	//--------------------------------------------------------------------------------------------------
}