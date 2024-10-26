#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Utility\Handles\VariableHandle.h"
#include "Utility\Formatting.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T> 
	class GuiTextVariable : public GuiTextBox
	{
	private:
		class GuiTextVariableVariableHandle : public VariableHandle<T>
		{
		private:
			friend class GuiTextVariable;
			GuiTextVariable* parentElement;
		protected:
			void onEvent() override
			{
				parentElement->updateText();
			}
		public:
			GuiTextVariableVariableHandle(GuiTextVariable* parentElement)
				: VariableHandle<T>(), parentElement(parentElement) {}
		};
		/// The handle to the value that is shown in this textBox
		GuiTextVariableVariableHandle value{ this };
		/// The formatter used to format the text
		std::unique_ptr<Formatter<T>> formatter = nullptr;
		/// Helper function that updates the text
		void updateText();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_TEXT_VARIABLE";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiTextVariable(const T& value = T{}, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Font& font = defaultFont, const double& fontSize = defaultFontSizeNormal);
		/// Constructor from JSON
		GuiTextVariable(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiTextVariable() {};
		/// Copy constructor and assignment operator
		GuiTextVariable(const GuiTextVariable& other) noexcept;
		GuiTextVariable& operator=(const GuiTextVariable& other) noexcept;
		/// Move constructor and assignment operator
		GuiTextVariable(GuiTextVariable&& other) noexcept;
		GuiTextVariable& operator=(GuiTextVariable&& other) noexcept;
		/// Getters
		const T& getValue() const { return value; }
		const std::unique_ptr<Formatter<T>>& getFormatter() const { return formatter; }
		VariableHandle<T>& getVariableHandle() { return value; }
		/// Setters
		void setValue(const T& value);
		void setFormatter(std::unique_ptr<Formatter<T>>&& formatter) { this->formatter = std::move(formatter); }
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::updateText()
	{
		if (!formatter) setText(to_string(value.get()));
		else setText(formatter->to_string(value.get()));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const T& value, const Vec2i& position, const Vec2i& size, const Font& font, const double& fontSize)
		: GuiTextBox(position, size, "", font, fontSize), value(value)
	{
		setParseMode(StaticText::ParseMode::SINGLE_LINE);
		setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_SIZE);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		T temp{};
		parseJsonOrReadFromData(temp, "value", json, data, parameterNames);
		value.set(temp);
		if (!json.contains("parseMode")) setParseMode(StaticText::ParseMode::SINGLE_LINE);
		if (!json.contains("sizeHintModeMinSize")) setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_SIZE);
		if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_SIZE);
		if (!json.contains("alignment")) setAlignment(StaticText::Alignment::LEFT);
		updateText();

	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(const GuiTextVariable& other) noexcept
		: GuiTextBox(other), value(other.value),
		formatter(other.formatter ? std::make_unique(*other.formatter) : nullptr) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>& GuiTextVariable<T>::operator=(const GuiTextVariable& other) noexcept
	{
		GuiTextBox::operator=(other);
		value = other.value;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>::GuiTextVariable(GuiTextVariable&& other) noexcept 
		: GuiTextBox(std::move(other)), value(std::move(other.value)),
		formatter(std::move(other.formatter)) 
	{
		value.parentElement = this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiTextVariable<T>& GuiTextVariable<T>::operator=(GuiTextVariable&& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		value = std::move(other.value);
		value.parentElement = this;
		formatter = std::move(other.formatter);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiTextVariable<T>::setValue(const T& value)
	{
		if (this->value != value) {
			this->value.set(value);
			updateText();
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::unique_ptr<GuiElementAnimatable> GuiTextVariable<T>::animateValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiTextVariableValueAnimatable : public GuiElementValueAnimatable<T>
		{
			virtual void onAnimate(const T& currentVal) override { if (GuiElementValueAnimatable<T>::element) static_cast<GuiTextVariable<T>*>(GuiElementValueAnimatable<T>::element)->setValue(currentVal); };
		public:
			GuiTextVariableValueAnimatable(GuiElement& element, const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<T>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiTextVariableValueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
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