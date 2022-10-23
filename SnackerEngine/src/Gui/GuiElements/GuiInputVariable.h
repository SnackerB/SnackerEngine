#pragma once

#include "Gui/GuiElement.h"
#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiStyle.h"
#include "Gui/GuiManager.h"

#include <memory>
#include <optional>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Forward declaration of GuiStyle
	struct GuiStyle;
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiInputVariable : public GuiElement
	{
	protected:
		/// class for a eventHandle that notifies the GuiInputVariable when the text is edited
		class GuiInputVariableEventHandle : public GuiEventHandle
		{
			GuiInputVariable<T>* inputVariable;
			friend class GuiInputVariable<T>;
		protected:
			/// Function that is called when the event happens. Can be overwritten
			virtual void onEvent() override
			{
				inputVariable->updateVariableAndTextBox();
				reset();
			}
			/// Constructor
			GuiInputVariableEventHandle(GuiInputVariable<T>& inputVariable)
				: GuiEventHandle(), inputVariable(&inputVariable) {}
			/// Should be called by the GuiInputVariable after it moved
			void onMove(GuiInputVariable<T>& inputVariable)
			{
				this->inputVariable = &inputVariable;
			}
		};
		/// The handle to the variable
		GuiVariableHandle<T>* variableHandle;
		/// Handle that gets activated when the text is edited
		GuiInputVariableEventHandle inputVariableEventHandle;
		/// Text that is displayed in front of the variable
		GuiDynamicTextBox labelText;
		/// Atual input box
		GuiEditTextBox inputTextBox;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// Transforms the variable of the handle to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText();
		/// Tries to read in the given string and return the corresponding value. On failure an empty optional is returned.
		virtual std::optional<T> toValue(const std::string& text);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle) override;
		/// This function is called by the guiManager after registering this guiInteractable object.
		/// When this function is called, the guiManager pointer is set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Returns true if the given position vector (relative to the top left corner of the parent element)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& position) override;
		/// Sets the value in the variable handle to a given value and updates the text
		void setVariable(const T& value);
		/// Returns a const reference to the value of the variable handle
		const T& getVariable() const;
		/// DEBUG. TODO: REMOVE
		virtual void draw(const Vec2i& parentPosition) override;
		/// Updates the variable using the text of the inputTextBox
		void updateVariableAndTextBox();
	public:
		/// Constructor
		GuiInputVariable(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			const Color4f& backgroundColorText = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& backgroundColorInputBox = { 0.0f, 0.0f, 0.0f, 1.0f},
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE,
			const double& cursorWidth = 0.0, const double& cursorBlinkTime = 0.5);
		/// Constructor that already registers variable handle
		GuiInputVariable(GuiVariableHandle<T>& handle, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			const Color4f& backgroundColorText = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& backgroundColorInputBox = { 0.0f, 0.0f, 0.0f, 1.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE,
			const double& cursorWidth = 0.0, const double& cursorBlinkTime = 0.5);
		/// Constructors using GuiStyle
		GuiInputVariable(const std::string& label, GuiVariableHandle<T>& handle, const GuiStyle& style);
		GuiInputVariable(const std::string& label, const GuiStyle& style);
		GuiInputVariable(const std::string& label, const double& fontSize, GuiVariableHandle<T>& handle, const GuiStyle& style);
		GuiInputVariable(const std::string& label, const double& fontSize, const GuiStyle& style);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle<T>& variableHandle);
		/// Copy constructor
		GuiInputVariable(const GuiInputVariable<T>& other) noexcept;
		/// Copy assignment operator
		GuiInputVariable& operator=(const GuiInputVariable<T>& other) noexcept;
		/// Move constructor
		GuiInputVariable(GuiInputVariable<T>&& other) noexcept;
		/// Move assignment operator
		GuiInputVariable& operator=(GuiInputVariable<T>&& other) noexcept;
		/// Destructor
		~GuiInputVariable();
	};

	template<typename T>
	inline void GuiInputVariable<T>::onHandleDestruction(GuiHandle& guiHandle)
	{
		variableHandle = nullptr;
	}

	template<typename T>
	inline void GuiInputVariable<T>::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle<T>*>(&guiHandle);
	}

	template<typename T>
	inline std::string GuiInputVariable<T>::toText()
	{
		return std::to_string(variableHandle->get());
	}

	template<typename T>
	inline std::optional<T> GuiInputVariable<T>::toValue(const std::string& text)
	{
		return {};
	}

	template<typename T>
	inline void GuiInputVariable<T>::onHandleUpdate(GuiHandle& guiHandle)
	{
		if (!inputTextBox.isActive()) inputTextBox.setText(toText());
	}

	template<typename T>
	inline void GuiInputVariable<T>::onRegister()
	{
		GuiElement::onRegister();
		guiManager->registerElement(*this, labelText);
		guiManager->registerElement(*this, inputTextBox);
		setSize(Vec2i(size.x, labelText.getSize().y));
	}

	template<typename T>
	inline GuiInputVariable<T>::IsCollidingResult GuiInputVariable<T>::isColliding(const Vec2i& position)
	{
		return (position.x > this->position.x && position.x < this->position.x + size.x
			&& position.y > this->position.y && position.y < this->position.y + size.y) ?
			IsCollidingResult::WEAK_COLLIDING : IsCollidingResult::NOT_COLLIDING;
	}

	template<typename T>
	inline void GuiInputVariable<T>::setVariable(const T& value)
	{
		if (variableHandle) {
			setVariableHandleValue<T>(*variableHandle, value);
			inputTextBox.setText(toText());
		}
	}

	template<typename T>
	inline const T& GuiInputVariable<T>::getVariable() const
	{
		if (variableHandle) return variableHandle->get();
		return T{};
	}

	template<typename T>
	inline void GuiInputVariable<T>::draw(const Vec2i& parentPosition)
	{
		GuiElement::draw(parentPosition);
	}

	template<typename T>
	inline void GuiInputVariable<T>::updateVariableAndTextBox()
	{
		if (!variableHandle) return;
		auto value = toValue(inputTextBox.getText());
		if (value) {
			setVariable(value.value());
			inputTextBox.setText(toText());
		}
		else {
			inputTextBox.setText(toText());
		}
	}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& backgroundColorText, const Color4f& backgroundColorInputBox, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode, const double& cursorWidth, const double& cursorBlinkTime)
		: GuiElement(position, size), variableHandle(nullptr), inputVariableEventHandle(*this), labelText(position, size, label, font, fontSize, textColor, backgroundColorText, parseMode, alignment, GuiDynamicTextBox::TextBoxMode::SHRINK_TO_FIT, true),
		inputTextBox(Vec2i(position.x + labelText.getSize().x, position.y), Vec2i(std::max(0, size.x - (position.x + labelText.getSize().x)), size.y), "", font, fontSize, cursorWidth, textColor, backgroundColorInputBox, parseMode, alignment, GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT, true, cursorBlinkTime) 
	{
		inputTextBox.setEventHandleTextWasEdited(inputVariableEventHandle);
		setSize(Vec2i(size.x, labelText.getSize().y));
	}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(GuiVariableHandle<T>& handle, const Vec2i& position, const Vec2i& size, const std::string& label, const Font& font, const double& fontSize, const Color4f& textColor, const Color4f& backgroundColorText, const Color4f& backgroundColorInputBox, const StaticText::ParseMode& parseMode, const StaticText::Alignment& alignment, const GuiDynamicTextBox::TextBoxMode& textBoxMode, const double& cursorWidth, const double& cursorBlinkTime)
		: GuiInputVariable<T>(position, size, label, font, fontSize, textColor, backgroundColorText, backgroundColorInputBox, parseMode, alignment, textBoxMode, cursorWidth, cursorBlinkTime)
	{
		setVariableHandle(handle);
	}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(const std::string& label, GuiVariableHandle<T>& handle, const GuiStyle& style)
		: GuiInputVariable<T>(handle, Vec2i(0, 0), style.guiInputVariableSize, label, style.defaultFont, style.fontSizeNormal, style.guiTextBoxTextColor, style.guiInputVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiInputVariableParseMode, style.guiInputVariableAlignment, style.guiInputVariableTextBoxMode, style.guiEditTextBoxCursorWidth, style.guiEditTextBoxBlinkTime) {}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(const std::string& label, const GuiStyle& style)
		: GuiInputVariable<T>(Vec2i(0, 0), style.guiInputVariableSize, label, style.defaultFont, style.fontSizeNormal, style.guiTextBoxTextColor, style.guiInputVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiInputVariableParseMode, style.guiInputVariableAlignment, style.guiInputVariableTextBoxMode, style.guiEditTextBoxCursorWidth, style.guiEditTextBoxBlinkTime) {}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(const std::string& label, const double& fontSize, GuiVariableHandle<T>& handle, const GuiStyle& style)
		: GuiInputVariable<T>(handle, Vec2i(0, 0), style.guiInputVariableSize, label, style.defaultFont, fontSize, style.guiTextBoxTextColor, style.guiInputVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiInputVariableParseMode, style.guiInputVariableAlignment, style.guiInputVariableTextBoxMode, style.guiEditTextBoxCursorWidth, style.guiEditTextBoxBlinkTime) {}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiInputVariable<T>(Vec2i(0, 0), style.guiInputVariableSize, label, style.defaultFont, fontSize, style.guiTextBoxTextColor, style.guiInputVariableBackgroundColor, style.guiEditTextBoxBackgroundColor, style.guiInputVariableParseMode, style.guiInputVariableAlignment, style.guiInputVariableTextBoxMode, style.guiEditTextBoxCursorWidth, style.guiEditTextBoxBlinkTime) {}

	template<typename T>
	inline void GuiInputVariable<T>::setVariableHandle(GuiVariableHandle<T>& variableHandle)
	{
		if (!this->variableHandle) {
			this->variableHandle = &variableHandle;
			signUpHandle(variableHandle, 0);
			inputTextBox.setText(toText());
		}
	}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(const GuiInputVariable<T>& other) noexcept
		: GuiElement(other), variableHandle(nullptr), inputVariableEventHandle(*this), labelText(other.labelText), inputTextBox(other.inputTextBox) 
	{
		inputTextBox.setEventHandleTextWasEdited(inputVariableEventHandle);
	}

	template<typename T>
	inline GuiInputVariable<T>& GuiInputVariable<T>::operator=(const GuiInputVariable<T>& other) noexcept
	{
		GuiElement::operator=(other);
		variableHandle = nullptr;
		inputVariableEventHandle = GuiInputVariableEventHandle(*this);
		labelText = other.labelText;
		inputTextBox = other.inputTextBox;
		inputTextBox.setEventHandleTextWasEdited(inputVariableEventHandle);
		return *this;
	}

	template<typename T>
	inline GuiInputVariable<T>::GuiInputVariable(GuiInputVariable<T>&& other) noexcept
		: GuiElement(std::move(other)), variableHandle(std::move(other.variableHandle)), inputVariableEventHandle(std::move(other.inputVariableEventHandle)), labelText(std::move(other.labelText)), inputTextBox(std::move(other.inputTextBox))
	{
		other.variableHandle = nullptr;
		notifyHandleOnGuiElementMove(*variableHandle);
		inputVariableEventHandle.onMove(*this);
	}

	template<typename T>
	inline GuiInputVariable<T>& GuiInputVariable<T>::operator=(GuiInputVariable<T>&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		variableHandle = std::move(other.variableHandle);
		inputVariableEventHandle = std::move(other.inputVariableEventHandle);
		labelText = std::move(other.labelText);
		inputTextBox = std::move(other.inputTextBox);
		other.variableHandle = nullptr;
		notifyHandleOnGuiElementMove(*variableHandle);
		inputVariableEventHandle.onMove(*this);
		return *this;
	}

	template<typename T>
	inline GuiInputVariable<T>::~GuiInputVariable()
	{
		if (variableHandle) signOffHandle(*variableHandle);
	}

	class GuiInputInt : public GuiInputVariable<int>
	{
	protected:
		/// Tries to read in the given string and return the corresponding value. On failure an empty optional is returned.
		virtual std::optional<int> toValue(const std::string& text) override;
	public:
		/// Constructor
		GuiInputInt(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			const Color4f& backgroundColorText = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& backgroundColorInputBox = { 0.0f, 0.0f, 0.0f, 1.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE,
			const double& cursorWidth = 0.0, const double& cursorBlinkTime = 0.5);
		/// Constructor that already registers variable handle
		GuiInputInt(GuiVariableHandle<int>& handle, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			const Color4f& backgroundColorText = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& backgroundColorInputBox = { 0.0f, 0.0f, 0.0f, 1.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE,
			const double& cursorWidth = 0.0, const double& cursorBlinkTime = 0.5);
		/// Constructors using GuiStyle
		GuiInputInt(const std::string& label, GuiVariableHandle<int>& handle, const GuiStyle& style);
		GuiInputInt(const std::string& label, const GuiStyle& style);
		GuiInputInt(const std::string& label, const double& fontSize, GuiVariableHandle<int>& handle, const GuiStyle& style);
		GuiInputInt(const std::string& label, const double& fontSize, const GuiStyle& style);
	};

	class GuiInputUnsignedInt : public GuiInputVariable<unsigned int>
	{
	protected:
		/// Tries to read in the given string and return the corresponding value. On failure an empty optional is returned.
		virtual std::optional<unsigned int> toValue(const std::string& text) override;
	public:
		/// Constructor
		GuiInputUnsignedInt(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			const Color4f& backgroundColorText = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& backgroundColorInputBox = { 0.0f, 0.0f, 0.0f, 1.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE,
			const double& cursorWidth = 0.0, const double& cursorBlinkTime = 0.5);
		/// Constructor that already registers variable handle
		GuiInputUnsignedInt(GuiVariableHandle<unsigned int>& handle, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& label = "", const Font& font = Font(),
			const double& fontSize = 0.0, const Color4f& textColor = { 1.0f, 1.0f, 1.0f, 0.0f },
			const Color4f& backgroundColorText = { 0.0f, 0.0f, 0.0f, 0.0f }, const Color4f& backgroundColorInputBox = { 0.0f, 0.0f, 0.0f, 1.0f },
			const StaticText::ParseMode& parseMode = StaticText::ParseMode::WORD_BY_WORD, const StaticText::Alignment& alignment = StaticText::Alignment::LEFT,
			const GuiDynamicTextBox::TextBoxMode& textBoxMode = GuiDynamicTextBox::TextBoxMode::FORCE_SIZE,
			const double& cursorWidth = 0.0, const double& cursorBlinkTime = 0.5);
		/// Constructors using GuiStyle
		GuiInputUnsignedInt(const std::string& label, GuiVariableHandle<unsigned int>& handle, const GuiStyle& style);
		GuiInputUnsignedInt(const std::string& label, const GuiStyle& style);
		GuiInputUnsignedInt(const std::string& label, const double& fontSize, GuiVariableHandle<unsigned int>& handle, const GuiStyle& style);
		GuiInputUnsignedInt(const std::string& label, const double& fontSize, const GuiStyle& style);
	};

}