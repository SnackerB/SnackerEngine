#include "GuiEditVariable.h"

namespace SnackerEngine
{

	std::optional<int> GuiEditInt::toValue(const std::string& text)
	{
		try {
			return atoi(text.c_str());
		}
		catch (...) {
			return {};
		}
	}

	GuiEditInt::GuiEditInt(const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& editTextColor, const Color4f& labelTextBackgroundColor, const Color4f& editTextBackgroundColor, const StaticText::Alignment& editTextAlignment, const double& cursorWidth)
		: GuiEditVariable<int>(position, size, resizeMode, label, font, fontSize, labelTextColor, editTextColor, labelTextBackgroundColor, editTextBackgroundColor, editTextAlignment, cursorWidth) {}

	GuiEditInt::GuiEditInt(GuiVariableHandle2<int>& handle, const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& editTextColor, const Color4f& labelTextBackgroundColor, const Color4f& editTextBackgroundColor, const StaticText::Alignment& editTextAlignment, const double& cursorWidth)
		: GuiEditVariable<int>(handle, position, size, resizeMode, label, font, fontSize, labelTextColor, editTextColor, labelTextBackgroundColor, editTextBackgroundColor, editTextAlignment, cursorWidth) {}

	GuiEditInt::GuiEditInt(const std::string& label, GuiVariableHandle2<int>& handle, const GuiStyle& style)
		: GuiEditVariable<int>(label, handle, style) {}

	GuiEditInt::GuiEditInt(const std::string& label, const GuiStyle& style)
		: GuiEditVariable<int>(label, style) {}

	GuiEditInt::GuiEditInt(const std::string& label, const double& fontSize, GuiVariableHandle2<int>& handle, const GuiStyle& style)
		: GuiEditVariable(label, fontSize, handle, style) {}

	GuiEditInt::GuiEditInt(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiEditVariable(label, fontSize, style) {}

}