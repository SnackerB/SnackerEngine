#pragma once

#include "Gui/GuiElement.h"
#include "Math/Mat.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Gui/Animation/GuiElementAnimatable.h"

#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <optional>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiManager
	{
	private:
		friend class GuiElement;
		using GuiID = int;
		/// Vectors with pointers to all GuiElement objects. GuiIDs are indices into this vector. If no
		/// element with a given GuiID is known, nullptr is stored instead
		std::vector<GuiElement*> registeredGuiElements;
		/// Vectors with pointers to all GuiElements that are owned by the guiManager. The guiManager is
		/// responsible for deleting these elements!
		std::vector<std::unique_ptr<GuiElement>> ownedGuiElements;
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
		/// The guiID of the parentElement (in most cases this will be 0)
		GuiID parentElement;
		/// The current mouse position
		Vec2i currentMousePosition;
		/// The guiID of the element the mouse was over last
		GuiID lastMouseHoverElement;
		/// The current screen dimensions
		Vec2i screenDims;
		

		//==============================================================================================
		// Events
		//==============================================================================================

		/// Event sets for all types of events that a GuiElement object can register for
		std::unordered_set<GuiID> eventSetMouseButton;
		std::unordered_set<GuiID> eventSetMouseMotion;
		std::unordered_set<GuiID> eventSetKeyboard;
		std::unordered_set<GuiID> eventSetCharacterInput;
		std::unordered_set<GuiID> eventSetMouseButtonOnElement;
		std::unordered_set<GuiID> eventSetMouseScrollOnElement;
		std::unordered_set<GuiID> eventSetMouseEnter;
		std::unordered_set<GuiID> eventSetMouseLeave;
		std::unordered_set<GuiID> eventSetUpdate;
		std::vector<GuiID> eventVectorDrawOnTop;
		/// After an event happens, the signoff queue will be processed. This is necessary, as 
		/// modifying a set while it is iterated through can lead to crashes
		std::vector<std::pair<GuiID, GuiElement::CallbackType>> signOffQueue;
		/// processes the signoff queue by signing off elements from the event sets
		void processSignOffQueue();
	protected:
		/// This function can be called by GuiElement objects to register themselves for getting 
		/// notified when events happen
		void signUpEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType);
		/// This function can be called by GuiElement objects to sign off getting notified when 
		/// certain events happen
		void signOffEvent(const GuiElement& guiElement, const GuiElement::CallbackType& callbackType);
		/// Returns the GuiID of the element currently colliding with the mouse button, which will be 
		/// used for mouse enter/leave and mouse button events. 
		/// If zero is returned, this means that no element is colliding
		GuiID getCollidingElement();

		//==============================================================================================
		// Special Models, Textures etc. stored by the guiManager
		//==============================================================================================
		
	private:
		Model squareModel;
		Model triangleModel;

		//==============================================================================================
		// Rectangle clipping (scissor testing)
		//==============================================================================================
	protected:
		/// Clipping boxes are pushed onto this stack when a parent is rendered and removed
		/// when children finished rendering.
		std::vector<Vec4i> clippingBoxStack;
		/// Wether clipping is currently done. Is true by default
		bool doClipping;
		/// Adds a clipping box to the stack and enables the scissor test
		void pushClippingBox(const Vec4i& clippingBox);
		/// Pops the top clipping box from the stack and updates the scissor test
		void popClippingBox();

		//==============================================================================================
		// Enforcing Layouts
		//==============================================================================================
	private:
		/// Maps that store for each depth a set of GuiIDs that correspond to GuiElements that need their
		/// Layouts enforced. The maps are cleared when enforceLayouts() is called.
		std::map<unsigned int, std::set<GuiID>> enforceLayoutQueueUp;
		std::map<unsigned int, std::set<GuiID>> enforceLayoutQueueDown;
		/// Removes a guiElement from both LayoutQueues
		void removeFromEnforceLayoutQueues(const GuiID guiID);
	protected:
		/// This is called by GuiElements who need to enforce Layouts up and down the tree, 
		/// eg. when the size changes. Layouts are enforced whenever the GuiManager updates.
		void registerForEnforcingLayoutsUpAndDown(const GuiID& guiID);
		/// This is called by GuiElements who need to enforce Layouts up the tree,
		/// eg. when the position changes. Layouts are enforced whenever the GuiManager updates.
		void registerForEnforcingLayoutsUp(const GuiID& guiID);
		/// This is called by GuiElements who need to enforce Layouts down the tree,
		/// eg. when some internal parameters change, but the size/position is not changed.
		///  Layouts are enforced whenever the GuiManager updates.
		void registerForEnforcingLayoutsDown(const GuiID& guiID);
	public:
		/// Enforces all Layouts that are registered to be needed to enforce. This is normally
		/// called by the GuiManager at the end of each updat(), but can also be called manually
		/// after adding a bunch of elements. Layouts are enforced first from the bottom up, and
		/// then from the top down in two successive sweeps.
		void enforceLayouts();
		
		//==============================================================================================
		// Animations
		//==============================================================================================
	private:
		/// Map that stores all active animations, ordered by which GuiElement they belong to.
		std::unordered_map<GuiID, std::vector<std::unique_ptr<GuiElementAnimatable>>> animations;
		/// Deletes all animations of the given element from the animations map. Is called when a
		/// guiElement is signed off
		void deleteAnimations(const GuiID& guiID);
		/// animates all currently registered animations and deletes finished animations
		void animate(const double& dt);
	public:
		/// Registers an animation
		template<typename GuiElementAnimatableType>
		void registerAnimation(GuiElementAnimatableType&& animatable);

		//==============================================================================================
		// Helper functions
		//==============================================================================================
		
	private:
		/// Returns a new GuiID. The slot in the registeredGuiElements and the ownedGuiElements vectors
		/// can then be used for registering/storing a new GuiElement object
		GuiID getNewGuiID();
		/// Computes this guiManagers view and projection matrices
		void computeViewAndProjection();
		/// Helper function that clears all event queues of a given GuiElement object
		void clearEventQueues(const GuiID& guiID);
	protected:
		/// Called by GuiElement objects when they want to be signed off from this GuiManager
		void signOff(const GuiID& guiElement);
		/// Helper function that signs off a guiElement without notifying the parent 
		/// (called ny guiManager when doing a cascading sign off starting at some element)
		void signOffWithoutNotifyingParent(const GuiID guiElement);
		/// Called by GuiElement objects when they are moved, to update the pointer in the GuiManager
		void updateMoved(GuiElement& guiElement);
		/// Returns a reference to the guiElement with a given guiID
		GuiElement& getElement(const GuiID& guiID);
		/// Registers the given element as a child of a different element
		/// Returns true on success
		bool registerElementAsChild(GuiElement& parent, GuiElement& child);
		/// returns the current mouse offset to a given GuiElement
		Vec2i getMouseOffset(GuiID guiID);
		/// Returns the vector from the parentGuiElement with guiID == 0 to the given guiElement
		Vec2i getWorldOffset(GuiID guiID);
		/// Returns the lowest currently colliding element in a given event set. In this context
		/// "colliding" means != IsCollidingResult::NOT_COLLIDING.
		std::optional<GuiID> getLowestCollidingElementInEventSet(const std::unordered_set<GuiID>& eventSet);
		/// Helper function for getLowestCollidingElementInEventSet() function. Returns the lowest colliding
		/// child in a given event set, if it exists
		/// offset: offset from the top left corner of the parent element
		std::optional<GuiID> getLowestCollidingChildInEventSet(const GuiID& parentID, const Vec2i& offset, const std::unordered_set<GuiID>& eventSet);

		//==============================================================================================
		// Constructors and public functionality
		//==============================================================================================

	public:
		/// Constructor
		GuiManager(const unsigned int& startingSize = 20);
		/// Destructor
		~GuiManager();
		/// Deleted copy constructor and operator
		GuiManager(const GuiManager& other) = delete;
		GuiManager& operator=(const GuiManager& other) = delete;

		/// Registers the given element as a parent element
		void registerElement(GuiElement& guiElement);
		/// Registers the given element as a parent element and moves it to the guiManager.
		template<typename GuiElementType>
		void registerAndMoveElement(GuiElementType&& guiElement);
		/// Moves the given element to the guiManager. Works only if the element was already registered,
		/// either using the guiManager or a parent element registered at the guiManager.
		template<typename GuiElementType>
		void moveElement(GuiElementType&& guiElement);
		/// Clears all elements. Elements that are owned by the GuiManager are deleted!
		void clear();
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
		/// Takes a size and a guiElement and clips the size according to minSize and maxSize of the guiElement
		Vec2i clipSizeToMinMaxSize(const Vec2i& size, const GuiID& guiElement);
		int clipWidthToMinMaxWidth(const int& width, const GuiID& guiElement);
		int clipHeightToMinMaxHeight(const int& height, const GuiID& guiElement);
		/// Disables/Enables clipping boxes. Should only be used for debugging GUI
		void disableClippingBoxes();
		void enableClippingBoxes();

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
	};

	template<typename GuiElementAnimatableType>
	inline void GuiManager::registerAnimation(GuiElementAnimatableType&& animatable)
	{
		auto it = animations.find(animatable.getGuiElement()->getGuiID());
		auto ptr = std::make_unique<GuiElementAnimatableType>(std::move(animatable));
		if (it != animations.end()) {
			it->second.push_back(std::move(ptr));
		}
		else {
			std::vector<std::unique_ptr<GuiElementAnimatable>> vector;
			vector.push_back(std::move(ptr));
			animations.insert(std::make_pair(animatable.getGuiElement()->getGuiID(), std::move(vector)));
		}
	}

	template<typename GuiElementType>
	inline void GuiManager::registerAndMoveElement(GuiElementType&& guiElement)
	{
		if (guiElement.isValid()) {
			if (guiElement.guiManager == this) {
				GuiID guiID = guiElement.guiID;
				ownedGuiElements[guiID] = std::make_unique<GuiElementType>(std::move(guiElement));
				ownedGuiElementsCount++;
				registeredGuiElements[guiID] = ownedGuiElements[guiID].get();
				auto& element = *ownedGuiElements[guiID];
				if (element.parentID < 0) {
					element.parentID = 0;
					registeredGuiElements[parentElement]->children.push_back(element.guiID);
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
		ownedGuiElements[newGuiID] = std::make_unique<GuiElementType>(std::move(guiElement));
		ownedGuiElementsCount++;
		registeredGuiElements[newGuiID] = ownedGuiElements[newGuiID].get();
		registeredGuiElementsCount++;
		auto& element = *ownedGuiElements[newGuiID];
		element.guiID = newGuiID;
		element.guiManager = this;
		element.parentID = 0;
		element.depth = 1;
		registeredGuiElements[parentElement]->children.push_back(element.guiID);
		element.onRegister();
		registerForEnforcingLayoutsUpAndDown(element.guiID);
		// Callback mouseMotion, because we could collide with the new element!
		callbackMouseMotion(currentMousePosition);
	}

	template<typename GuiElementType>
	inline void GuiManager::moveElement(GuiElementType&& guiElement)
	{
		if (!guiElement.isValid()) {
			warningLogger << LOGGER::BEGIN << "Tried to move an invalid guiElement to a guiManager. Try to register the element first!" << LOGGER::ENDL;
			return;
		}
		GuiID guiID = guiElement.guiID;
		std::unique_ptr<GuiElementType> newPtr = std::make_unique<GuiElementType>(std::move(guiElement));
		ownedGuiElements[guiID] = std::move(newPtr);
		registeredGuiElements[guiID] = ownedGuiElements[guiID].get();
		ownedGuiElementsCount++;
	}

}