#pragma once

#include "Gui/GuiElements/GuiTextBox2.h"
#include "Gui/GuiEventHandles/GuiVariableHandle2.h"
#include "Math/Vec.h"
#include "Gui/Text/Unicode.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiTextVariable2 : public GuiDynamicTextBox2
	{
		/// The handle to the variable that is shown in this textBox
		GuiVariableHandle2<T>* variableHandle;
		/// Text that is displayed in front of the variable
		std::string label;
	protected:
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle2& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle2& guiHandle) override;
		/// Transforms the variable of the handle to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText();
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle2& guiHandle) override;
		/// Returns how the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Sets the value in the variable handle to a given value and updates the text
		void setVariable(const T& value);
		/// Returns a const reference to the value of the variable handle
		const T& getVariable() const;
	public:
		/// Constructor
		GuiTextVariable2(const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode,
			const std::string& label, const Font& font, const double& fontSize,
			Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE, const double& singleLine = false);
		/// Constructor that already registers variable handle
		GuiTextVariable2(GuiVariableHandle2<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode,
			const std::string& label, const Font& font, const double& fontSize,
			Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const TextBoxMode& textBoxMode = TextBoxMode::FORCE_SIZE, const double& singleLine = false);
		/// Constructors using GuiStyle
		GuiTextVariable2(const std::string& label, GuiVariableHandle2<T>& handle, const GuiStyle& style);
		GuiTextVariable2(const std::string& label, const GuiStyle& style);
		GuiTextVariable2(const std::string& label, const double& fontSize, GuiVariableHandle2<T>& handle, const GuiStyle& style);
		GuiTextVariable2(const std::string& label, const double& fontSize, const GuiStyle& style);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle2<T>& variableHandle);
		/// Copy constructor and assignment operator
		GuiTextVariable2(const GuiTextVariable2<T>& other) noexcept;
		GuiTextVariable2& operator=(const GuiTextVariable2<T>& other) noexcept;
		/// Move constructor and assignment operator
		GuiTextVariable2(GuiTextVariable2<T>&& other) noexcept;
		GuiTextVariable2& operator=(GuiTextVariable2<T>&& other) noexcept;
		/// Destructor
		~GuiTextVariable2();
	};

	template<typename T>
	inline void GuiTextVariable2<T>::onHandleDestruction(GuiHandle2& guiHandle)
	{
		variableHandle = nullptr;
	}

	template<typename T>
	inline void GuiTextVariable2<T>::onHandleMove(GuiHandle2& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle2<T>*>(&guiHandle);
	}

	template<typename T>
	inline std::string GuiTextVariable2<T>::toText()
	{
		return std::to_string(variableHandle->get());
	}

	template<typename T>
	inline void GuiTextVariable2<T>::onHandleUpdate(GuiHandle2& guiHandle)
	{
		setText(label + toText());
	}

	template<typename T>
	inline GuiTextVariable2<T>::IsCollidingResult GuiTextVariable2<T>::isColliding(const Vec2i& position)
	{
		return IsCollidingResult::COLLIDE_CHILD;
	}

	template<typename T>
	inline void GuiTextVariable2<T>::setVariable(const T& value)
	{
		if (variableHandle) {
			setVariableHandleValue<T>(*variableHandle, value);
			setText(label + toText());
		}
	}

	template<typename T>
	inline const T& GuiTextVariable2<T>::getVariable() const
	{
		if (variableHandle) return variableHandle->get();
		return T{};
	}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiDynamicTextBox2(position, size, resizeMode, label, font, fontSize, textColor, backgroundColor, parseMode, alignment, textBoxMode, singleLine), variableHandle(nullptr), label(label) {}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(GuiVariableHandle2<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement2::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const TextBoxMode& textBoxMode, const double& singleLine)
		: GuiTextVariable2<T>(position, size, resizeMode, label, font, fontSize, textColor, backgroundColor, parseMode, alignment, textBoxMode, singleLine)
	{
		setVariableHandle(handle);
	}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(const std::string& label, GuiVariableHandle2<T>& handle, const GuiStyle& style)
		: GuiTextVariable2<T>(handle, Vec2i(), style.guiTextVariableSize, style.guiTextVariableResizeMode, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode2, style.guiTextVariableSingleLine) {}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(const std::string& label, const GuiStyle& style)
		: GuiTextVariable2<T>(Vec2i(), style.guiTextVariableSize, style.guiTextVariableResizeMode, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode2, style.guiTextVariableSingleLine) {}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(const std::string& label, const double& fontSize, GuiVariableHandle2<T>& handle, const GuiStyle& style)
		: GuiTextVariable2<T>(handle, Vec2i(), style.guiTextVariableSize, style.guiTextVariableResizeMode, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode2, style.guiTextVariableSingleLine) {}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiTextVariable2<T>(Vec2i(), style.guiTextVariableSize, style.guiTextVariableResizeMode, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode2, style.guiTextVariableSingleLine) {}

	template<typename T>
	inline void GuiTextVariable2<T>::setVariableHandle(GuiVariableHandle2<T>& variableHandle)
	{
		if (!this->variableHandle) {
			this->variableHandle = &variableHandle;
			signUpHandle(variableHandle, 0);
			setText(label + toText());
		}
	}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(const GuiTextVariable2<T>& other) noexcept
		: GuiDynamicTextBox2(other), variableHandle(nullptr), label(other.label) {}

	template<typename T>
	inline GuiTextVariable2<T>& GuiTextVariable2<T>::operator=(const GuiTextVariable2<T>& other) noexcept
	{
		GuiDynamicTextBox2::operator=(other);
		variableHandle = nullptr;
		label = other.label;
		return *this;
	}

	template<typename T>
	inline GuiTextVariable2<T>::GuiTextVariable2(GuiTextVariable2<T>&& other) noexcept
		: GuiDynamicTextBox2(std::move(other)), variableHandle(other.variableHandle), label(other.label)
	{
		other.variableHandle = nullptr;
		if (variableHandle) notifyHandleOnGuiElementMove(*variableHandle);
	}

	template<typename T>
	inline GuiTextVariable2<T>& GuiTextVariable2<T>::operator=(GuiTextVariable2<T>&& other) noexcept
	{
		GuiDynamicTextBox2::operator=(std::move(other));
		variableHandle = other.variableHandle;
		label = other.label;
		other.variableHandle = nullptr;
		if (variableHandle) notifyHandleOnGuiElementMove(*variableHandle);
		return *this;
	}

	template<typename T>
	inline GuiTextVariable2<T>::~GuiTextVariable2()
	{
		if (variableHandle) signOffHandle(*variableHandle);
	}

}