#pragma once

#include <vector>

namespace SnackerEngine
{
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
	protected:
		/// Function that is called by guiElement to register handle
		void registerHandle(const GuiHandleID& guiHandleID, GuiElement& guiElement);
		/// Function that is called by guiElement when the handle is no longer needed 
		/// (e.g. on destruction of the guiElement)
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
	public:
		/// Returns the handle ID
		const std::vector<GuiHandleID>& getGuiHandleIDs() const;
		/// Deleted copy constructor and assignment operator
		GuiHandle(const GuiHandle& other) = delete;
		GuiHandle& operator=(const GuiHandle& other) = delete;
		/// Move constructor and assignment operator
		GuiHandle(GuiHandle&& other) noexcept;
		GuiHandle& operator=(GuiHandle&& other) noexcept;
		/// Destructor
		~GuiHandle();
	};
	//--------------------------------------------------------------------------------------------------
}