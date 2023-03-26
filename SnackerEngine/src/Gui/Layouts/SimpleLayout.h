#pragma once

#include "Gui/GuiLayout.h"

namespace SnackerEngine
{
	/// Very simple layout that positions a single child
	class SimpleLayout : public GuiLayout
	{
	public:
		/// Layout mode, decides where the child is positioned
		enum class Mode
		{
			CENTER,
			LEFT,
			TOP_LEFT,
			TOP,
			TOP_RIGHT,
			RIGHT,
			BOTTOM_RIGHT,
			BOTTOM,
			BOTTOM_LEFT,
		};
	private:
		/// Determines the mode of this SimpleLayout instance
		Mode mode;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
	public:
		/// Constructor
		SimpleLayout(Mode mode = Mode::CENTER);
		/// Copy constructor and assignment operator
		SimpleLayout(const SimpleLayout& other) noexcept;
		SimpleLayout& operator=(const SimpleLayout& other) noexcept;
		/// Move constructor and assignment operator
		SimpleLayout(SimpleLayout&& other) noexcept;
		SimpleLayout& operator=(SimpleLayout&& other) noexcept;
		/// Sets the layout mode
		void setMode(Mode mode);
		/// Returns the layout mode
		Mode getMode() const { return mode; }
	};

}