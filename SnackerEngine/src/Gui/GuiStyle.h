#pragma once

#include "Math/Vec.h"
#include "Graphics/Color.h"
#include "Gui/Text/Font.h"
#include "Gui/Text/Text.h"
#include "Gui/GuiElements/GuiTextBox.h"

#include "Gui/Layouts/ListLayout2.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	struct GuiStyle
	{
		/// Fonts and font sizes
		Font defaultFont = Font();
		double fontSizeTiny{};
		double fontSizeSmall{};
		double fontSizeNormal{};
		double fontSizeBig{};
		double fontSizeHuge{};
		/// GuiTextBox
		Vec2i guiTextBoxSize{};
		Color4f guiTextBoxTextColor{};
		Color4f guiTextBoxBackgroundColor{};
		StaticText::ParseMode guiTextBoxParseMode{};
		StaticText::Alignment guiTextBoxAlignment{};
		GuiDynamicTextBox::TextBoxMode guiTextBoxMode{};
		/// GuiTextVariable
		Vec2i guiTextVariableSize{};
		Color4f guiTextVariableTextColor{};
		Color4f guiTextVariableBackgroundColor{};
		StaticText::ParseMode guiTextVariableParseMode{};
		StaticText::Alignment guiTextVariableAlignment{};
		GuiDynamicTextBox::TextBoxMode guiTextVariableTextBoxMode{};
		/// GuiEditTextBox
		double guiEditTextBoxCursorWidth{};
		double guiEditTextBoxBlinkTime{};
		Color4f guiEditTextBoxBackgroundColor{};
		/// GuiInputVariable
		Vec2i guiInputVariableSize{};
		Color4f guiInputVariableTextColor{};
		Color4f guiInputVariableBackgroundColor{};
		StaticText::ParseMode guiInputVariableParseMode{};
		StaticText::Alignment guiInputVariableAlignment{};
		GuiDynamicTextBox::TextBoxMode guiInputVariableTextBoxMode{};
		/// GuiButton
		Vec2i guiButtonSize{};
		Color4f guiButtonTextColor{};
		Color4f guiButtonBackgroundColor{};
		Color4f guiButtonHoverColor{};
		Color4f guiButtonPressedColor{};
		Color4f guiButtonPressedHoverColor{};
		StaticText::ParseMode guiButtonParseMode{};
		StaticText::Alignment guiButtonAlignment{};
		GuiDynamicTextBox::TextBoxMode guiButtonTextBoxMode{};
		/// GuiWindow
		Vec2i guiWindowSize{};
		Color3f guiWindowBackgroundColor{};
		Color3f guiWindowResizeButtonColor{};
		double guiWindowResizeButtonSize{};
		/// ListLayout
		float listLayoutVerticalOffset{};
		float listLayoutLeftBorder{};
		float listLayoutScrollSpeed{};
		Color3f listLayoutScrollBarBackgroundColor{};
		Color3f listLayoutScrollBarColor{};
		float listLayoutScrollBarWidth{};
		float listLayoutScrollBarOffsetTop{};
		float listLayoutScrollBarOffsetBottom{};
		float listLayoutScrollBarOffsetRight{};
		ListLayout2::ListLayoutResizeMode listLayoutResizeMode{};
	};
	//--------------------------------------------------------------------------------------------------
	GuiStyle getDefaultStyle();
	//--------------------------------------------------------------------------------------------------
}