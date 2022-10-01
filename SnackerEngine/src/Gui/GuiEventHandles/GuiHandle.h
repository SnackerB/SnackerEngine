#pragma once

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
		/// GuiHandleID of this handle
		GuiHandleID guiHandleID;
		/// Pointer to the guiElement this handle belongs to
		GuiElement* guiElement;
	protected:
		/// Function that is called by guiElement to register handle
		void registerHandle(const GuiHandleID& guiHandleID, GuiElement& guiElement);
		/// Function that is called by guiElement when the handle is no longer needed 
		/// (e.g. on destruction of the guiElement)
		void signOff();
		/// Function that is called by the guiElement after it is moved
		void onMove(GuiElement& guiElement);
		/// Constructor
		GuiHandle();
	public:
		/// Returns the handle ID
		const GuiHandleID& getGuiHandleID() const;
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