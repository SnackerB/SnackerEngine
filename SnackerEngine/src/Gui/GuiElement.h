#pragma once

#include "Math/Vec.h"
#include "Utility/Json.h"
#include "Gui/GuiEventHandles/GuiVariableHandle.h"
#include "Gui/SizeHints.h"
#include "Gui\Text\Font.h"

#include <vector>
#include <optional>
#include <set>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declarations
	class GuiManager;
	class GuiGroup;
	using GuiGroupID = int;
	//--------------------------------------------------------------------------------------------------
	/// Base class for all GuiElements, including Layouts!
	class GuiElement
	{
	public:
		/// Enum that can be used to distinguish different behaviours for resizing done by parent
		/// GuiElements. Note that this does not have to be followed by the parent GuiElement,
		/// and is more of a resize hint.
		enum class ResizeMode
		{
			SAME_AS_PARENT,		/// Always have the same size as the parent element. Usually the
								/// go-to mode for Layout objects
			RESIZE_RANGE,		/// Size can be set to anything in between the member variables 
								/// minSize and maxSize
		};
		/// Values >= 0 denote valid GuiIDs
		using GuiID = int;
		/// Static default Attributes
		static double defaultFontSizeSmall;
		static double defaultFontSizeNormal;
		static double defaultFontSizeBig;
		static double defaultFontSizeHuge;
		static Font defaultFont;
		static int defaultBorderSmall;
		static int defaultBorderNormal;
		static int defaultBorderLarge;
		static int defaultBorderHuge;
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_ELEMENT";
		/// Default constructor
		GuiElement(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const ResizeMode& resizeMode = ResizeMode::RESIZE_RANGE);
		/// Constructor from JSON
		GuiElement(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiElement();
		/// Copy constructor and assignment operator
		GuiElement(const GuiElement& other) noexcept;
		GuiElement& operator=(const GuiElement& other) noexcept;
		/// Move constructor and assignment operator
		GuiElement(GuiElement&& other) noexcept;
		GuiElement& operator=(GuiElement&& other) noexcept;
		/// Adds a child to this guiElement. Returns true on success
		virtual bool registerChild(GuiElement& guiElement);
		/// Adds a child to this guiElement, with options given in JSON
		virtual bool registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Deletes all children of this guiElement
		void deleteChildren();
		/// Returns true if this GuiElement object is managed by a guiManager
		bool isValid();
		/// Sets the position of this element. May register children and/or parent for
		/// enforcing layouts.
		void setPosition(const Vec2i& position);
		void setPositionX(const int& positionX);
		void setPositionY(const int& positionY);
		/// Sets the size of this element. May register children and/or parent for
		/// enforcing layouts.
		void setSize(const Vec2i& size);
		void setWidth(const int& width);
		void setHeight(const int& height);
		/// Sets the position and the size of this element. May register children and/or parent for
		/// enforcing layouts. If the position and size is to be set,
		/// this function should be preferred over two seperate calls of setPosition()
		/// and setSize()
		void setPositionAndSize(const Vec2i& position, const Vec2i& size);
		/// Clamps the given size vector to the min/max size of this GuiElement
		Vec2i clampToMinMaxSize(const Vec2i& size) const;
		int clampToMinMaxWidth(int width) const;
		int clampToMinMaxHeight(int height) const;
		/// Setters for min/max/preferredSize. May May register children and/or parent for
		/// enforcing layouts as well
		void setMinSize(const Vec2i& minSize);
		void setMinWidth(const int& minWidth);
		void setMinHeight(const int& minHeight);
		void setMaxSize(const Vec2i& maxSize);
		void setMaxWidth(const int& maxWidth);
		void setMaxHeight(const int& maxHeight);
		void setPreferredSize(const Vec2i& preferredSize);
		void setPreferredWidth(const int& preferredWidth);
		void setPreferredHeight(const int& preferredHeight);
		void setResizeMode(ResizeMode resizeMode) { this->resizeMode = resizeMode; }
		/// Getters
		const std::string& getName() const { return name; }
		GuiID getGuiID() const { return guiID; }
		GuiID getParentID() const { return parentID; }
		unsigned getDepth() const { return depth; }
		const Vec2i& getPosition() const { return position; }
		int getPositionX() const { return position.x; }
		int getPositionY() const { return position.y; }
		const Vec2i& getSize() const { return size; }
		const int& getWidth() const { return size.x; }
		const int& getHeight() const { return size.y; }
		const ResizeMode& getResizeMode() const { return resizeMode; }
		const Vec2i& getMinSize() const { return sizeHints.minSize; }
		const int& getMinWidth() const { return sizeHints.minSize.x; }
		const int& getMinHeight() const { return sizeHints.minSize.y; }
		const Vec2i& getMaxSize() const { return sizeHints.maxSize; }
		const int& getMaxWidth() const { return sizeHints.maxSize.x; }
		const int& getMaxHeight() const { return sizeHints.maxSize.y; }
		const Vec2i& getPreferredSize() const { return sizeHints.preferredSize; }
		const int& getPreferredWidth() const { return sizeHints.preferredSize.x; }
		const int& getPreferredHeight() const { return sizeHints.preferredSize.y; }
		const GuiSizeHints& getSizeHints() const { return sizeHints; }
		const std::vector<GuiID>& getChildren() const { return children; }
		/// Returns mouseOffset from this GuiElement
		const Vec2i getMouseOffset() const;
		int getMouseOffsetX() const;
		int getMouseOffsetY() const;
	private:
		friend class GuiManager;
		friend class GuiHandle;
		/// Pointer to the parent guiManager
		GuiManager* guiManager = nullptr;
		/// Name of this element. Can also be empty
		std::string name = "";
		/// GuiID of this GuiElement object
		GuiID guiID = -1;
		/// guiID of the parent GuiElement object. If this is zero, this GuiElement object does not 
		/// have a parent object.
		GuiID parentID = -1;
		/// The depth is the depth of this element when drawing the GUI as a tree, with the GuiElement with GuiID 0
		/// as the root! This variable is automatically set by the GuiManager when registering an element
		unsigned int depth = 0;
		/// Position of this GuiElement object relative to its parent. The position vector points
		///  to the upper left corner of the bounding rectangle.
		Vec2i position = Vec2i();
		/// Dimensions of the bounding rectangle in pixels. The origin is at the upper left corner
		Vec2i size = Vec2i();
		/// The resize mode of this element. For more explanation see the definition of
		/// the enum class above
		ResizeMode resizeMode = ResizeMode::RESIZE_RANGE;
		/// The resizeHints of this element. See definition of GuiSizeHints struct.
		GuiSizeHints sizeHints;
		/// Vector of child elements. Sorted in the order they will be drawn (but this can be
		/// changed in derived elements by overwriting draw())
		std::vector<GuiID> children{};
		/// Tells this GuiElement object that the guiManager was deleted.
		virtual void signOff();
		/// Friend classes
		friend class GuiManager;
	protected:
		/// Signs off a child element without notifying the parent. Useful if additional children belong to this Element
		/// which are not listed in the children vector! (ie. GuiCheckBox, GuiEditVariable, etc.)
		void signOffWithoutNotifyingParents(const GuiID& guiID);
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition);
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
		/// Removes the given child from this GuiElement object. Returns the index
		/// the element had into the children vector, if it existed
		virtual std::optional<unsigned> removeChild(GuiID guiElement);
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May register children and/or parent for
		/// enforcing layouts as well
		virtual void enforceLayout();
		/// Registers an element at the guiManager without pushing it to the children
		/// vector (Used for registering guiElements that are part of other
		/// guiElements, eg. the window bar of a window element)
		void registerElementAsChild(GuiElement& guiElement);
		/// Returns the mouse offset of a child element from this element. Can be
		/// overwritten if the children are displayed at a different place than they
		/// are (eg. in a scrolling list etc)
		virtual Vec2i getChildOffset(const GuiID& childID) const;
		/// Returns the index of an element into the children vector, if
		/// the element is a child of this GuiElement.
		std::optional<unsigned> getIndexIntoChildrenVector(GuiID childID) const;
		/// Returns a const pointer ref to the GuiManager
		GuiManager* const& getGuiManager() { return guiManager; }
		/// Returns the element with the given ID (if it exists)
		GuiElement* getElement(GuiID guiID) const;
		/// Sets position directly without calling OnPositionChange()
		/// Use only if you know what you're doing!
		void setPositionInternal(const Vec2i& position);
		/// Sets size directly without calling OnPositionChange()
		/// Use only if you know what you're doing!
		void setSizeInternal(const Vec2i& size);
	public:
		/// This function can be used to tell the GuiManager that this element wants to enforce its layout
		/// and the layouts of its child elements if necessary
		void registerEnforceLayoutDown();
	protected:
		/// Draws a child
		void drawElement(GuiID element, Vec2i worldPosition);
		/// Using these setters a guiElement can change the position and size of child elements.
		/// This will trigger them to enforce layouts on themselves and their children respectively.
		/// This will of course also call onSizeChange() and/or onPositionChange() on the child element affected.
		void setPositionAndSizeOfChild(const GuiID& guiID, const Vec2i& position, const Vec2i& size);
		void setPositionOfChild(const GuiID& guiID, const Vec2i& position);
		void setPositionXOfChild(const GuiID& guiID, const int& positionX);
		void setPositionYOfChild(const GuiID& guiID, const int& positionY);
		void setSizeOfChild(const GuiID& guiID, const Vec2i& size);
		void setWidthOfChild(const GuiID& guiID, const int& width);
		void setHeightOfChild(const GuiID& guiID, const int& height);

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
		void notifyHandleOnGuiElementMove(GuiElement* oldElement, GuiHandle& guiHandle);
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
		// Collisions
		//==============================================================================================

	public:
		/// Enum that contains the possible return types of isColliding
		enum class IsCollidingResult
		{
			NOT_COLLIDING,				/// No collision detected
			COLLIDE_IF_CHILD_DOES_NOT,	/// Collision detected, but if a childElement is also colliding, 
			/// choose it instead
			COLLIDE_CHILD,				/// Check for collisions only on child elements
			COLLIDE_STRONG,				/// Collision detected, no child elements should be checked!
		};
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const;
		/// Helper function that returns true if a collision occured inside the bounding box given by
		/// the position and size of this element
		bool isCollidingBoundingBox(const Vec2i& offset) const;
	protected:
		/// Helper function used in getCollidingChild(const Vec2i& offset): Based on the collision mode
		/// returns either the child element or a child of the child element!
		GuiID getCollidingChild(const IsCollidingResult& collidingResult, const GuiID& childID, const Vec2i& offset) const;
		/// Returns the first colliding child which collides with the given offset vector. The offset
		/// vector is relative to the top left corner of the guiElement. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& offset) const;

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
			DRAW_ON_TOP,
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
		/// Draws this GuiElement object relative to its parent element. This draw call is executed after
		/// all regular draw calls. This will NOT recursively draw all children of this element.
		/// If this is the desired behaviour, call the normal draw() function on all children
		/// in this function.
		/// worldPosition:		position of the upper left corner of the element in world space
		virtual void callbackDrawOnTop(const Vec2i& worldPosition) {};

		//==============================================================================================
		// ClippingBoxes (Scissor test) for rendering
		//==============================================================================================

		/// Pushes a clipping box to the stack and enables the scissor test
		void pushClippingBox(const Vec4i& clippingBox);
		/// Computes a clipping box from the worldPosition and pushes it to the stack
		void pushClippingBox(const Vec2i& worldPosition);
		/// pops a clipping box from the stack
		void popClippingBox();

		//==============================================================================================
		// Animatables
		//==============================================================================================

		// TODO

		//==============================================================================================
		// Groups
		//==============================================================================================
	protected:
		/// Tries to add the element to the group with the given groupID. If the group 
		/// does not exist, false is returned.
		bool joinGroup(GuiGroupID groupID);
		/// Tries to create a group with the given name and return a new group ID. 
		/// If a group with this name already exists, an empty optional is returned instead.
		/// If the name is the empty string, it is ignored and the group is created in any case.
		std::optional<GuiGroupID> createGroup(std::unique_ptr<GuiGroup>&& group);
		/// Checks if a group with the given name exists. Returns the groupID if it does, and
		/// an empty optional if it does not.
		std::optional<GuiGroupID> groupExists(const std::string& groupName);
		/// Checks if a group with the given ID exists
		bool groupExists(GuiGroupID groupID);
		/// Returns a reference to the group with the given name, if it exists
		GuiGroup* getGroup(const std::string& groupName);
		/// Returns a reference to the group with the given ID, if it exists
		GuiGroup* getGroup(GuiGroupID groupID);
		/// Removes the element from the group with the given ID.
		/// If the group is empty after this, the group is deleted.
		void leaveGroup(GuiGroupID groupID);
		/// Removes the element from all groups it is currently in. Must be overwritten if this element
		/// belongs to any groups
		virtual void leaveAllGroups() {};

	};

	template<typename T>
	inline void GuiElement::setVariableHandleValue(GuiVariableHandle<T>& variableHandle, const T& value)
	{
		variableHandle.val = value;
		variableHandle.activate();
		variableHandle.onHandleUpdateFromElement(*this);
	}

	/*
	// =================================================================================================
	// Class template for new GuiElements!
	// =================================================================================================

	// A new GuiElement can be implemented by using the following class template. Below the class templates
	// there are templates given for the definition of some of the functions that any GuiElement has
	// to implement.
	
	class GuiElementType
	{
	public:

		// NOTE: The following public variables and functions with the exception of the destructor
		// MUST be implemented for any GuiElement!

		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_ELEMENT";
		/// Default constructor
		GuiElementType(...); // NOTE: All parameters must have default values!
		/// Constructor from JSON
		GuiElement(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiElementType() {};
		/// Copy constructor and assignment operator
		GuiElementType(const GuiElementType& other) noexcept;
		GuiElementType& operator=(const GuiElementType& other) noexcept;
		/// Move constructor and assignment operator
		GuiElementType(GuiElementType&& other) noexcept;
		GuiElementType& operator=(GuiElementType&& other) noexcept;

		// NOTE: both registerChild do not need to be overloaded.

		/// Adds a child to this guiElement. Returns true on success
		virtual bool registerChild(GuiElement& guiElement);
		/// Adds a child to this guiElement, with options given in JSON
		virtual bool registerChild(GuiElement& guiElement, const nlohmann::json& json, const nlohmann::json* data);
		
	protected:

		// NOTE: Overloading any of the following protected functions is optional

		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the position changes. Can eg. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onPositionChange() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Removes the given child from this GuiElement object
		virtual void removeChild(GuiID guiElement) override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May register children and/or parent for
		/// enforcing layouts as well
		virtual void enforceLayout() override;
		/// Returns the mouse offset of a child element from this element. Can be
		/// overwritten if the children are displayed at a different place than they
		/// are (eg. in a scrolling list etc)
		virtual Vec2i getChildOffset(const GuiID& childID) const override;
		
		//==============================================================================================
		// GuiHandles
		//==============================================================================================

		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle);
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle);

		//==============================================================================================
		// Collisions
		//==============================================================================================

		// NOTE: Overloading any function in the "Collisions" section is optional

		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
	public:
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;
		/// Returns the first colliding child which collides with the given offset vector. The offset
		/// vector is relative to the top left corner of the guiElement. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& offset) const override;

		//==============================================================================================
		// Events
		//==============================================================================================

		// NOTE: Overloading any function in the "Events" section is optional

	protected:
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		// callback function for keyboard input. Parameters the same as in Scene.h
		virtual void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) override;
		/// Callback function for the input of unicode characters. Parameter the same as in Scene.h
		virtual void callbackCharacterInput(const unsigned int& codepoint) override;
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for scrolling the mouse wheel. Parameter the same as in Scene.h
		virtual void callbackMouseScrollOnElement(const Vec2d& offset) override;
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
		/// Update function
		virtual void update(const double& dt) override;
		/// Draws this GuiElement object relative to its parent element. This draw call is executed after
		/// all regular draw calls. This will NOT recursively draw all children of this element.
		/// If this is the desired behaviour, call the normal draw() function on all children
		/// in this function.
		/// worldPosition:		position of the upper left corner of the element in world space
		virtual void callbackDrawOnTop(const Vec2i& worldPosition) override;

		//==============================================================================================
		// Animatables
		//==============================================================================================

		// TODO

		//==============================================================================================
		// Groups
		//==============================================================================================
	protected:
		/// Removes the element from all groups it is currently in. Must be overwritten if this element
		/// belongs to any groups
		virtual void leaveAllGroups() {};
	};

	GuiElementType::GuiElementType(...)
		: ParentElementType(...), ...
	{
	}

	GuiElementType::GuiElementType(const nlohmann::json& json, const nlohmann::json* data)
		: ParentElementType(json, data)
	{
	}

	GuiElementType::GuiElementType(const GuiElementType& other)
		: ParentElementType(other), ...
	{
	}
	
	GuiElementType::GuiElementType& operator=(const GuiElementType& other)
	{
		ParentElementType::operator=(other);
		...
		return *this;
	}
		
	GuiElementType::GuiElementType(GuiElementType&& other)
		: ParentElementType(std::move(other)), ...
	{
	}

	GuiElementType::operator=(GuiElementType&& other) 
	{
		ParentElementType::operator=(other);
		...
		return *this;
	}

	*/
}