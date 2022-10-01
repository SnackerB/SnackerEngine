#pragma once

#include "Gui/GuiElement.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Core/Log.h"

#include <vector>
#include <memory>
#include <queue>
#include <unordered_set>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiManager
	{
	private:
		/// We just use unsigned ints as GuiIDs. The IDs are also indices into the guiElementPtrArray and the guiElementPtrOwnedArray!
		using GuiID = unsigned int;
		/// Vector of pointers that point to all guiElements. The GuiIDs are indices into this array. 
		/// If no element with this ID is known, nullptr is stored instead. 
		std::vector<GuiElement*> guiElementPtrArray;
		/// Vector of unique pointers to guiElements that are stored by this guiManager. The GuiIDs are
		/// indices into this array. If no guiElement of this ID is stored by the guiManager, nullptr is
		/// stored instead
		std::vector<GuiElement*> guiElementPtrOwnedArray;
		/// Queue of available (unused) GuiIDs
		std::queue<GuiID> availableGuiIDs;
		/// Current maximal amount of GuiElement objects that we can have before we have to resize
		GuiID maxGuiElements;
		/// Current count of GuiElements registered
		GuiID registeredGuiElementsCount;
		/// Current count of GuiElements that are owned by this guiManager
		GuiID ownedGuiElementsCount;
		/// View matrix of this guiManager
		Mat4f viewMatrix;
		/// Projection matrix of this guiManager
		Mat4f projectionMatrix;
		/// Vector of parent guiElements. The draw function of these elements is called in order
		std::vector<GuiID> parentGuiElements;
		/// The current mouse position
		Vec2i currentMousePosition;
		/// The guiID of the element the mouse was over last
		GuiID lastMouseHoverElement;
		/// Sets of GuiIDs for each event that can happen to a guiElement
		std::unordered_set<GuiID> eventSetMouseButton;
		std::unordered_set<GuiID> eventSetMouseMotion;
		std::unordered_set<GuiID> eventSetKeyboard;
		std::unordered_set<GuiID> eventSetCharacterInput;
		std::unordered_set<GuiID> eventSetMouseButtonOnElement;
		std::unordered_set<GuiID> eventSetMouseScrollOnElement;
		std::unordered_set<GuiID> eventSetMouseEnter;
		std::unordered_set<GuiID> eventSetMouseLeave;
		std::unordered_set<GuiID> eventSetUpdate;
		//==============================================================================================
		// Special Models, Textures etc. stored by the guiManager
		//==============================================================================================
		Model squareModel;
		Model triangleModel;
		unsigned DPI;
		//==============================================================================================
		// Helper functions
		//==============================================================================================
		/// Returns a new GuiID. The slot in the guiElementPtrArray and the guiElementPtrOwnedArray can 
		/// then be used for registering/storing a new GuiElement object
		GuiID getNewGuiID();
		/// Computes this guiManagers view and projection matrices
		void computeViewAndProjection();
		/// Helper function that signs off a guiElement without notifying the parent 
		/// (called ny guiManager when doing a cascading sign off starting at some element)
		void signOffWithoutNotifyingParent(GuiElement& guiElement);
		/// Helper function that clears all event queues of a given guiElement
		void clearEventQueues(GuiElement& guiElement);
		/// Computes the GuiID of the element that is colliding with the given position, or 0 if no element
		/// is found. Chooses the child farthest down the childElement tree!
		GuiID getCollidingElement(const Vec2i& position);
		/// Computes the current offset of the mouse to the top left corner of the element with the given ID
		Vec2f getMouseOffset(GuiID guiID);
		/// Updates the elementID and calls enforceLayout() of all layouts that a guiElement owns. 
		/// Should be called when a new guiElement is registered.
		void handleLayoutsOnGuiElementRegister(GuiElement& guiElement);
		/// Updates the parentIDs of all children. Should be called when a new guiElement is registered
		void handleChildrenOnGuiElementRegister(GuiElement& guiElement);
	protected:
		friend class GuiElement;
		friend class Layout;
		/// Called by GuiElement objects when they want to be signed off from this GuiManager
		void signOff(GuiElement& guiElement);
		/// Called by GuiElement objects when they are moved, to update the pointer in the GuiManager
		void updateMoved(GuiElement& guiElement);
		/// Called by GuiElement objects to draw their children
		void drawElements(const std::vector<GuiID>& guiIDs, const Vec2i& parentPosition);
		/// Returns a reference to the guiElement with a given guiID
		GuiElement& getElement(const GuiID& guiID);
		//==============================================================================================
		// Events that can happen to a guiElement
		//==============================================================================================
		/// This function can be called by guiElement to register themselves for getting notified when events happen
		void signUpEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType);
		/// This function can be called by guiElement to sign off getting notified when certain events happen
		void signOffEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType);
	public:
		/// Constructor
		GuiManager(const unsigned int& startingSize = 20);
		/// Registers the given element as parent element, but the element remains at its position!
		void registerElement(GuiElement& guiElement);
		/// Registers the given element as parent element and moves it to the GuiManager, which from this point on stores 
		/// the object. If this function is called with an already registered element, it is still moved to the guiManager!
		template<typename GuiElementType>
		void registerElement(GuiElementType&& guiElement);
		/// Registers the given element as child element of a parent element. child element remains at its position.
		/// The parent element has to be already registered!
		void registerElement(GuiElement& parentElement, GuiElement& childElement);
		/// Registers the given element as child element of a parent element, using the specified layout with the given options.
		/// Child element remains at its position.
		/// The parent element has to be already registered!
		template<typename LayoutType>
		void registerElement(GuiElement& parentElement, GuiElement& childElement, const typename LayoutType::LayoutReference& layoutReference, const typename LayoutType::LayoutOptions& layoutOptions);
		/// Registers the given element as child element of a parent element. child element is moved to guiManager.
		/// The parent element has to be already registered! childElement has to not be registered yet!
		template<typename GuiElementType>
		void registerElement(GuiElement& parentElement, GuiElementType&& childElement);
		/// Registers the given element as child element of a parent element, using the specified layout with the given options.
		/// Child element is moved to guiManager.
		/// The parent element has to be already registered! childElement has to not be registered yet!
		template<typename GuiElementType, typename LayoutType>
		void registerElement(GuiElement& parentElement, GuiElementType&& childElement, const typename LayoutType::LayoutReference& layoutReference, const typename LayoutType::LayoutOptions& layoutOptions);
		/// Adds a given Layout to the given guiElement and returns a LayoutReference object that can be used to add
		/// child elements to the layout
		template<typename LayoutType>
		LayoutType::LayoutReference registerLayout(GuiElement& guiElement, LayoutType&& layout);
		/// Returns the DPI (dots per inch) of this guiManager
		unsigned int getDPI();
		/// Deleted copy and move constructors and assignment operators
		GuiManager(GuiManager& other) = delete;
		GuiManager& operator=(GuiManager& other) = delete;
		GuiManager(GuiManager&& other) = delete;
		GuiManager& operator=(GuiManager&& other) = delete;
		//==============================================================================================
		// Callback functions
		//==============================================================================================
		// callback function for keyboard input. Parameters the same as in Scene.h
		void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods);
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		void callbackMouseButton(const int& button, const int& action, const int& mods);
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		void callbackMouseMotion(const Vec2d& position);
		/// Callback function for resizing the window. Parameter the same as in Scene.h
		void callbackWindowResize(const Vec2i& screenDims);
		/// Callback function for scrolling the mouse wheel. Parameter the same as in Scene.h
		void callbackMouseScroll(const Vec2d& offset);
		/// Callback function for the input of unicode characters. Parameter the same as in Scene.h
		void callbackCharacterInput(const unsigned int& codepoint);
		/// This function get calls regularly by the Engine class.
		void update(const double& dt);
		// Draws the GUI
		void draw();
		//==============================================================================================
		// Functions that should only be used by classes derived from GuiElement
		//==============================================================================================
		/// Sets the view and projection matrix uniform of the given shader
		void setUniformViewAndProjectionMatrices(const Shader& shader);
		/// Returns the view matrix
		const Mat4f& getViewMatrix() const;
		/// Returns the projection matrix
		const Mat4f& getProjectionMatrix() const;
		/// Returns a model of a square, with positions and texture coordinates
		Model getModelSquare();
		/// Returns a model of a triangle, with positions and texture coordinates
		Model getModelTriangle();
	};
	//--------------------------------------------------------------------------------------------------
	template<typename GuiElementType>
	inline void GuiManager::registerElement(GuiElementType&& guiElement)
	{
		GuiID originalGuiID = guiElement.guiID;
		GuiID guiID = originalGuiID;
		if (originalGuiID == 0) {
			// Get new GuiID
			guiID = getNewGuiID();
			registeredGuiElementsCount++;
		}
		ownedGuiElementsCount++;
		guiElementPtrOwnedArray[guiID] = new GuiElementType(std::move(guiElement));
		guiElementPtrOwnedArray[guiID]->guiID = guiID;
		guiElementPtrOwnedArray[guiID]->guiManager = this;
		if (originalGuiID == 0) {
			// We also have to put the element into the guiElementPtrArray, since updateMove is not called (guiManager of guiElement is nullptr)
			guiElementPtrArray[guiID] = guiElementPtrOwnedArray[guiID];
		}
		// Put into back of parent array
		parentGuiElements.push_back(guiID);
		// Update children
		handleChildrenOnGuiElementRegister(*guiElementPtrArray[guiID]);
		// Update layouts
		handleLayoutsOnGuiElementRegister(*guiElementPtrArray[guiID]);
		// Call onRegister()
		guiElementPtrArray[guiID]->onRegister();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename LayoutType>
	inline void GuiManager::registerElement(GuiElement& parentElement, GuiElement& childElement, const typename LayoutType::LayoutReference& layoutReference, const typename LayoutType::LayoutOptions& layoutOptions)
	{
		registerElement(parentElement, childElement);
		parentElement.addChild(layoutReference, childElement.guiID, layoutOptions);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename GuiElementType>
	inline void GuiManager::registerElement(GuiElement& parentElement, GuiElementType&& childElement)
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
		GuiID guiID = getNewGuiID();
		registeredGuiElementsCount++;
		ownedGuiElementsCount++;
		guiElementPtrOwnedArray[guiID] = new GuiElementType(std::move(childElement));
		guiElementPtrOwnedArray[guiID]->guiID = guiID;
		guiElementPtrOwnedArray[guiID]->guiManager = this;
		guiElementPtrArray[guiID] = guiElementPtrOwnedArray[guiID];
		// Insert as child into parent element (in the back of the children vector)
		parentElement.childrenIDs.push_back(guiID);
		guiElementPtrArray[guiID]->parentID = parentElement.guiID;
		// Call onRegister()
		guiElementPtrArray[guiID]->onRegister();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename GuiElementType, typename LayoutType>
	inline void GuiManager::registerElement(GuiElement& parentElement, GuiElementType&& childElement, const typename LayoutType::LayoutReference& layoutReference, const typename LayoutType::LayoutOptions& layoutOptions)
	{
		registerElement<GuiElementType>(parentElement, std::move(childElement));
		parentElement.addChild<LayoutType>(layoutReference, parentElement.childrenIDs.back(), layoutOptions);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename LayoutType>
	inline LayoutType::LayoutReference GuiManager::registerLayout(GuiElement& guiElement, LayoutType&& layout)
	{
		return guiElement.registerLayout<LayoutType>(std::move(layout));
	}
	//--------------------------------------------------------------------------------------------------
}