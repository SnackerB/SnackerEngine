#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiEventHandles/GuiVariableHandle.h"
#include "Utility\Formatting.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T> 
	class GuiTextVariable : public GuiTextBox
	{
	public:
		/// Static default Attributes
		static std::optional<Formatter<T>> defaultFormatter;
	private:
		/// The handle to the variable that is shown in this textBox
		GuiVariableHandle<T>* variableHandle = nullptr;
		/// The current value
		T value{};
		/// The formatter used to format the text
		std::unique_ptr<Formatter<T>> formatter = nullptr;
		/// Helper function that updates the text
		void updateText();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_TEXT_VARIABLE";
		/// Default constructor
		GuiTextVariable(const T& value, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Font& font = defaultFont, const double& fontSize = defaultFontSizeNormal);
		/// Constructor from JSON
		GuiTextVariable(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiTextVariable();
		/// Copy constructor and assignment operator
		GuiTextVariable(const GuiTextVariable& other) noexcept;
		GuiTextVariable& operator=(const GuiTextVariable& other) noexcept;
		/// Move constructor and assignment operator
		GuiTextVariable(GuiTextVariable&& other) noexcept;
		GuiTextVariable& operator=(GuiTextVariable&& other) noexcept;
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
	inline void GuiTextVariable<T>::updateText()
	{
		if (!formatter) setText(to_string(value));
		else setText(formatter->to_string(value));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const T& value, const Vec2i& position, const Vec2i& size, const Font& font, const double& fontSize)
		: GuiTextBox(position, size, to_string(value), font, fontSize), variableHandle(nullptr), value(value)
	{
		setParseMode(StaticText::ParseMode::SINGLE_LINE);
		setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		parseJsonOrReadFromData(value, "value", json, data, parameterNames);
		if (!json.contains("parseMode")) setParseMode(StaticText::ParseMode::SINGLE_LINE);
		if (!json.contains("sizeHintModeMinSize")) setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (!json.contains("alignment")) setAlignment(StaticText::Alignment::LEFT);
		updateText();

	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::~GuiTextVariable()
	{
		if (variableHandle) signOffHandle(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const GuiTextVariable& other) noexcept
		: GuiTextBox(other), variableHandle(nullptr), value(other.value),
		formatter(other.formatter ? std::make_unique(*other.formatter) : nullptr) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>& GuiTextVariable<T>::operator=(const GuiTextVariable& other) noexcept
	{
		GuiTextBox::operator=(other);
		if (variableHandle) signOffHandle(variableHandle);
		variableHandle = nullptr;
		this->value = other.value;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(GuiTextVariable&& other) noexcept
		: GuiTextBox(std::move(other)), variableHandle(std::move(other.variableHandle)), value(std::move(other.value)),
		formatter(std::move(other.formatter))
	{
		if (variableHandle) onHandleMove(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>& GuiTextVariable<T>::operator=(GuiTextVariable&& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		if (variableHandle) signOffHandle(variableHandle);
		variableHandle = std::move(other.variableHandle);
		this->value = std::move(other.value);
		if (variableHandle) onHandleMove(*variableHandle);
		formatter = std::move(other.formatter);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::setValue(const T& value)
	{
		if (this->value != value) {
			this->value = value;
			if (variableHandle) setVariableHandleValue(*variableHandle, value);
			updateText();
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::onHandleMove(GuiHandle& guiHandle)
	{
		variableHandle = &static_cast<GuiVariableHandle<T>&>(guiHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::onHandleDestruction(GuiHandle& guiHandle)
	{
		variableHandle = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::onHandleUpdate(GuiHandle& guiHandle)
	{
		if (value != variableHandle->get()) {
			value = variableHandle->get();
			updateText();
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline bool GuiTextVariable<T>::setVariableHandle(GuiVariableHandle<T>& variableHandle)
	{
		if (this->variableHandle) return false;
		this->variableHandle = &variableHandle;
		signUpHandle(variableHandle, 1);
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	using GuiTextVariableFloat = GuiTextVariable<float>;
	using GuiTextVariableDouble = GuiTextVariable<double>;
	using GuiTextVariableInt = GuiTextVariable<int>;
	using GuiTextVariableUnsignedInt = GuiTextVariable<unsigned>;
	//--------------------------------------------------------------------------------------------------
	using GuiTextVariableVec2f = GuiTextVariable<Vec2f>;
	using GuiTextVariableVec2d = GuiTextVariable<Vec2d>;
	using GuiTextVariableVec2i = GuiTextVariable<Vec2i>;
	//--------------------------------------------------------------------------------------------------
	using GuiTextVariableVec3f = GuiTextVariable<Vec3f>;
	using GuiTextVariableVec3d = GuiTextVariable<Vec3d>;
	using GuiTextVariableVec3i = GuiTextVariable<Vec3i>;
	//--------------------------------------------------------------------------------------------------
	using GuiTextVariableVec4f = GuiTextVariable<Vec4f>;
	using GuiTextVariableVec4d = GuiTextVariable<Vec4d>;
	using GuiTextVariableVec4i = GuiTextVariable<Vec4i>;
	//--------------------------------------------------------------------------------------------------
}