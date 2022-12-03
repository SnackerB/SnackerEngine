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
		/// Fonts and font sizes
		result.defaultFont = Font("fonts/Arial.ttf");
		result.fontSizeNormal = 16;
		result.fontSizeTiny = static_cast<unsigned int>(0.5 * static_cast<float>(result.fontSizeNormal));
		result.fontSizeSmall = static_cast<unsigned int>(0.75 * static_cast<float>(result.fontSizeNormal));
		result.fontSizeBig = static_cast<unsigned int>(1.5 * static_cast<float>(result.fontSizeNormal));
		result.fontSizeHuge = 2 * result.fontSizeNormal;
		/// GuiTextBox
		result.guiTextBoxSize = Vec2i(static_cast<int>(static_cast<double>(screenDims.x) * 0.2), static_cast<int>(static_cast<double>(screenDims.y) * 0.2));
		result.guiTextBoxTextColor = Color4f(1.0f);
		result.guiTextBoxBackgroundColor = Color4f(0.0f, 0.0f);
		result.guiTextBoxParseMode = StaticText::ParseMode::WORD_BY_WORD;
		result.guiTextBoxAlignment = StaticText::Alignment::LEFT;
		result.guiTextBoxMode2 = GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT;
		result.guiTextBoxResizeMode = GuiElement::ResizeMode::DO_NOT_RESIZE;
		result.guiTextBoxSingleLine = true;
		/// GuiTextVariable
		result.guiTextVariableSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiTextVariableTextColor = result.guiTextBoxTextColor;
		result.guiTextVariableBackgroundColor = result.guiTextBoxBackgroundColor;
		result.guiTextVariableParseMode = StaticText::ParseMode::SINGLE_LINE;
		result.guiTextVariableAlignment = StaticText::Alignment::LEFT;
		result.guiTextVariableTextBoxMode2 = GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT;
		result.guiTextVariableSingleLine = true;
		result.guiTextVariableResizeMode = GuiElement::ResizeMode::DO_NOT_RESIZE;
		/// GuiEditTextBox
		result.guiEditTextBoxCursorWidth = 0.1; // TODO
		result.guiEditTextBoxBlinkTime = 0.5;
		result.guiEditTextBoxBackgroundColor = Color4f(0.0f, 1.0f);
		result.guiEditTextBoxSingleLine = true;
		/// GuiInputVariable
		//result.guiInputVariableSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		//result.guiInputVariableTextColor = result.guiTextBoxTextColor;
		//result.guiInputVariableBackgroundColor = result.guiTextBoxBackgroundColor;
		//result.guiInputVariableParseMode = StaticText::ParseMode::SINGLE_LINE;
		//result.guiInputVariableAlignment = StaticText::Alignment::CENTER;
		/// GuiEditVariable
		result.guiEditVariableSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiEditVariableResizeMode = GuiElement::ResizeMode::RESIZE_RANGE;
		result.guiEditVariableEditTextAlignment = StaticText::Alignment::CENTER;
		/// GuiButton
		result.guiButtonSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiButtonTextColor = Color4f(1.0f);
		result.guiButtonBackgroundColor = Color4f(0.5f, 0.5f, 0.7f, 1.0f);
		result.guiButtonHoverColor = Color4f(result.guiButtonBackgroundColor.r * 0.9f, result.guiButtonBackgroundColor.g * 0.9f, result.guiButtonBackgroundColor.b * 0.9f, 1.0f);
		result.guiButtonPressedColor = Color4f(result.guiButtonBackgroundColor.r * 0.8f, result.guiButtonBackgroundColor.g * 0.8f, result.guiButtonBackgroundColor.b * 0.8f, 1.0f);
		result.guiButtonPressedHoverColor = Color4f(result.guiButtonBackgroundColor.r * 0.7f, result.guiButtonBackgroundColor.g * 0.7f, result.guiButtonBackgroundColor.b * 0.7f, 1.0f);
		result.guiButtonParseMode = StaticText::ParseMode::WORD_BY_WORD;
		result.guiButtonAlignment = StaticText::Alignment::CENTER;
		result.guiButtonResizeMode = GuiButton::ResizeMode::RESIZE_RANGE;
		result.guiButtonTextBoxMode2 = GuiButton::TextBoxMode::SHRINK_HEIGHT_TO_FIT;
		result.guiButtonSingleLine = false;
		/// GuiWindow
		result.guiWindowSize = Vec2i(800, 900);
		result.guiWindowBackgroundColor = Color3f(0.2f, 0.2f, 0.2f);
		result.guiWindowResizeButtonColor = Color3f::fromColor256(Color3<unsigned char>(23, 70, 162));
		result.guiWindowResizeButtonSize = screenDims.y * 0.04;
		/// ListLayout
		result.listLayoutVerticalOffset = 20.0f;
		result.listLayoutLeftBorder = 20.0f;
		result.listLayoutScrollSpeed = 20.0f;
		result.listLayoutScrollBarBackgroundColor = Color3f(0.07f);
		result.listLayoutScrollBarColor = result.guiWindowResizeButtonColor;
		result.listLayoutScrollBarWidth = 25.0f;
		result.listLayoutScrollBarOffsetTop = 0.0f;
		result.listLayoutScrollBarOffsetBottom = 20.0f;
		result.listLayoutScrollBarOffsetRight = 20.0f;
		result.listLayoutResizeMode = ListLayout::ListLayoutResizeMode::RESIZE_WIDTH_IF_POSSIBLE;
		/// Slider
		result.guiSliderSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiSliderResizeMode = GuiElement::ResizeMode::RESIZE_RANGE;
		result.guiSliderLabelTextColor = Color4f(1.0f);
		result.guiSliderLabelBackgroundColor = Color4f(0.0f, 0.0f);
		result.guiSliderVariableBoxTextColor = Color4f(1.0f);
		result.guiSliderVariableBoxBackgroundColor = Color4f(0.0f, 1.0f);
		result.guiSliderButtonColor = Color4f(1.0f, 0.0f, 0.0f, 0.5f);
		result.guiSliderButtonWidth = 10.0;
		/// GuiImage
		result.guiImageBackgroundColor = Color4f(0.0f, 1.0f);
		result.guiImageGuiImageMode = GuiImage::GuiImageMode::FIT_IMAGE_TO_SIZE;
		// Return the result
		return result;
	}

}