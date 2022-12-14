#pragma once

#include "Gui/GuiElement.h"

namespace SnackerEngine
{

	class GuiLayout : public GuiElement
	{
		/// Friend declarations
		friend class GuiManager;
		friend class GuiElement;
	public:
		/// Default constructor
		GuiLayout();
		/// Destructor
		~GuiLayout();
		/// Copy constructor and assignment operator
		GuiLayout(const GuiLayout& other) noexcept;
		GuiLayout& operator=(const GuiLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiLayout(GuiLayout&& other) noexcept;
		GuiLayout& operator=(GuiLayout&& other) noexcept;
	};

}