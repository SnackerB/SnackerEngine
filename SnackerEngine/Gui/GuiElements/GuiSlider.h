#pragma once

#include "Gui\GuiElements\GuiPanel.h"
#include "Gui\GuiElements\GuiScaleButton.h"

namespace SnackerEngine
{

	template<typename T>
	class GuiSlider : public GuiPanel
	{
	public:
		/// Static default Attributes
		static Vec2i defaultSliderButtonSize;
		static Color4f defaultSliderButtonColor;
		static Color4f defaultBackgroundColor;
	private:
		class GuiSliderVariableHandle : public VariableHandle<T>
		{
		private:
			friend class GuiSlider;
			GuiSlider* parentElement;
		protected:
			void onEvent() override
			{
				parentElement->computeSliderButtonPositionFromValue();
			}
		public:
			GuiSliderVariableHandle(GuiSlider* parentElement)
				: VariableHandle<T>(), parentElement(parentElement) {}
		};
		GuiSliderVariableHandle value{ this };
		T minValue{};
		T maxValue{};
		GuiScaleButton sliderButton{};
		/// mouse offset when the moving of the slider button started
		int mouseOffset{};
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSlider(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i());
		GuiSlider(const T& minValue, const T& maxValue, const T& value);
		/// Constructor from JSON.
		GuiSlider(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiSlider() {};
		/// Copy constructor and assignment operator
		GuiSlider(const GuiSlider& other) noexcept;
		GuiSlider& operator=(const GuiSlider& other) noexcept;
		/// Move constructor and assignment operator
		GuiSlider(GuiSlider&& other) noexcept;
		GuiSlider& operator=(GuiSlider&& other) noexcept;
		/// Getters
		const T& getValue() const { return value; }
		const T& getMinValue() const { return minValue; }
		const T& getMaxValue() const { return maxValue; }
		VariableHandle<T>& getVariableHandle() { return value; }
		const GuiScaleButton& getSliderButton() const { return sliderButton; }
		/// Setters
		void setValue(const T& value);
		void setMinValue(const T& minValue);
		void setMaxValue(const T& maxValue);
		// TODO: Implement setters for the slider button
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateMinValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateMaxValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		// TODO: Implement animatables for the slider button
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the position changes. Can eg. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Computes and sets the position of the slider button from the value
		void computeSliderButtonPositionFromValue();
		/// Computes the value according to the slider position and updates the  variable handle
		void computeValueFromSliderButtonPosition();

		//==============================================================================================
		// Collisions
		//==============================================================================================

		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;

		//==============================================================================================
		// Events
		//==============================================================================================

		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline Vec2i GuiSlider<T>::defaultSliderButtonSize = Vec2i(20, 20);
	template<typename T>
	inline Color4f GuiSlider<T>::defaultSliderButtonColor = Color4f(1.0f, 0.0f, 0.0f, 0.5f);
	template<typename T>
	inline Color4f GuiSlider<T>::defaultBackgroundColor = Color4f(0.0f, 1.0f);
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const Vec2i& position, const Vec2i& size)
		: GuiPanel(position, size), sliderButton(Vec2i(), defaultSliderButtonSize, "")
	{
		setBackgroundColor(defaultBackgroundColor);
		sliderButton.setDefaultColor(defaultSliderButtonColor);
		sliderButton.setHoverColor(defaultSliderButtonColor);
		sliderButton.setPressedHoverColor(defaultSliderButtonColor);
		sliderButton.setPressedColor(defaultSliderButtonColor);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const T& minValue, const T& maxValue, const T& value)
		: GuiSlider(Vec2i(), Vec2i()), minValue(minValue), maxValue(maxValue)
	{
		this->value.set(value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiPanel(json, data, parameterNames)
	{
		if (json.contains("sliderButtonOptions") && json["sliderButtonOptions"].is_object()) {
			sliderButton = GuiScaleButton(json["sliderButtonOptions"], data, nullptr);
			parameterNames->erase("sliderButtonOptions"); // TODO: Can we make this recursive, s.t. we also check if everything is read from sliderButtonOptions? This would help in debugging GUI applications.
		}
		T temp{};
		parseJsonOrReadFromData(temp, "value", json, data, parameterNames);
		value.set(temp);
		parseJsonOrReadFromData(minValue, "minValue", json, data, parameterNames);
		parseJsonOrReadFromData(maxValue, "maxValue", json, data, parameterNames);
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const GuiSlider& other) noexcept
		: GuiPanel(other), value(this), minValue(other.minValue), maxValue(other.maxValue),
		sliderButton(other.sliderButton), mouseOffset(0)
	{
		value.set(other.value);
		sliderButton.setPosition(Vec2i());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(const GuiSlider& other) noexcept
	{
		// TODO: Remove from callback functions
		GuiPanel::operator=(other);
		value = GuiSliderVariableHandle(this);
		value.set(other.value);
		minValue = other.minValue;
		maxValue = other.maxValue;
		sliderButton = other.sliderButton;
		sliderButton.setPosition(Vec2i());
		mouseOffset = 0;
		// TODO: Register new slider button and compute its position
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(GuiSlider&& other) noexcept
		: GuiPanel(std::move(other)), value(std::move(other.value)), minValue(std::move(other.minValue)),
		maxValue(std::move(other.maxValue)), sliderButton(std::move(other.sliderButton)),
		mouseOffset(std::move(other.mouseOffset))
	{
		value.parentElement(this);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(GuiSlider&& other) noexcept
	{
		GuiPanel::operator=(std::move(other));
		value = std::move(other.value);
		value.parentElement = this;
		minValue = std::move(other.minValue);
		maxValue = std::move(other.maxValue);
		sliderButton = std::move(other.sliderButton);
		mouseOffset = std::move(other.mouseOffset);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::setValue(const T& value)
	{
		T temp = std::min(maxValue, std::max(minValue, value));
		if (this->value != temp)
		{
			this->value.set(temp);
			computeSliderButtonPositionFromValue();
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::draw(const Vec2i& worldPosition)
	{
		GuiPanel::draw(worldPosition);
		// No clipping boxes, because the sliderButton may be larger than the background panel
		drawElement(sliderButton.getGuiID(), worldPosition + sliderButton.getPosition());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onRegister()
	{
		GuiPanel::onRegister();
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onSizeChange()
	{
		GuiPanel::onSizeChange();
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeSliderButtonPositionFromValue()
	{
		T temp = std::max(minValue, std::min(maxValue, value.get()));
		double percentage = static_cast<double>(temp - minValue) / (static_cast<double>(maxValue - minValue));
		sliderButton.setPositionX(std::max(0, static_cast<int>(std::lround(static_cast<double>(getWidth() - sliderButton.getWidth()) * percentage))));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeValueFromSliderButtonPosition()
	{
		double percentage = static_cast<double>(sliderButton.getPositionX()) / (static_cast<double>(getWidth() - sliderButton.getWidth()));
		T temp = interpolate(minValue, maxValue, percentage);
		temp = std::max(minValue, std::min(maxValue, temp));
		if (value != temp) {
			value.set(temp);
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::IsCollidingResult GuiSlider<T>::isColliding(const Vec2i& offset) const
	{
		return IsCollidingResult();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::callbackMouseMotion(const Vec2d& position)
	{
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
	}
	//--------------------------------------------------------------------------------------------------
}