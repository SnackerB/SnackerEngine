#pragma once

#include <vector>
#include <optional>

namespace SnackerEngine
{
	/*
	//--------------------------------------------------------------------------------------------------
	/// Forward declaration of GuiElement
	class GuiElement;
	//--------------------------------------------------------------------------------------------------
	class GuiHandle
	{
		friend class GuiElement;
	public:
		/// If a guiElement has multiple handles, it can distinguish between them by their handle ID
		using GuiHandleID = unsigned int;
	private:
		/// GuiHandleIDs of this handle
		std::vector<GuiHandleID> guiHandleIDs;
		/// Pointer to the guiElements this handle belongs to
		std::vector<GuiElement*> guiElements;
	public:
		/// Returns the handle IDs
		const std::vector<GuiHandleID>& getGuiHandleIDs() const;
		/// Returns the handle ID associated with a given guiElement, if it exists
		std::optional<GuiHandleID> getHandleID(const GuiElement& guiElement);
		/// Deleted copy constructor and assignment operator
		GuiHandle(const GuiHandle& other) = delete;
		GuiHandle& operator=(const GuiHandle& other) = delete;
		/// Move constructor and assignment operator
		GuiHandle(GuiHandle&& other) noexcept;
		GuiHandle& operator=(GuiHandle&& other) noexcept;
		/// Destructor
		~GuiHandle();
	protected:
		/// Function that is called by guiElement to register handle
		void registerHandle(const GuiHandleID& guiHandleID, GuiElement& guiElement);
		/// Function that should be called if this handle needs to forget
		/// about its guiElements (eg. when it is moved from)
		void signOff();
		/// Function that is called by the guiElement after it is moved
		void onMove(GuiElement* oldElement, GuiElement* newElement);
		/// Function that can be called by the handle to notify to the guiElements
		/// that something has occured/changed!
		void onHandleUpdate();
		/// Similar to onHandleUpdate(), but notifies only elements other than the given
		/// guiElement instance.
		void onHandleUpdateFromElement(GuiElement& element);
		/// Function that should be called if a guiElement no longer needs this handle,
		/// i.e. on destruction!
		void signOff(GuiElement& element);
		/// Constructor
		GuiHandle();
	};
	//--------------------------------------------------------------------------------------------------
	*/
}