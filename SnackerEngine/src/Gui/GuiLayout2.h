#pragma once

#include "Gui/GuiElement2.h"

namespace SnackerEngine
{

	class GuiLayout2 : public GuiElement2
	{
		/// Friend declarations
		friend class GuiManager2;
		friend class GuiElement2;
	public:
		/// Default constructor
		GuiLayout2();
		/// Destructor
		~GuiLayout2();
		/// Copy constructor and assignment operator
		GuiLayout2(const GuiLayout2& other) noexcept;
		GuiLayout2& operator=(const GuiLayout2& other) noexcept;
		/// Move constructor and assignment operator
		GuiLayout2(GuiLayout2&& other) noexcept;
		GuiLayout2& operator=(GuiLayout2&& other) noexcept;
	};

}