#pragma once

#include "Math/Vec.h"
#include "Utility/Json.h"

#include <vector>
#include <optional>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declarations
	class GuiManager;
	//--------------------------------------------------------------------------------------------------
	/// Hacky way for differentiating two constructors with same arguments
	struct defaultConstructor_t {};
	constexpr defaultConstructor_t defaultConstructor = defaultConstructor_t();
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
			DO_NOT_RESIZE,		/// Size is set by this element, should not be resized
			RESIZE_RANGE,		/// Size can be set to anything in between the member variables 
								/// minSize and maxSize
		};
		/// Values >= 0 denote valid GuiIDs
		using GuiID = int;

		/// Default constructor
		GuiElement(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const ResizeMode& resizeMode = ResizeMode::DO_NOT_RESIZE);
		/// Constructor for loading from JSON file
		GuiElement(const nlohmann::json& json, const nlohmann::json* data);
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
		/// Deletes all children of this guiElement
		void deleteChildren();
		/// Returns true if this GuiElement object is managed by a guiManager
		bool isValid();
		/// Sets the position of this element. May call enforceLayout() on the parent
		/// and child elements
		void setPosition(const Vec2i& position);
		void setPositionX(const int& positionX);
		void setPositionY(const int& positionY);
		/// Sets the size of this element. May call enforceLayout() on the parent
		/// and child elements
		void setSize(const Vec2i& size);
		void setWidth(const int& width);
		void setHeight(const int& height);
		/// Sets the position and the size of this element. May call enforceLayout()
		/// on the parent and child elements. If the position and size is to be set,
		/// this function should be preferred over two seperate calls of setPosition()
		/// and setSize()
		void setPositionAndSize(const Vec2i& position, const Vec2i& size);
		/// Setters for min/max/preferredSize. May call enforceLayout() on the parent element.
		void setMinSize(const Vec2i& minSize);
		void setMinWidth(const int& minWidth);
		void setMinHeight(const int& minHeight);
		void setMaxSize(const Vec2i& maxSize);
		void setMaxWidth(const int& maxWidth);
		void setMaxHeight(const int& maxHeight);
		void setPreferredSize(const Vec2i& preferredSize);
		void setPreferredWidth(const int& preferredWidth);
		void setPreferredHeight(const int& preferredHeight);
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
		const Vec2i& getMinSize() const { return minSize; }
		const int& getMinWidth() const { return minSize.x; }
		const int& getMinHeight() const { return minSize.y; }
		const Vec2i& getMaxSize() const { return maxSize; }
		const int& getMaxWidth() const { return maxSize.x; }
		const int& getMaxHeight() const { return maxSize.y; }
		const Vec2i& getPreferredSize() const { return preferredSize; }
		const int& getPreferredWidth() const { return preferredSize.x; }
		const int& getPreferredHeight() const { return preferredSize.y; }
		const std::vector<GuiID>& getChildren() const { return children; }

	private:
		/// Pointer to the parent guiManager
		GuiManager* guiManager;
		/// Name of this element. Can also be empty
		std::string name;
		/// GuiID of this GuiElement object
		GuiID guiID;
		/// guiID of the parent GuiElement object. If this is zero, this GuiElement object does not 
		/// have a parent object.
		GuiID parentID;
		/// The depth is the depth of this element when drawing the GUI as a tree, with the GuiElement with GuiID 0
		/// as the root! This variable is automatically set by the GuiManager when registering an element
		unsigned int depth;
		/// Position of this GuiElement object relative to its parent. The position vector points
		///  to the upper left corner of the bounding rectangle.
		Vec2i position;
		/// Dimensions of the bounding rectangle in pixels. The origin is at the upper left corner
		Vec2i size;
		/// The resize mode of this element. For more explanation see the definition of
		/// the enum class above
		ResizeMode resizeMode;
		/// The minimal and maximal size that this GuiElement object can be resized to. Only used
		/// when this elements resizeMode is set to ResizeMode::RESIZE_RANGE
		Vec2i minSize;
		/// If a component of maxSize is set to -1 this means that there is no constraint along
		/// this direction and the element can get as large as necessary
		Vec2i maxSize;
		/// The preferred size of this element. If a component of preferredSize is set to -1
		/// this means that there is no preferred size along this direction!
		Vec2i preferredSize;
		/// Vector of child elements. Sorted in the order they will be drawn (but this can be
		/// changed in derived elements by overwriting draw())
		std::vector<GuiID> children;
		/// Tells this GuiElement object that the guiManager was deleted.
		virtual void signOff();
		/// Friend classes
		friend class GuiManager;
	protected:
		/// Default/Copy/Move constructors which do not perform initial calculations
		GuiElement(defaultConstructor_t, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const ResizeMode& resizeMode = ResizeMode::DO_NOT_RESIZE);
		GuiElement(defaultConstructor_t, const GuiElement& other) noexcept;
		GuiElement(defaultConstructor_t, GuiElement&& other) noexcept;
		/// Copy/Move operators which do not perform initial calculations
		void copyFromWithoutInitializing(const GuiElement& other);
		void moveFromWithoutInitializing(GuiElement&& other);
		/// Initializes the GuiElement. Is called after construction. Should be used for setting up modelMatrices,
		/// and other initial computations. Should call initialize() on the parent element recursively.
		virtual void initialize();
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
		/// Removes the given child from this GuiElement object
		virtual void removeChild(GuiID guiElement);
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout();
		/// Registers an element at the guiManager without pushing it to the children
		/// vector (Used for registering guiElements that are part of other
		/// guiElements, eg. the window bar of a window element)
		void registerElementAsChild(GuiElement& guiElement);
		/// Returns the mouse offset of a child element from this element. Can be
		/// overwritten if the children are displayed at a different place than they
		/// are (eg. in a scrolling list etc)
		virtual Vec2i getChildOffset(const GuiID& childID) const;
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
		/// This function can be used to tell the GuiManager that this element wants to enforce its layout
		/// and the layouts of its child elements if necessary
		void registerEnforceLayoutDown();
		/// Parses this element from a JSON file (and optionally a data file). This function is used
		/// for recursive construction from a JSON file
		virtual void parseFromJSON(const nlohmann::json& json, const nlohmann::json* data);
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

		// TODO

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
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const;
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


	};
}