#pragma once

#include "Math/Vec.h"
#include "Graphics/Color.h"
#include "Gui/Text/Font.h"
#include "Gui/Text/Text.h"
#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiElements/GuiImage.h"

#include "Gui/Layouts/ListLayout.h"

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
		GuiDynamicTextBox::TextScaleMode guiTextBoxTextScaleMode{};
		GuiDynamicTextBox::SizeHintModes guiTextBoxSizeHintModes{};
		int guiTextBoxBorder{};
		GuiElement::ResizeMode guiTextBoxResizeMode{};
		/// GuiTextVariable
		Vec2i guiTextVariableSize{};
		Color4f guiTextVariableTextColor{};
		Color4f guiTextVariableBackgroundColor{};
		StaticText::ParseMode guiTextVariableParseMode{};
		StaticText::Alignment guiTextVariableAlignment{};
		GuiDynamicTextBox::TextScaleMode guiTextVariableTextScaleMode{};
		GuiDynamicTextBox::SizeHintModes guiTextVariableSizeHintModes{};
		int guiTextVariableBorder{};
		GuiElement::ResizeMode guiTextVariableResizeMode{};
		/// GuiEditTextBox
		double guiEditTextBoxCursorWidth{};
		double guiEditTextBoxBlinkTime{};
		Color4f guiEditTextBoxBackgroundColor{};
		Color4f guiEditTextBoxSelectionBoxColor{};
		GuiDynamicTextBox::TextScaleMode guiEditTextBoxTextScaleMode{};
		GuiDynamicTextBox::SizeHintModes guiEditTextBoxSizeHintModes{};
		int guiEditTextBoxBorder{};
		/// GuiEditVariable
		Vec2i guiEditVariableSize{};
		GuiElement::ResizeMode guiEditVariableResizeMode{};
		StaticText::Alignment guiEditVariableEditTextAlignment{};
		/// GuiButton
		Vec2i guiButtonSize{};
		Color4f guiButtonTextColor{};
		Color4f guiButtonBackgroundColor{};
		Color4f guiButtonHoverColor{};
		Color4f guiButtonPressedColor{};
		Color4f guiButtonPressedHoverColor{};
		StaticText::ParseMode guiButtonParseMode{};
		StaticText::Alignment guiButtonAlignment{};
		GuiElement::ResizeMode guiButtonResizeMode{};
		GuiButton::TextScaleMode guiButtonTextScaleMode{};
		GuiButton::SizeHintModes guiButtonSizeHintModes{};
		int guiButtonBorder{};
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
		ListLayout::ListLayoutResizeMode listLayoutResizeMode{};
		/// Slider
		Vec2i guiSliderSize{}; 
		GuiElement::ResizeMode guiSliderResizeMode{};
		Color4f guiSliderLabelTextColor{};
		Color4f guiSliderLabelBackgroundColor{};
		Color4f guiSliderVariableBoxTextColor{};
		Color4f guiSliderVariableBoxBackgroundColor{};
		Color4f guiSliderButtonColor{}; 
		double guiSliderButtonWidth{};
		/// GuiImage
		Color4f guiImageBackgroundColor{};
		GuiImage::GuiImageMode guiImageGuiImageMode{};
	};
	//--------------------------------------------------------------------------------------------------
	GuiStyle getDefaultStyle();
	//--------------------------------------------------------------------------------------------------
}