#pragma once

#include "Math/Vec.h"
#include "Gui/GuiEventHandles/GuiHandle.h"
#include "Gui/GuiEventHandles/GuiVariableHandle.h"
#include "Core/Log.h"

#include <vector>
#include <unordered_map>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declarations
	class GuiManager;
	class GuiHandle;
	class GuiEventHandle;
	class GuiLayout;
	//--------------------------------------------------------------------------------------------------
	/// Base class for guiElement and guiLayout
	class GuiInteractable
	{
	private:
		friend class GuiManager;
		friend class GuiElement;
		using GuiID = unsigned int;
	protected:
		/// Pointer to the parent guiManager
		GuiManager* guiManager;
		/// guiID of this guiInteractable object. For guiElements, this is an index into the guiElements vector
		/// of the guiManager. For guiLayouts, this can be used to access the layout from the parent guiElement.
		GuiID guiID;
		/// guiID of the parent guiElement. If this is zero, this GuiInteractable object does not have a parent.
		GuiID parentID;
		/// enum that can be used to distinguish guiElements and guiLayouts
		enum GuiInteractableType
		{
			GUI_ELEMENT,
			GUI_LAYOUT,
		};
		GuiInteractableType type;
		/// tells this guiInteractable object that the guiManager was deleted
		virtual void signOff();
		/// Draws this guiInteractable object relative to its parent element
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) {};
		/// Update function
		virtual void update(const float dt) {};
		/// This function is called by the guiManager after registering this guiInteractable object.
		/// When this function is called, the guiManager pointer is set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() {};

		//==============================================================================================
		// Collisions
		//==============================================================================================

		/// Enum that contains the possible return types of isColliding
		enum class IsCollidingResult
		{
			NOT_COLLIDING,		/// No collision detected
			WEAK_COLLIDING,		/// Collision detected, but if a childElement is also colliding, choose this one instead
			STRONG_COLLIDING,	/// Collision detected, no child elements should be checked!
		};
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position);

		//==============================================================================================
		// Events that can happen to a guiInteractable
		//==============================================================================================

		/// Enum for different types of update/callback events a guiInteractable object can register itself for
		enum class CallbackType {
			MOUSE_BUTTON,
			MOUSE_MOTION,
			KEYBOARD,
			CHARACTER_INPUT,
			MOUSE_BUTTON_ON_ELEMENT,
			MOUSE_SCROLL_ON_ELEMENT,
			MOUSE_ENTER,
			MOUSE_LEAVE,
			UPDATE,
		};
		/// This function can be called by derived guiElements or guiLayouts to register themselves for getting notified when events happen
		void signUpEvent(const CallbackType& callbackType);
		/// This function can be called by derived guiElements or guiLayouts to sign off getting notified when certain events happen
		void signOffEvent(const CallbackType& callbackType);
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) {};
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) {};
		// callback function for keyboard input. Parameters the same as in Scene.h
		virtual void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) {};
		/// Callback function for the input of unicode characters. Parameter the same as in Scene.h
		virtual void callbackCharacterInput(const unsigned int& codepoint) {};
		/// Callback function for mouse button input on this guiInteractable object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) {};
		/// Callback function for scrolling the mouse wheel. Parameter the same as in Scene.h
		virtual void callbackMouseScrollOnElement(const Vec2d& offset) {};
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) {};
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) {};
		/// Update function
		virtual void update(const double& dt) {};
		/// Protected constructor
		GuiInteractable(const GuiInteractableType& type);
	public:
		/// Destructor
		~GuiInteractable();
		/// Copy constructor and assignment operator
		GuiInteractable(const GuiInteractable& other) noexcept;
		GuiInteractable& operator=(const GuiInteractable& other) noexcept;
		/// Move constructor and assignment operator
		GuiInteractable(GuiInteractable&& other) noexcept;
		GuiInteractable& operator=(GuiInteractable&& other) noexcept;
		/// Returns true if this GuiInteractable object is managed by a guiManager
		bool isValid();
	};
	/// Base class for all GuiElements (Buttons, Sliders, Text, Windows, ...)
	//--------------------------------------------------------------------------------------------------
	class GuiElement : public GuiInteractable
	{
	private:
		using GuiID = unsigned int;
		friend class GuiManager;
		friend class GuiHandle;
		friend class GuiLayout;
	protected:
		/// GuiID of the layout that this guiElement is saved in.
		GuiID parentLayoutID;
		/// GuiID of the next layout that is added to this element
		GuiID nextLayoutID;
		/// Position of this guiElement relative to its parent guiElement. The position vector points to the upper left corner
		/// of the bounding rectangle.
		Vec2i position;
		/// Dimensions of the bounding rectangle in pixels. The origin is at the upper left corner
		Vec2i size;
		/// Preferred sizes are just hints to the layout and not a guarantee.
		/// A preferred size of zero means that the element does not has size preferences at all.
		Vec2i preferredSize;
		Vec2i preferredMinSize;
		Vec2i preferredMaxSize;
		/// Vector of pointers to the GuiLayouts that handle this guiElements childrens position and size
		std::vector<std::unique_ptr<GuiLayout>> layouts;
		/// Map that can be used to get an index into the layout vector
		std::unordered_map<GuiID, std::size_t> layoutIDToIndex;
		/// tells this guiElement that the guiManager was deleted
		void signOff() override;
		/// Calls the draw function on all layouts.
		/// parentPosition:		position of the upper left corner of the parent element
		void drawChildren(const Vec2i& parentPosition);
		/// This function gets called when the position changes. Not called by the constructor!
		virtual void onPositionChange() {};
		/// This function gets called when the size changes. Not called by the constructor!
		virtual void onSizeChange() {};
		/// This function is called by the guiManager after registering this guiElement.
		/// When this function is called, the guiManager pointer, the guiID, and the parent element id are set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override {};
		/// Removes the given child from this guiElement
		void removeChild(GuiElement& guiElement);
		/// Returns the offset of the mouse to the current element
		Vec2f getMouseOffset();
		/// Returns the offset of the mouse to the parent of the current element
		Vec2f getParentMouseOffset();

		//==============================================================================================
		// GuiHandles
		//==============================================================================================

		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) {};
		/// This function should be called by the guiElement to sign up a new handle
		void signUpHandle(GuiHandle& guiHandle, const GuiHandle::GuiHandleID& handleID);
		/// This function should be called to sign off a given eventHandle if it is no longer needed
		/// (e.g. destruction of a guiElement) 
		void signOffHandle(GuiHandle& guiHandle);
		/// This function should be called when moving a guiElement that owns a guiHandle
		void notifyHandleOnGuiElementMove(GuiHandle& guiHandle);
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) {};
		/// This function calls activate() on the given GuiEventHandle
		void activate(GuiEventHandle& guiEventHandle);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle) {};
		/// template function used to change a value of a variable handle
		template<typename T>
		void setVariableHandleValue(GuiVariableHandle<T>& variableHandle, const T& value);

		//==============================================================================================
		// Layouts
		//==============================================================================================

		/// Registers the given layout on this element and returns a reference object. After calling the 
		/// function, the layout is owned by this element.
		template<typename LayoutType>
		LayoutType::LayoutReference registerLayout(LayoutType&& layout);
		/// Adds a child element with the given options for the given layout. The layout reference object
		/// must be one created by calling registerLayout on this guiElement!
		template<typename LayoutType>
		void addChild(const typename LayoutType::LayoutReference& layoutReference, const GuiID& childID, const typename LayoutType::LayoutOptions& options);
		/// Adds a child to the standard layout
		void addChild(const GuiID& childID);
		/// Enforces all layouts of this element, which will may also call enforceLayouts() on children elements
		void enforceLayouts();
		/// Enforces a specific layout of this element, which will may also call enforceLayouts() on children elements
		void enforceLayout(const GuiID& layoutID);
		/// Returns a reference to the layout with the given layoutID
		GuiLayout& getLayout(const GuiID& layoutID);
	public:
		/// Default constructor
		GuiElement(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i());
		/// Copy constructor
		GuiElement(const GuiElement& other) noexcept;
		/// Copy assignment operator
		GuiElement& operator=(const GuiElement& other) noexcept;
		/// Move constructor
		GuiElement(GuiElement&& other) noexcept;
		/// Move assignment operator
		GuiElement& operator=(GuiElement&& other) noexcept;
		/// Sets the position of this element. Calls OnPositionChange() and enforces the layouts of the parent
		/// element, this element, and child elements
		virtual void setPosition(const Vec2i& position);
		/// Sets the size of this element. Calls OnPositionChange() and enforces the layouts of the parent
		/// element, this element, and child elements
		virtual void setSize(const Vec2i& size);
		/// Destructor
		~GuiElement();
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiElement::setVariableHandleValue(GuiVariableHandle<T>& variableHandle, const T& value)
	{
		variableHandle->val = value;
		variableHandle->activate();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename LayoutType>
	inline LayoutType::LayoutReference GuiElement::registerLayout(LayoutType&& layout)
	{
		layouts.push_back(std::make_unique<LayoutType>(std::move(layout)));
		layouts.back()->parentID = guiID;
		layouts.back()->guiID = nextLayoutID;
		layoutIDToIndex[nextLayoutID] = layouts.size() - 1;
		if (guiManager) {
			layouts.back()->guiManager = guiManager;
			layouts.back()->onRegister();
		}
		return LayoutType::LayoutReference(nextLayoutID++);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename LayoutType>
	inline void GuiElement::addChild(const typename LayoutType::LayoutReference& layoutReference, const GuiID& childID, const typename LayoutType::LayoutOptions& options)
	{
		auto result = layoutIDToIndex.find(layoutReference.layoutID);
		if (result == layoutIDToIndex.end()) {
			warningLogger << LOGGER::BEGIN << "Tried to add guiElement to layout using an invalid layoutReference!" << LOGGER::ENDL;
			return;
		}
		static_cast<LayoutType*>(layouts[result->second].get())->addChild(childID, options);
	}
	//--------------------------------------------------------------------------------------------------
}