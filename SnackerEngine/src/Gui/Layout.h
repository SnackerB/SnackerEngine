#pragma once

#include "Math/Vec.h"

namespace SnackerEngine
{

	class GuiManager;

	class GuiLayoutReference
	{
		friend class GuiElement;
		unsigned referenceID;
	protected:
		GuiLayoutReference(const unsigned& referenceID)
			: referenceID(referenceID) {}
	};

	class Layout
	{
	protected:
		friend class GuiElement;
		friend class GuiManager;
		using GuiID = unsigned int;
	protected:
		/// The ID of the guiElement owning this layout
		GuiID elementID;
		/// Pointer to the guiManager managing the element owning this layout
		GuiManager* guiManager;
		/// Sets the position of a guiElement without enforcing its layout
		void setPosition(const GuiID& guiID, const Vec2i& position);
		/// Sets the size of a guiElement without enforcing its layout
		void setSize(const GuiID& guiID, const Vec2i& size);
		/// Returns the position of a guiElement
		const Vec2i& getPosition(const GuiID& guiID);
		/// Returns the size of a guiElement
		const Vec2i& getSize(const GuiID& guiID);
		/// Returns the parentID of a guiElement
		const GuiID& getParentID(const GuiID& guiID);
		/// Enforces the layouts of a guiElement, should be called after setting its position and size
		void enforceLayouts(const GuiID& guiID);
		/// Calls OnPositionChange() on the given guiElement
		void notifyPositionChange(const GuiID& guiID);
		/// Calls OnSizeChange() on the given guiElement
		void notifySizeChange(const GuiID& guiID);
	public:
		Layout() : elementID(0), guiManager(nullptr) {}
		/// Enforces this layout by possibly changing position and size of the children guiElements
		virtual void enforceLayout() {}
	};

}