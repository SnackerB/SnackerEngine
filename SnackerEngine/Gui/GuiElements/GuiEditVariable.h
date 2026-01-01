#pragma once

#include "Gui\GuiElements\GuiEditBox.h"
#include "Utility\Formatting.h"
#include "Utility\Handles\VariableHandle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiEditVariable : public GuiEditBox
	{
	private:
		class GuiEditVariableVariableHandle : public VariableHandle<T>
		{
		private:
			friend class GuiEditVariable;
			GuiEditVariable* parentElement;
		protected:
			void onEvent() override
			{
				parentElement->updateText();
			}
		public:
			GuiEditVariableVariableHandle(GuiEditVariable* parentElement)
				: VariableHandle<T>(), parentElement(parentElement) {}
		};
		/// Handle to the current value
		GuiEditVariableVariableHandle value{ this };
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
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiEditVariable(const T& value, const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const Font& font = defaultFont, const double& fontSize = defaultFontSizeNormal);
		/// Constructor from JSON
		GuiEditVariable(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiEditVariable() {};
		/// Copy constructor and assignment operator
		GuiEditVariable(const GuiEditVariable& other) noexcept;
		GuiEditVariable& operator=(const GuiEditVariable& other) noexcept;
		/// Move constructor and assignment operator
		GuiEditVariable(GuiEditVariable&& other) noexcept;
		GuiEditVariable& operator=(GuiEditVariable&& other) noexcept;
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
	inline void GuiEditVariable<T>::updateText()
	{
		if (!formatter) setText(to_string(value.get()));
		else setText(formatter->to_string(value.get()));
		GuiEditBox::OnTextEdit();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::updateValue()
	{
		if (formatter) {
			std::optional<T> result = formatter->from_string(getText());
			if (result.has_value()) value.set(result.value());
		}
		else {
			std::optional<T> result = from_string<T>(getText());
			if (result.has_value()) value.set(result.value());
		}
		updateText();
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
		: GuiEditBox(position, size, "", font, fontSize), value( this )
	{
		setValue(value);
		setParseMode(StaticText::ParseMode::SINGLE_LINE);
		setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_SIZE);
		setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		setSizeHintModeMaxSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
		setAlignmentHorizontal(AlignmentHorizontal::CENTER);
		setAlignmentVertical(AlignmentVertical::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiEditBox(json, data, parameterNames)
	{
		T temp{};
		parseJsonOrReadFromData(temp, "value", json, data, parameterNames);
		value.set(temp);
		if (!json.contains("parseMode")) setParseMode(StaticText::ParseMode::SINGLE_LINE);
		if (!json.contains("alignmentHorizontal")) setAlignmentHorizontal(AlignmentHorizontal::CENTER);
		if (!json.contains("alignmentVertical")) setAlignmentVertical(AlignmentVertical::CENTER);
		updateText();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(const GuiEditVariable& other) noexcept
		: GuiEditBox(other), value(other.value),
		formatter(other.formatter ? std::make_unique(*other.formatter) : nullptr) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(const GuiEditVariable& other) noexcept
	{
		GuiEditBox::operator=(other);
		value = other.value;
		formatter = other.formatter ? std::make_unique(*other.formatter) : nullptr;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>::GuiEditVariable(GuiEditVariable&& other) noexcept
		: GuiEditBox(std::move(other)), value(std::move(other.value)),
		formatter(std::move(other.formatter))
	{
		value.parentElement = this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiEditVariable<T>& GuiEditVariable<T>::operator=(GuiEditVariable&& other) noexcept
	{
		GuiEditBox::operator=(std::move(other));
		value = std::move(other.value);
		value.parentElement = this;
		formatter = std::move(other.formatter);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiEditVariable<T>::setValue(const T& value)
	{
		if (this->value != value) {
			this->value.set(value);
			updateText();
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::unique_ptr<GuiElementAnimatable> GuiEditVariable<T>::animateValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiEditVariableValueAnimatable : public GuiElementValueAnimatable<T>
		{
			virtual void onAnimate(const T& currentVal) override { if (GuiElementValueAnimatable<T>::element) static_cast<GuiEditVariable<T>*>(GuiElementValueAnimatable<T>::element)->setValue(currentVal); };
		public:
			GuiEditVariableValueAnimatable(GuiElement& element, const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<T>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiEditVariableValueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	using GuiEditVariableFloat = GuiEditVariable<float>;
	using GuiEditVariableDouble = GuiEditVariable<double>;
	using GuiEditVariableInt = GuiEditVariable<int>;
	using GuiEditVariableUnsignedInt = GuiEditVariable<unsigned>;
	//--------------------------------------------------------------------------------------------------
}