#include "Gui/GuiStyle.h"
#include "Core/Engine.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{

	GuiStyle getDefaultStyle()
	{
		GuiStyle result;
		Vec2<unsigned int> dpi = Engine::getDPI();
		Vec2i screenDims = Renderer::getNativeScreenDimensions();
		double screenDimScale = static_cast<double>(screenDims.x) / 4096.0 * 1.5f;
		/// Fonts and font sizes
		result.defaultFont = Font("fonts/Arial.ttf");
		result.fontSizeNormal = 11 * screenDimScale;
		result.fontSizeTiny = static_cast<unsigned int>(0.5 * static_cast<float>(result.fontSizeNormal));
		result.fontSizeSmall = static_cast<unsigned int>(0.75 * static_cast<float>(result.fontSizeNormal));
		result.fontSizeBig = static_cast<unsigned int>(1.5 * static_cast<float>(result.fontSizeNormal));
		result.fontSizeHuge = static_cast<unsigned int>(2.0 * static_cast<float>(result.fontSizeNormal));
		/// GuiTextBox
		result.guiTextBoxSize = Vec2i(static_cast<int>(static_cast<double>(screenDims.x) * 0.2), static_cast<int>(static_cast<double>(screenDims.y) * 0.2));
		result.guiTextBoxTextColor = Color4f(1.0f);
		result.guiTextBoxBackgroundColor = Color4f(0.0f, 0.0f);
		result.guiTextBoxParseMode = StaticText::ParseMode::WORD_BY_WORD;
		result.guiTextBoxAlignment = StaticText::Alignment::LEFT;
		result.guiTextBoxTextScaleMode = GuiDynamicTextBox::TextScaleMode::DONT_SCALE;
		result.guiTextBoxSizeHintModes = { GuiDynamicTextBox::SizeHintMode::ARBITRARY, GuiDynamicTextBox::SizeHintMode::ARBITRARY, GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_SIZE };
		result.guiTextBoxBorder = 10 * screenDimScale;
		result.guiTextBoxResizeMode = GuiElement::ResizeMode::DO_NOT_RESIZE;
		result.guiButtonDoRecomputeOnSizeChange = true;
		/// GuiTextVariable
		result.guiTextVariableSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiTextVariableTextColor = result.guiTextBoxTextColor;
		result.guiTextVariableBackgroundColor = result.guiTextBoxBackgroundColor;
		result.guiTextVariableParseMode = StaticText::ParseMode::SINGLE_LINE;
		result.guiTextVariableAlignment = StaticText::Alignment::LEFT;
		result.guiTextVariableTextScaleMode = GuiDynamicTextBox::TextScaleMode::DONT_SCALE;
		result.guiTextVariableSizeHintModes = { GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT, GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT , GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT };
		result.guiTextVariableBorder = result.guiTextBoxBorder;
		result.guiTextVariableResizeMode = GuiElement::ResizeMode::DO_NOT_RESIZE;
		/// GuiEditTextBox
		result.guiEditTextBoxCursorWidth = 0.1; // TODO
		result.guiEditTextBoxBlinkTime = 0.5;
		result.guiEditTextBoxBackgroundColor = Color4f(0.0f, 1.0f);
		result.guiEditTextBoxSelectionBoxColor = Color4f(0.5f, 0.5f, 0.5f, 1.0f);
		result.guiEditTextBoxTextScaleMode = GuiEditTextBox::TextScaleMode::RECOMPUTE_DOWN;
		result.guiEditTextBoxSizeHintModes = {GuiEditTextBox::SizeHintMode::ARBITRARY, GuiEditTextBox::SizeHintMode::ARBITRARY , GuiEditTextBox::SizeHintMode::ARBITRARY };
		result.guiEditTextBoxBorder = result.guiTextBoxBorder;
		/// GuiEditVariable
		result.guiEditVariableSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiEditVariableResizeMode = GuiElement::ResizeMode::RESIZE_RANGE;
		result.guiEditVariableEditTextAlignment = StaticText::Alignment::CENTER;
		/// GuiButton
		result.guiButtonSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.3f), 0);
		result.guiButtonTextColor = Color4f(1.0f);
		result.guiButtonBackgroundColor = Color4f(0.5f, 0.5f, 0.7f, 1.0f);
		result.guiButtonHoverColor = Color4f(result.guiButtonBackgroundColor.r * 0.9f, result.guiButtonBackgroundColor.g * 0.9f, result.guiButtonBackgroundColor.b * 0.9f, 1.0f);
		result.guiButtonPressedColor = Color4f(result.guiButtonBackgroundColor.r * 0.8f, result.guiButtonBackgroundColor.g * 0.8f, result.guiButtonBackgroundColor.b * 0.8f, 1.0f);
		result.guiButtonPressedHoverColor = Color4f(result.guiButtonBackgroundColor.r * 0.7f, result.guiButtonBackgroundColor.g * 0.7f, result.guiButtonBackgroundColor.b * 0.7f, 1.0f);
		result.guiButtonLockedColor = Color4f(0.5f, 0.5f, 0.5f, 1.0f);
		result.guiButtonParseMode = StaticText::ParseMode::WORD_BY_WORD;
		result.guiButtonAlignment = StaticText::Alignment::CENTER;
		result.guiButtonResizeMode = GuiButton::ResizeMode::RESIZE_RANGE;
		result.guiButtonTextScaleMode = GuiButton::TextScaleMode::DONT_SCALE;
		result.guiButtonSizeHintModes = {GuiButton::SizeHintMode::SET_TO_TEXT_SIZE, GuiButton::SizeHintMode::SET_TO_TEXT_HEIGHT, GuiButton::SizeHintMode::SET_TO_TEXT_SIZE};
		result.guiButtonBorder = result.guiTextBoxBorder;
		result.guiButtonDoRecomputeOnSizeChange = false;
		/// GuiWindow
		result.guiWindowSize = Vec2i(800, 900);
		result.guiWindowBackgroundColor = Color3f(0.2f, 0.2f, 0.2f);
		result.guiWindowResizeButtonColor = Color3f::fromColor256(Color3<unsigned char>(23, 70, 162));
		result.guiWindowResizeButtonSize = screenDims.y * 0.04;
		/// ScrollingListLayout
		result.verticalScrollingListLayoutVerticalOffset = result.guiTextBoxBorder;
		result.verticalScrollingListLayoutLeftBorder = result.guiTextBoxBorder;
		result.verticalScrollingListLayoutScrollSpeed = 20.0f;
		result.verticalScrollingListLayoutScrollBarBackgroundColor = Color3f(0.07f);
		result.verticalScrollingListLayoutScrollBarColor = result.guiWindowResizeButtonColor;
		result.verticalScrollingListLayoutScrollBarWidth = 30 * screenDimScale;
		result.verticalScrollingListLayoutScrollBarOffsetTop = result.guiTextBoxBorder;
		result.verticalScrollingListLayoutScrollBarOffsetBottom = result.guiTextBoxBorder;
		result.verticalScrollingListLayoutScrollBarOffsetRight = result.guiTextBoxBorder;
		/// Slider
		result.guiSliderSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiSliderResizeMode = GuiElement::ResizeMode::RESIZE_RANGE;
		result.guiSliderLabelTextColor = Color4f(1.0f);
		result.guiSliderLabelBackgroundColor = Color4f(0.0f, 0.0f);
		result.guiSliderVariableBoxTextColor = Color4f(1.0f);
		result.guiSliderVariableBoxBackgroundColor = Color4f(0.0f, 1.0f);
		result.guiSliderButtonColor = Color4f(1.0f, 0.0f, 0.0f, 0.5f);
		result.guiSliderButtonWidth = 5 * result.guiTextBoxBorder;
		/// GuiImage
		result.guiImageBackgroundColor = Color4f(0.0f, 1.0f);
		result.guiImageGuiImageMode = GuiImage::GuiImageMode::FIT_IMAGE_TO_SIZE;
		/// GuiCheckBox
		result.checkBoxButtonDefaultColorTrue = Color4f(0.0f, 1.0f, 0.0f, 1.0f);
		result.checkBoxButtonHoverColorTrue = Color4f(result.checkBoxButtonDefaultColorTrue.r * 0.9, result.checkBoxButtonDefaultColorTrue.g * 0.9, result.checkBoxButtonDefaultColorTrue.b * 0.9, 1.0);
		result.checkBoxButtonPressedColorTrue = Color4f(result.checkBoxButtonDefaultColorTrue.r * 0.8, result.checkBoxButtonDefaultColorTrue.g * 0.8, result.checkBoxButtonDefaultColorTrue.b * 0.8, 1.0);
		result.checkBoxButtonPressedHoverColorTrue = Color4f(result.checkBoxButtonDefaultColorTrue.r * 0.7, result.checkBoxButtonDefaultColorTrue.g * 0.7, result.checkBoxButtonDefaultColorTrue.b * 0.7, 1.0);
		result.checkBoxButtonDefaultColorFalse = Color4f(1.0f, 0.0f, 0.0f, 1.0f);
		result.checkBoxButtonHoverColorFalse = Color4f(result.checkBoxButtonDefaultColorFalse.r * 0.9, result.checkBoxButtonDefaultColorFalse.g * 0.9, result.checkBoxButtonDefaultColorFalse.b * 0.9, 1.0);
		result.checkBoxButtonPressedColorFalse = Color4f(result.checkBoxButtonDefaultColorFalse.r * 0.8, result.checkBoxButtonDefaultColorFalse.g * 0.8, result.checkBoxButtonDefaultColorFalse.b * 0.8, 1.0);
		result.checkBoxButtonPressedHoverColorFalse = Color4f(result.checkBoxButtonDefaultColorFalse.r * 0.7, result.checkBoxButtonDefaultColorFalse.g * 0.7, result.checkBoxButtonDefaultColorFalse.b * 0.7, 1.0);
		// Return the result
		return result;
	}

}