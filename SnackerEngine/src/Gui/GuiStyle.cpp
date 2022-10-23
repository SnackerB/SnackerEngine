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
		result.guiTextBoxSize = screenDims * 0.1;
		result.guiTextBoxTextColor = Color4f(1.0f);
		result.guiTextBoxBackgroundColor = Color4f(0.0f, 0.0f);
		result.guiTextBoxParseMode = StaticText::ParseMode::WORD_BY_WORD;
		result.guiTextBoxAlignment = StaticText::Alignment::LEFT;
		result.guiTextBoxMode = GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT;
		/// GuiTextVariable
		result.guiTextVariableSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiTextVariableTextColor = result.guiTextBoxTextColor;
		result.guiTextVariableBackgroundColor = result.guiTextBoxBackgroundColor;
		result.guiTextVariableParseMode = StaticText::ParseMode::SINGLE_LINE;
		result.guiTextVariableAlignment = StaticText::Alignment::LEFT;
		result.guiTextVariableTextBoxMode = GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT;
		/// GuiButton
		result.guiButtonSize = Vec2i(static_cast<int>(static_cast<float>(screenDims.x) * 0.25f), 0);
		result.guiButtonTextColor = Color4f(1.0f);
		result.guiButtonBackgroundColor = Color4f(0.5f, 0.5f, 0.7f, 1.0f);
		result.guiButtonHoverColor = Color4f(result.guiButtonBackgroundColor.r * 0.9f, result.guiButtonBackgroundColor.g * 0.9f, result.guiButtonBackgroundColor.b * 0.9f, 1.0f);
		result.guiButtonPressedColor = Color4f(result.guiButtonBackgroundColor.r * 0.8f, result.guiButtonBackgroundColor.g * 0.8f, result.guiButtonBackgroundColor.b * 0.8f, 1.0f);
		result.guiButtonPressedHoverColor = Color4f(result.guiButtonBackgroundColor.r * 0.7f, result.guiButtonBackgroundColor.g * 0.7f, result.guiButtonBackgroundColor.b * 0.7f, 1.0f);
		result.guiButtonParseMode = StaticText::ParseMode::WORD_BY_WORD;
		result.guiButtonAlignment = StaticText::Alignment::CENTER;
		result.guiButtonTextBoxMode = GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT;
		/// GuiWindow
		result.guiWindowSize = Vec2i(800, 1400);
		result.guiWindowBackgroundColor = Color3f(0.1f, 0.1f, 0.1f);
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
		result.listLayoutScrollBarOffsetBottom = result.guiWindowResizeButtonSize;
		result.listLayoutScrollBarOffsetRight = 20.0f;
		// Return the result
		return result;
	}

}