#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiEventHandles/GuiVariableHandle.h"
#include "Math/Vec.h"
#include "Gui/Text/Unicode.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declaration of GuiStyle
	struct GuiStyle;
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiTextVariable : public GuiDynamicTextBox
	{
		/// The handle to the variable that is shown in this textBox
		GuiVariableHandle<T>* variableHandle;
		/// Text that is displayed in front of the variable
		std::string label;
	protected:
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// Transforms the variable of the handle to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText();
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle) override;
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Sets the value in the variable handle to a given value and updates the text
		void setVariable(const T& value);
		/// Returns a const reference to the value of the variable handle
		const T& getVariable() const;
	public:
		/// Constructor
		GuiTextVariable(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE);
		/// Constructor that already registers variable handle
		GuiTextVariable(GuiVariableHandle<T>& handle, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = u8"", const Font& font = Font(),
			const double& fontSize = 0.0, Color4f textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			Color4f backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE);
		/// Constructors using GuiStyle
		GuiTextVariable(const std::string& label, GuiVariableHandle<T>& handle, const GuiStyle& style);
		GuiTextVariable(const std::string& label, const GuiStyle& style);
		GuiTextVariable(const std::string& label, const double& fontSize, GuiVariableHandle<T>& handle, const GuiStyle& style);
		GuiTextVariable(const std::string& label, const double& fontSize, const GuiStyle& style);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle<T>& variableHandle);
		/// Copy constructor
		GuiTextVariable(const GuiTextVariable<T>& other) noexcept;
		/// Copy assignment operator
		GuiTextVariable& operator=(const GuiTextVariable<T>& other) noexcept;
		/// Move constructor
		GuiTextVariable(GuiTextVariable<T>&& other) noexcept;
		/// Move assignment operator
		GuiTextVariable& operator=(GuiTextVariable<T>&& other) noexcept;
		/// Destructor
		~GuiTextVariable();
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::onHandleDestruction(GuiHandle& guiHandle)
	{
		variableHandle = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle<T>*>(&guiHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::string GuiTextVariable<T>::toText()
	{
		return std::to_string(variableHandle->get());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::onHandleUpdate(GuiHandle& guiHandle)
	{
		setText(label + toText());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiElement::IsCollidingResult SnackerEngine::GuiTextVariable<T>::isColliding(const Vec2i& position)
	{
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::setVariable(const T& value)
	{
		if (variableHandle) {
			setVariableHandleValue<T>(*variableHandle, value);
			setText(label + toText());
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode)
		: GuiDynamicTextBox(position, size, label, font, fontSize, textColor, backgroundColor, parseMode, alignment, textBoxMode), variableHandle(nullptr), label(label) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(GuiVariableHandle<T>& handle, const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, Color4f textColor, Color4f backgroundColor, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode)
		: GuiTextVariable(position, size, label, font, fontSize, textColor, backgroundColor, parseMode, alignment, textBoxMode)
	{
		setVariableHandle(handle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const std::string& label, GuiVariableHandle<T>& handle, const GuiStyle& style)
		: GuiTextVariable(handle, Vec2i(), style.guiTextVariableSize, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const std::string& label, const GuiStyle& style)
		: GuiTextVariable(Vec2i(), style.guiTextVariableSize, label, style.defaultFont, style.fontSizeNormal, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const std::string& label, const double& fontSize, GuiVariableHandle<T>& handle, const GuiStyle& style)
		: GuiTextVariable(handle, Vec2i(), style.guiTextVariableSize, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiTextVariable(Vec2i(), style.guiTextVariableSize, label, style.defaultFont, fontSize, style.guiTextVariableTextColor, style.guiTextVariableBackgroundColor, style.guiTextVariableParseMode, style.guiTextVariableAlignment, style.guiTextVariableTextBoxMode) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline const T& GuiTextVariable<T>::getVariable() const
	{
		if (variableHandle) return variableHandle->get();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::setVariableHandle(GuiVariableHandle<T>& variableHandle)
	{
		if (!this->variableHandle) {
			this->variableHandle = &variableHandle;
			signUpHandle(variableHandle, 0);
			setText(label + toText());
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>& GuiTextVariable<T>::operator=(const GuiTextVariable<T>& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		variableHandle = nullptr;
		label = other.label;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>& GuiTextVariable<T>::operator=(GuiTextVariable<T>&& other) noexcept
	{
		GuiDynamicTextBox::operator=(other);
		variableHandle = other.variableHandle;
		label = other.label;
		other.variableHandle = nullptr;
		notifyHandleOnGuiElementMove(*variableHandle);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const GuiTextVariable<T>& other) noexcept
		: GuiDynamicTextBox(other), variableHandle(nullptr), label(other.label) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(GuiTextVariable<T>&& other) noexcept
		: GuiDynamicTextBox(other), variableHandle(other.variableHandle), label(other.label)
	{
		other.variableHandle = nullptr;
		notifyHandleOnGuiElementMove(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::~GuiTextVariable()
	{
		if (variableHandle) signOffHandle(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
}