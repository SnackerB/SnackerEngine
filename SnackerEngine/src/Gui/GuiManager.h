#pragma once

#include "Gui/GuiElement.h"
#include "Gui/GuiLayout.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Core/Log.h"
#include "Math/Utility.h"

#include <vector>
#include <memory>
#include <queue>
#include <unordered_set>
#include <span>

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
		std::pair<GuiID, GuiID> lastMouseHoverElement;
		/// Sets of std::pairs of GuiIDs for each event that can happen to a guiElement or guiLayout.
		/// In a pair, the first entry is the guiID of the element or layout. The second entry is zero
		/// if the object is a guiElement, and is the guiID of the parent element if the object
		/// is a guiLayout.
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetMouseButton;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetMouseMotion;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetKeyboard;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetCharacterInput;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetMouseButtonOnElement;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetMouseScrollOnElement;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetMouseEnter;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetMouseLeave;
		std::unordered_set<std::pair<GuiID, GuiID>, Pairhash> eventSetUpdate;
		/// The pairs in the sign off queue are processed after an event happens. If the sets are changed
		/// while they are iterated errors can occur!
		std::vector<std::pair<std::pair<GuiID, GuiID>, GuiInteractable::CallbackType>> signOffQueue;
		/// empties the signoff queue and signs off the elements
		void processSignOffQueue();
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
		void clearEventQueues(GuiInteractable& guiInteractable);
		/// Checks which guiElement or GuiLayout is colliding with the given position. Returns a pair of
		/// GuiIDs. The first GuiID is the guiID of the guiElement or guiLayout. The second guiID is zero
		/// in case of a guiElement and holds the parentID in case of a layout.
		/// If no colliding element/layout is found, {0, 0} is returned.
		std::pair<GuiID, GuiID> getCollidingInteractable(const Vec2i& position);
		/// Computes the current offset of the mouse to the top left corner of the element with the given ID
		Vec2f getMouseOffset(GuiID guiID);
		/// Updates the elementID and calls enforceLayout() of all layouts that a guiElement owns. 
		/// Should be called when a new guiElement is registered.
		void handleLayoutsOnGuiElementRegister(GuiElement& guiElement);
		/// Returns a reference to the GuiInteractable object that is referenced by the pair of GuiIDs
		GuiInteractable& getGuiInteractable(std::pair<GuiID, GuiID> identifier);
	protected:
		friend class GuiInteractable;
		friend class GuiElement;
		friend class GuiLayout;
		/// Called by GuiInteractable objects when they want to be signed off from this GuiManager
		void signOff(GuiElement& guiElement);
		/// Called by GuiInteractable objects when they are moved, to update the pointer in the GuiManager
		void updateMoved(GuiElement& guiElement);
		/// Returns a reference to the guiElement with a given guiID
		GuiElement& getElement(const GuiID& guiID);
		/// Looksup the parent GuiInteractable object (layout or element) of the given GuiInteractable
		std::pair<GuiID, GuiID> getParentInteractable(std::pair<GuiID, GuiID> interactable);
		//==============================================================================================
		// Events that can happen to a guiINteractable object
		//==============================================================================================
		/// This function can be called by guiINteractable to register themselves for getting notified when events happen
		void signUpEvent(const GuiInteractable& guiInteractable, const GuiElement::CallbackType& callbackType);
		/// This function can be called by guiINteractable to sign off getting notified when certain events happen
		void signOffEvent(const GuiInteractable& guiInteractable, const GuiElement::CallbackType& callbackType);
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
		// Update layouts and children
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
		// Insert as child into parent element
		parentElement.addChild(guiID);
		// Call onRegister()
		guiElementPtrArray[guiID]->onRegister();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename GuiElementType, typename LayoutType>
	inline void GuiManager::registerElement(GuiElement& parentElement, GuiElementType&& childElement, const typename LayoutType::LayoutReference& layoutReference, const typename LayoutType::LayoutOptions& layoutOptions)
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
		// Insert as child into parent element
		parentElement.addChild<LayoutType>(layoutReference, guiID, layoutOptions);
		// Call onRegister()
		guiElementPtrArray[guiID]->onRegister();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename LayoutType>
	inline LayoutType::LayoutReference GuiManager::registerLayout(GuiElement& guiElement, LayoutType&& layout)
	{
		return guiElement.registerLayout<LayoutType>(std::move(layout));
	}
	//--------------------------------------------------------------------------------------------------
}