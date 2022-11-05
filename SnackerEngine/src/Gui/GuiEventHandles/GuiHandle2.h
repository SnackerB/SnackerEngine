#pragma once

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declaration of GuiElement
	class GuiElement2;
	//--------------------------------------------------------------------------------------------------
	class GuiHandle2
	{
		friend class GuiElement2;
	public:
		/// If a guiElement has multiple handles, it can distinguish between them by their handle ID
		using GuiHandleID = unsigned int;
	private:
		/// GuiHandleID of this handle
		GuiHandleID guiHandleID;
		/// Pointer to the guiElement this handle belongs to
		GuiElement2* guiElement;
	protected:
		/// Function that is called by guiElement to register handle
		void registerHandle(const GuiHandleID& guiHandleID, GuiElement2& guiElement);
		/// Function that is called by guiElement when the handle is no longer needed 
		/// (e.g. on destruction of the guiElement)
		void signOff();
		/// Function that is called by the guiElement after it is moved
		void onMove(GuiElement2& guiElement);
		/// Function that can be called by the handle to notify to the guiElement 
		/// that something has occured/changed!
		void onHandleUpdate();
		/// Constructor
		GuiHandle2();
	public:
		/// Returns the handle ID
		const GuiHandleID& getGuiHandleID() const;
		/// Deleted copy constructor and assignment operator
		GuiHandle2(const GuiHandle2& other) = delete;
		GuiHandle2& operator=(const GuiHandle2& other) = delete;
		/// Move constructor and assignment operator
		GuiHandle2(GuiHandle2&& other) noexcept;
		GuiHandle2& operator=(GuiHandle2&& other) noexcept;
		/// Destructor
		~GuiHandle2();
	};
	//--------------------------------------------------------------------------------------------------
}