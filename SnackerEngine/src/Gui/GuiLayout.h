#pragma once

#include "Math/Vec.h"
#include "Gui/GuiElement.h"

#include <span>
#include <optional>

namespace SnackerEngine
{

	class GuiLayoutReference
	{
		friend class GuiElement;
		using GuiID = unsigned int;
		GuiID layoutID;
	protected:
		GuiLayoutReference(const GuiID& layoutID)
			: layoutID(layoutID) {}
	};

	struct GuiLayoutOptions
	{
	};

	class GuiLayout : public GuiInteractable
	{
	public:
		using LayoutReference = GuiLayoutReference;
		using LayoutOptions = GuiLayoutOptions;
	protected:
		friend class GuiElement;
		friend class GuiManager;
		using GuiID = unsigned int;
		/// Vector of children GuiElements
		std::vector<GuiID> children;
		/// Sets the parentID of this layout and all children. Called by guiManager when registering a parent guiElement
		virtual void handleLayoutOnParentRegister(GuiID parentID);
		/// Adds the given child guiElement to this layout
		void addChild(const GuiID& childID, const GuiLayoutOptions& options);
		/// Removes the given child guiElement from this layout. Returns the index into the children
		/// vector that the removed element had before it was removed.
		virtual std::size_t removeChild(GuiElement& guiElement);
		/// Enforces the layout by potentially repositioning/resizing child elements. May call enforceLayout() on layouts
		/// of child elements
		virtual void enforceLayout() {}
		/// Draws this guiLayout relative to its parent element
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Returns the first colliding child element, if it exists.
		/// position:	position vector (relative to the top left corner of the parent element)
		/// Returns:    <<GuiID of colliding child, IsCollidingResult>, new offset vector>
		virtual std::optional<std::pair<std::pair<GuiID, IsCollidingResult>, Vec2i>> getFirstCollidingChild(const Vec2i& position);
		/// Constructor using the parent guiElement 
		GuiLayout();

		//==============================================================================================
		// Helper functions for derived classes (getters&setters)
		//==============================================================================================
		Vec2i getSize(const GuiID& guiID);
		Vec2i getPosition(const GuiID& guiID);
		Vec2i getPreferredSize(const GuiID& guiID);
		Vec2i getPreferredMinSize(const GuiID& guiID);
		Vec2i getPreferredMaxSize(const GuiID& guiID);
		Vec2f getMouseOffset(const GuiID& guiID);
		void setSize(const GuiID& guiID, const Vec2f& size);
		void setPosition(const GuiID& guiID, const Vec2f& position);
		void setWidth(const GuiID& guiID, const int width);
		void setHeight(const GuiID& guiID, const int height);
		void setPreferredSize(const GuiID& guiID, const Vec2f& preferredSize);
		void setPreferredMinSize(const GuiID& guiID, const Vec2f& preferredMinSize);
		void setPreferredMaxSize(const GuiID& guiID, const Vec2f& preferredMaxSize);
		void notifyPositionChange(const GuiID& guiID);
		void notifySizeChange(const GuiID& guiID);
		void enforceLayouts(const GuiID& guiID);
		void drawElement(const GuiID& guiID, const Vec2i& parentPosition);
	public:
		/// Destructor
		~GuiLayout() noexcept;
	};

}