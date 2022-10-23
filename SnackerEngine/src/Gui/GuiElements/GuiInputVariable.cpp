#include "Gui/GuiElements/GuiInputVariable.h"

namespace SnackerEngine
{

	std::optional<int> GuiInputInt::toValue(const std::string& text)
	{
		try {
			return atoi(text.c_str());
		}
		catch (...) {
			return {};
		}
	}

	GuiInputInt::GuiInputInt(const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& backgroundColorText, const Color4f& backgroundColorInputBox, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode, const double& cursorWidth, const double& cursorBlinkTime)
		: GuiInputVariable<int>(position, size, label, font, fontSize, textColor, backgroundColorText, backgroundColorInputBox, parseMode, alignment, textBoxMode, cursorWidth, cursorBlinkTime) {}

	GuiInputInt::GuiInputInt(GuiVariableHandle<int>& handle, const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& backgroundColorText, const Color4f& backgroundColorInputBox, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode, const double& cursorWidth, const double& cursorBlinkTime)
		: GuiInputVariable<int>(handle, position, size, label, font, fontSize, textColor, backgroundColorText, backgroundColorInputBox, parseMode, alignment, textBoxMode, cursorWidth, cursorBlinkTime) {}

	GuiInputInt::GuiInputInt(const std::string& label, GuiVariableHandle<int>& handle, const GuiStyle& style)
		: GuiInputVariable<int>(label, handle, style) {}

	GuiInputInt::GuiInputInt(const std::string& label, const GuiStyle& style)
		: GuiInputVariable<int>(label, style) {}
	
	GuiInputInt::GuiInputInt(const std::string& label, const double& fontSize, GuiVariableHandle<int>& handle, const GuiStyle& style)
		: GuiInputVariable<int>(label, fontSize, handle, style) {}
	
	GuiInputInt::GuiInputInt(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiInputVariable<int>(label, fontSize, style) {}

	std::optional<unsigned int> GuiInputUnsignedInt::toValue(const std::string& text)
	{
		try {
			auto result = atoll(text.c_str());
			if (result >= 0) return static_cast<unsigned int>(result);
			return {};
		}
		catch (...) {
			return {};
		}
	}

	GuiInputUnsignedInt::GuiInputUnsignedInt(const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& backgroundColorText, const Color4f& backgroundColorInputBox, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode, const double& cursorWidth, const double& cursorBlinkTime)
		: GuiInputVariable<unsigned int>(position, size, label, font, fontSize, textColor, backgroundColorText, backgroundColorInputBox, parseMode, alignment, textBoxMode, cursorWidth, cursorBlinkTime) {}

	GuiInputUnsignedInt::GuiInputUnsignedInt(GuiVariableHandle<unsigned int>& handle, const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& backgroundColorText, const Color4f& backgroundColorInputBox, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode, const double& cursorWidth, const double& cursorBlinkTime)
		: GuiInputVariable<unsigned int>(handle, position, size, label, font, fontSize, textColor, backgroundColorText, backgroundColorInputBox, parseMode, alignment, textBoxMode, cursorWidth, cursorBlinkTime) {}

	GuiInputUnsignedInt::GuiInputUnsignedInt(const std::string& label, GuiVariableHandle<unsigned int>& handle, const GuiStyle& style)
		: GuiInputVariable<unsigned int>(label, handle, style) {}

	GuiInputUnsignedInt::GuiInputUnsignedInt(const std::string& label, const GuiStyle& style)
		: GuiInputVariable<unsigned int>(label, style) {}

	GuiInputUnsignedInt::GuiInputUnsignedInt(const std::string& label, const double& fontSize, GuiVariableHandle<unsigned int>& handle, const GuiStyle& style)
		: GuiInputVariable<unsigned int>(label, fontSize, handle, style) {}

	GuiInputUnsignedInt::GuiInputUnsignedInt(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiInputVariable<unsigned int>(label, fontSize, style) {}

}