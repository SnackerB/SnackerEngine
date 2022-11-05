#pragma once

#include "Math/Vec.h"
#include "Gui/GuiEventHandles/GuiHandle2.h"
#include "Gui/GuiEventHandles/GuiVariableHandle2.h"

#include <vector>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declarations
	class GuiManager2;
	//--------------------------------------------------------------------------------------------------
	/// Base class for all GuiElements, including Layouts!
	class GuiElement2
	{
	public:
		/// Enum that can be used to distinguish different behaviours for resizing done by other 
		/// GuiElements.
		enum class ResizeMode
		{
			SAME_AS_PARENT,		/// Always have the same size as the parent element. Usually the
								/// go-to mode for Layout objects
			DO_NOT_RESIZE,		/// Size is set by this element, should not be resized
			RESIZE_RANGE,		/// Size can be set to anything in between the member variables 
								/// minSize and maxSize
		};

	protected:
		friend class GuiManager2;
		friend class GuiHandle2;
		using GuiID = int;
		/// Pointer to the parent guiManager
		GuiManager2* guiManager;
	private:
		/// GuiID of this GuiElement object
		GuiID guiID;
		/// guiID of the parent GuiElement object. If this is zero, this GuiElement object does not 
		/// have a parent object.
		GuiID parentID;
	protected:
		/// Position of this GuiElement object relative to its parent. The position vector points
		///  to the upper left corner of the bounding rectangle.
		Vec2i position;
		/// Dimensions of the bounding rectangle in pixels. The origin is at the upper left corner
		Vec2i size;
	private:
		/// The resize mode of this element. For more explanation see the definition of
		/// the enum class above
		ResizeMode resizeMode;
		/// The minimal and maximal size that this GuiElement object can be resized to. Only used
		/// when this elements resizeMode is set to ResizeMode::RESIZE_RANGE
		Vec2i minSize;
		/// If a component of maxSize is set to -1 this means that there is no constraint along
		/// this direction and the element can get as large as necessary
		Vec2i maxSize;
		/// Vector of child elements. Sorted in the order they will be drawn (but this can be
		/// changed in derived elements by overwriting draw())
		std::vector<GuiID> children;
		/// Tells this GuiElement object that the guiManager was deleted
		void signOff();
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition);
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() {};
		/// This function is called when the position changes. Can eg. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onPositionChange() {};
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() {};
		/// Removes the given child from this GuiElement object
		virtual void removeChild(GuiElement2& guiElement);
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout();
		/// Registers an element at the guiManager without pushing it to the children
		/// vector (Used for registering guiElements that are part of other
		/// guiElements, eg. the window bar of a window element)
		void registerElementAsChild(GuiElement2& guiElement);

		//==============================================================================================
		// GuiHandles
		//==============================================================================================

		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle2& guiHandle) {};
		/// This function should be called by the guiElement to sign up a new handle
		void signUpHandle(GuiHandle2& guiHandle, const GuiHandle2::GuiHandleID& handleID);
		/// This function should be called to sign off a given eventHandle if it is no longer needed
		/// (e.g. destruction of a guiElement) 
		void signOffHandle(GuiHandle2& guiHandle);
		/// This function should be called when moving a guiElement that owns a guiHandle
		void notifyHandleOnGuiElementMove(GuiHandle2& guiHandle);
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle2& guiHandle) {};
		/// This function calls activate() on the given GuiEventHandle
		void activate(GuiEventHandle2& guiEventHandle);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle2& guiHandle) {};
		/// template function used to change a value of a variable handle
		template<typename T>
		void setVariableHandleValue(GuiVariableHandle2<T>& variableHandle, const T& value);

		//==============================================================================================
		// Collisions
		//==============================================================================================

		/// Enum that contains the possible return types of isColliding
		enum class IsCollidingResult
		{
			NOT_COLLIDING,				/// No collision detected
			COLLIDE_IF_CHILD_DOES_NOT,	/// Collision detected, but if a childElement is also colliding, 
										/// choose it instead
			COLLIDE_CHILD,				/// Check for collisions only on child elements
			COLLIDE_STRONG,				/// Collision detected, no child elements should be checked!
		};
		/// Returns how the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position);
		/// Helper function used in getCollidingChild(const Vec2i& position): Based on the collision mode
		/// returns either the child element or a child of the child element!
		GuiID getCollidingChild(const IsCollidingResult& collidingResult, const GuiID& childID, const Vec2i& position);
		/// Returns the first colliding child which collides with the given position vector. The position
		/// vector is relative to the top left corner of the parent. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& position);

		//==============================================================================================
		// Events
		//==============================================================================================

		/// Enum for different types of update/callback events a GuiElement object can register 
		/// itself for
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

		/// This function can be called by derived GuiElement objects to register themselves for getting 
		/// notified when events happen
		void signUpEvent(const CallbackType& callbackType);
		/// This function can be called by derived GuiElement objects to sign off for getting notified 
		/// when certain events happen
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
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
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

		//==============================================================================================
		// Access to (some) functionalities of child elements for derived guiElements
		//==============================================================================================

		const Vec2i& getPosition(const GuiID& guiID);
		const int& getPositionX(const GuiID& guiID);
		const int& getPositionY(const GuiID& guiID);
		const Vec2i& getSize(const GuiID& guiID);
		const int& getWidth(const GuiID& guiID);
		const int& getHeight(const GuiID& guiID);
		const GuiID& getParentID(const GuiID& guiID);
		const Vec2i& getMouseOffset(const GuiID& guiID);
		const ResizeMode& getResizeMode(const GuiID& guiID);
		const Vec2i& getMinSize(const GuiID& guiID);
		const Vec2i& getMaxSize(const GuiID& guiID);
		const IsCollidingResult& isColliding(const GuiID& guiID, const Vec2i& parentPosition);
		void setPosition(const GuiID& guiID, const Vec2i& position);
		void setSize(const GuiID& guiID, const Vec2i& size);
		/// Sets position directly without calling OnPositionChange()
		/// Should only be used during construction!
		void setPositionInternal(const Vec2i& position);
		/// Sets size directly without calling OnPositionChange()
		/// Should only be used during construction!
		void setSizeInternal(const Vec2i& size);
		void setPositionAndSize(const GuiID& guiID, const Vec2i& position, const Vec2i& size);
		void setPositionWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& position);
		void setPositionXWithoutEnforcingLayouts(const GuiID& guiID, const int& x);
		void setPositionYWithoutEnforcingLayouts(const GuiID& guiID, const int& y);
		void setSizeWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& size);
		void setWidthWithoutEnforcingLayouts(const GuiID& guiID, const int& width);
		void setHeightWithoutEnforcingLayouts(const GuiID& guiID, const int& height);
		void setPositionAndSizeWithoutEnforcingLayouts(const GuiID& guiID, const Vec2i& position, const Vec2i& size);
		void enforceLayoutOnElement(const GuiID& guiID);
		void drawElement(const GuiID& guiID, const Vec2i& newParentPosition);
		/// Adds a child to this guiElement. Returns true on success. Does not enforce any layouts
		bool registerChildWithoutEnforcingLayouts(GuiElement2& guiElement);

	public:
		/// Default constructor
		GuiElement2(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), 
			const ResizeMode& resizeMode = ResizeMode::DO_NOT_RESIZE);
		/// Destructor
		~GuiElement2();
		/// Copy constructor and assignment operator
		GuiElement2(const GuiElement2& other) noexcept;
		GuiElement2& operator=(const GuiElement2& other) noexcept;
		/// Move constructor and assignment operator
		GuiElement2(GuiElement2&& other) noexcept;
		GuiElement2& operator=(GuiElement2&& other) noexcept;
		/// Adds a child to this guiElement. Returns true on success
		virtual bool registerChild(GuiElement2& guiElement);
		/// Returns true if this GuiElement object is managed by a guiManager
		bool isValid();
		/// Sets the position of this element. May call enforceLayout() on the parent
		/// and child elements
		void setPosition(const Vec2i& position);
		/// Sets the size of this element. May call enforceLayout() on the parent
		/// and child elements
		void setSize(const Vec2i& size);
		/// Sets the position and the size of this element. May call enforceLayout()
		/// on the parent and child elements. If the position and size is to be set,
		/// this function should be preferred over two seperate calls of setPosition()
		/// and setSize()
		void setPositionAndSize(const Vec2i& position, const Vec2i& size);
		/// Getters
		const GuiID& getGuiID() const { return guiID; }
		const GuiID& getParentID() const { return parentID; }
		const Vec2i& getPosition() const { return position; }
		const int& getPositionX() const { return position.x; }
		const int& getPositionY() const { return position.y; }
		const Vec2i& getSize() const { return size; }
		const int& getWidth() const { return size.x; }
		const int& getHeight() const { return size.y; }
		const ResizeMode& getResizeMode() const { return resizeMode; }
		const Vec2i& getMinSize() const { return minSize; }
		const Vec2i& getMaxSize() const { return maxSize; }
		const std::vector<GuiID>& getChildren() const { return children; }
	};

	template<typename T>
	inline void GuiElement2::setVariableHandleValue(GuiVariableHandle2<T>& variableHandle, const T& value)
	{
		variableHandle.val = value;
		variableHandle.activate();
	}

}