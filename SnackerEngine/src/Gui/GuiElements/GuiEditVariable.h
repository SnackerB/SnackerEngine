#pragma once

#include "Gui\GuiElements\GuiEditBox.h"
#include "Utility\Formatting.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiEditVariable : public GuiEditBox
	{
	private:
		/// The handle to the variable that is shown in this textBox
		GuiVariableHandle<T>* variableHandle = nullptr;
		/// The current value
		T value{};
		/// The formatter used to format the text
		std::unique_ptr<Formatter<T>> formatter = nullptr;
		/// Helper function that updates the text
		void updateText();
		/// Helper function that updates the value
		void updateValue();
	protected:
		/// Gets called when, while in editing mode, the left mouse button is pressed outside the editbox
		/// or enter is pressed in SINGLE_LINE mode. Can be overwritten by derived GuiElements.
		virtual void OnTextEdit() override;
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_EDIT_VARIABLE";
		/// Default constructor
		GuiEditVariable(const T& value = T{}, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Font& font = defaultFont, const double& fontSize = defaultFontSizeNormal);
		/// Constructor from JSON
		GuiEditVariable(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiEditVariable();
		/// Copy constructor and assignment operator
		GuiEditVariable(const GuiEditVariable& other) noexcept;
		GuiEditVariable& operator=(const GuiEditVariable& other) noexcept;
		/// Move constructor and assignment operator
		GuiEditVariable(GuiEditVariable&& other) noexcept;
		GuiEditVariable& operator=(GuiEditVariable&& other) noexcept;
		/// Getters
		const T& getValue() const { return value; }
		const std::unique_ptr<Formatter<T>>& getFormatter() const { return formatter; }
		/// Setters
		void setValue(const T& value);
		void setFormatter(std::unique_ptr<Formatter<T>>&& formatter) { this->formatter = std::move(formatter); }
	protected:
		//==============================================================================================
		// GuiHandles
		//==============================================================================================
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle) override;
	public:
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!^Returns true on success
		bool setVariableHandle(GuiVariableHandle<T>& variableHandle);
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::updateText()
	{
		if(!formatter) setText(to_string(value));
		else setText(formatter->to_string(value));
		GuiEditBox::OnTextEdit();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::updateValue()
	{
		if (formatter) {
			std::optional<T> result = formatter->from_string(getText());
			if (result.has_value()) value = result.value();
		}
		else {
			std::optional<T> result = from_string<T>(getText());
			if (result.has_value()) value = result.value();
		}
		updateText();
		if (variableHandle) variableHandle->set(value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::OnTextEdit()
	{
		GuiEditBox::OnTextEdit();
		updateValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const T& value, const Vec2i& position, const Vec2i& size, const Font& font, const double& fontSize)
		: GuiEditBox(position, size, to_string(value), font, fontSize), variableHandle(nullptr), value(value)
	{
		setParseMode(StaticText::ParseMode::SINGLE_LINE);
		setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_SIZE);
		setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		setSizeHintModeMaxSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		setAlignment(StaticText::Alignment::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiEditBox(json, data, parameterNames)
	{
		parseJsonOrReadFromData(value, "value", json, data, parameterNames);
		if (!json.contains("parseMode")) setParseMode(StaticText::ParseMode::SINGLE_LINE);
		if (!json.contains("sizeHintModeMinSize")) setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_SIZE);
		if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (!json.contains("sizeHintModeMaxSize")) setSizeHintModeMaxSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (!json.contains("alignment")) setAlignment(StaticText::Alignment::CENTER);
		updateText();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::~GuiEditVariable()
	{
		if (variableHandle) signOffHandle(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const GuiEditVariable& other) noexcept
		: GuiEditBox(other), variableHandle(nullptr), value(other.value), 
		formatter(other.formatter ? std::make_unique(*other.formatter) : nullptr) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(const GuiEditVariable& other) noexcept
	{
		GuiEditBox::operator=(other);
		if (variableHandle) signOffHandle(variableHandle);
		variableHandle = nullptr;
		this->value = other.value;
		formatter = other.formatter ? std::make_unique(*other.formatter) : nullptr;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(GuiEditVariable&& other) noexcept
		: GuiEditBox(std::move(other)), variableHandle(std::move(other.variableHandle)), value(std::move(other.value)),
		formatter(std::move(other.formatter))
	{
		if (variableHandle) onHandleMove(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(GuiEditVariable&& other) noexcept
	{
		GuiEditBox::operator=(std::move(other));
		if (variableHandle) signOffHandle(*variableHandle);
		variableHandle = std::move(other.variableHandle);
		this->value = std::move(other.value);
		if (variableHandle) onHandleMove(*variableHandle);
		formatter = std::move(other.formatter);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::setValue(const T& value)
	{
		if (this->value != value) {
			this->value = value;
			if (variableHandle) setVariableHandleValue(*variableHandle, value);
			updateText();
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::onHandleMove(GuiHandle& guiHandle)
	{
		std::optional<GuiHandle::GuiHandleID> handleID = guiHandle.getHandleID(*this);
		if (!handleID) return;
		if (variableHandle && handleID.value() == 2) {
			variableHandle = &static_cast<GuiVariableHandle<T>&>(guiHandle);
		}
		else {
			GuiEditBox::onHandleMove(guiHandle);
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::onHandleDestruction(GuiHandle& guiHandle)
	{
		std::optional<GuiHandle::GuiHandleID> handleID = guiHandle.getHandleID(*this);
		if (!handleID) return;
		if (variableHandle && handleID.value() == 2) {
			variableHandle = nullptr;
		}
		else {
			GuiEditBox::onHandleDestruction(guiHandle);
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::onHandleUpdate(GuiHandle& guiHandle)
	{
		std::optional<GuiHandle::GuiHandleID> handleID = guiHandle.getHandleID(*this);
		if (!handleID) return;
		if (variableHandle && handleID.value() == 2) {
			if (value != variableHandle->get()) {
				value = variableHandle->get();
				updateText();
			}
		}
		else {
			GuiEditBox::onHandleUpdate(guiHandle);
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline bool GuiEditVariable<T>::setVariableHandle(GuiVariableHandle<T>& variableHandle)
	{
		if (this->variableHandle) return false;
		this->variableHandle = &variableHandle;
		signUpHandle(variableHandle, 2);
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	using GuiEditVariableFloat = GuiEditVariable<float>;
	using GuiEditVariableDouble = GuiEditVariable<double>;
	using GuiEditVariableInt = GuiEditVariable<int>;
	using GuiEditVariableUnsignedInt = GuiEditVariable<unsigned>;
	//--------------------------------------------------------------------------------------------------
}