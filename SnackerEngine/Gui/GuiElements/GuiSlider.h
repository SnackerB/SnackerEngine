#pragma once

#include "Gui\GuiElements\GuiPanel.h"
#include "Gui\GuiElements\GuiScaleButton.h"
#include "Utility\Keys.h"

#ifdef min
#undef min
#endif // min

#ifdef max
#undef max
#endif // max

namespace SnackerEngine
{

	template<typename T>
	class GuiSlider : public GuiElement
	{
	public:
		/// Static default Attributes
		static Vec2i defaultSliderButtonSize;
		static Color4f defaultSliderButtonColor;
		static int defaultBackgroundHeight;
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
		/// Special GuiPanel subclass that notifies the parent slider when it is clicked
		friend class SliderBackgroundPanel;
		class SliderBackgroundPanel : public GuiPanel {
		private:
			friend class GuiSlider;
			GuiSlider* parentElement;
		protected:
			void onRegister() override
			{
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
			}
			void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override
			{
				if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) parentElement->notifyLeftMouseButtonPressOnSliderBackground();
			}
		public:
			SliderBackgroundPanel(GuiSlider* parentElement)
				: GuiPanel(), parentElement(parentElement) {}
			SliderBackgroundPanel(GuiSlider* parentElement, const nlohmann::json& json, ParameterNameSet* parameterNameSet = nullptr)
				: GuiPanel(json, parameterNameSet), parentElement(parentElement) {}
			SliderBackgroundPanel(const SliderBackgroundPanel& other) = delete;
			SliderBackgroundPanel(SliderBackgroundPanel&& other) = default;
			SliderBackgroundPanel& operator=(const SliderBackgroundPanel& other) {
				GuiPanel::operator=(other);
				return *this;
			}
			SliderBackgroundPanel& operator=(SliderBackgroundPanel&& other) = default;
		};
		SliderBackgroundPanel sliderBackground{ this };
		// Special GuiScaleButton subclass that notifies the parent slider when it is clicked
		friend class SliderScaleButton;
		class SliderScaleButton : public GuiScaleButton {
		private:
			friend class GuiSlider;
			GuiSlider* parentElement;
		protected:
			void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override
			{
				GuiScaleButton::callbackMouseButtonOnElement(button, action, mods);
				if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) parentElement->notifyLeftMouseButtonPressOnSliderButton();
			}
		public:
			SliderScaleButton(GuiSlider* parentElement)
				: GuiScaleButton(), parentElement(parentElement) {}
			SliderScaleButton(GuiSlider* parentElement, const nlohmann::json& json, ParameterNameSet* parameterNameSet = nullptr)
				: GuiScaleButton(json, parameterNameSet), parentElement(parentElement) {}
			SliderScaleButton(const SliderScaleButton& other) = delete;
			SliderScaleButton(SliderScaleButton&& other) = default;
			SliderScaleButton& operator=(const SliderScaleButton& other) {
				GuiScaleButton::operator=(other);
				return *this;
			}
			SliderScaleButton& operator=(SliderScaleButton&& other) = default;
		};
		SliderScaleButton sliderButton{ this };
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
		GuiSlider(const nlohmann::json& json, ParameterNameSet* parameterNameSet = nullptr);
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
		/// Computes and sets the position and size of the background from the elements size.
		void computeBackgroundPositionAndSize();
		/// Computes and sets the position of the slider button from the value
		void computeSliderButtonPositionFromValue();
		/// Computes the value according to the slider position and updates the  variable handle
		void computeValueFromSliderButtonPosition();
		/// This function is called by the child sliderBackground element when it is clicked
		void notifyLeftMouseButtonPressOnSliderBackground();
		/// This function is called by the child sliderButton element when it is slicked
		void notifyLeftMouseButtonPressOnSliderButton();

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
	};
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline Vec2i GuiSlider<T>::defaultSliderButtonSize = Vec2i(30, 30);
	template<typename T>
	inline int GuiSlider<T>::defaultBackgroundHeight(10);
	template<typename T>
	inline Color4f GuiSlider<T>::defaultSliderButtonColor = Color4f(1.0f, 0.0f, 0.0f, 1.0f);
	template<typename T>
	inline Color4f GuiSlider<T>::defaultBackgroundColor = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const Vec2i& position, const Vec2i& size)
		: GuiElement(position, size)
	{
		sliderBackground.setBackgroundColor(defaultBackgroundColor);
		sliderBackground.setHeight(defaultBackgroundHeight);
		sliderButton.setSize(defaultSliderButtonSize);
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
		sliderBackground.setBackgroundColor(defaultBackgroundColor);
		sliderBackground.setHeight(defaultBackgroundHeight);
		sliderButton.setSize(defaultSliderButtonSize);
		sliderButton.setDefaultColor(defaultSliderButtonColor);
		sliderButton.setHoverColor(defaultSliderButtonColor);
		sliderButton.setPressedHoverColor(defaultSliderButtonColor);
		sliderButton.setPressedColor(defaultSliderButtonColor);
		this->value.set(value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const nlohmann::json& json, ParameterNameSet* parameterNameSet)
		: GuiElement(json, parameterNameSet)
	{
		sliderBackground.setBackgroundColor(defaultBackgroundColor);
		sliderBackground.setHeight(defaultBackgroundHeight);
		sliderButton.setSize(defaultSliderButtonSize);
		sliderButton.setDefaultColor(defaultSliderButtonColor);
		sliderButton.setHoverColor(defaultSliderButtonColor);
		sliderButton.setPressedHoverColor(defaultSliderButtonColor);
		sliderButton.setPressedColor(defaultSliderButtonColor);
		if (json.contains("sliderBackgroundOptions") && json["sliderBackgroundOptions"].is_object()) {
			sliderBackground = SliderBackgroundPanel(this, json["sliderBackgroundOptions"], parameterNameSet->getNestedParameterNameSet("sliderBackgroundOptions"));
		}
		if (json.contains("sliderButtonOptions") && json["sliderButtonOptions"].is_object()) {
			sliderButton = SliderScaleButton(this, json["sliderButtonOptions"], parameterNameSet->getNestedParameterNameSet("sliderButtonOptions"));
		}
		parseJson(minValue, "minValue", json, parameterNameSet);
		parseJson(maxValue, "maxValue", json, parameterNameSet);
		T temp{};
		parseJson(temp, "value", json, parameterNameSet);
		value.set(temp);
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const GuiSlider& other) noexcept
		: GuiElement(other), value(this), minValue(other.minValue), maxValue(other.maxValue), mouseOffset(0)
	{
		value.set(other.value);
		sliderBackground = other.sliderBackground;
		sliderButton = other.sliderButton;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(const GuiSlider& other) noexcept
	{
		GuiElement::operator=(other);
		value = GuiSliderVariableHandle(this);
		value.set(other.value);
		minValue = other.minValue;
		maxValue = other.maxValue;
		sliderBackground = other.sliderBackground;
		sliderBackground.parentElement = this;
		sliderButton = other.sliderButton;
		sliderButton.parentElement = this;
		mouseOffset = 0;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(GuiSlider&& other) noexcept
		: GuiElement(std::move(other)), value(std::move(other.value)), minValue(std::move(other.minValue)),
		maxValue(std::move(other.maxValue)), sliderBackground(std::move(other.sliderBackground)), 
		sliderButton(std::move(other.sliderButton)), mouseOffset(std::move(other.mouseOffset))
	{
		value.parentElement(this);
		sliderBackground.parentElement = this;
		sliderButton.parentElement = this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(GuiSlider&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		value = std::move(other.value);
		value.parentElement = this;
		minValue = std::move(other.minValue);
		maxValue = std::move(other.maxValue);
		sliderBackground = std::move(other.sliderBackground);
		sliderBackground.parentElement = this;
		sliderButton = std::move(other.sliderButton);
		sliderButton.parentElement = this;
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
		if (!getGuiManager()) return;
		if (getClipChildren()) pushClippingBox(worldPosition);
		sliderBackground.draw(worldPosition + sliderBackground.getPosition());
		if (getClipChildren()) popClippingBox();
		// We need to draw the button without a cplipping box, since it is a scale button and get larger than the element.
		sliderButton.draw(worldPosition + sliderButton.getPosition());
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onRegister()
	{
		GuiElement::onRegister();
		computeSliderButtonPositionFromValue();
		registerChild(sliderBackground);
		registerChild(sliderButton);
		onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onSizeChange()
	{
		GuiElement::onSizeChange();
		computeBackgroundPositionAndSize();
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeBackgroundPositionAndSize()
	{
		sliderBackground.setPositionAndSize(
			Vec2i(sliderButton.getWidth() / 2, (getHeight() - sliderBackground.getHeight()) / 2),
			Vec2i(getWidth() - sliderButton.getWidth(), sliderBackground.getHeight()));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeSliderButtonPositionFromValue()
	{
		T temp = std::max(minValue, std::min(maxValue, value.get()));
		double percentage = static_cast<double>(temp - minValue) / (static_cast<double>(maxValue - minValue));
		sliderButton.setPositionX(static_cast<double>(getWidth() - sliderButton.getWidth()) * percentage);
		sliderButton.setPositionY((getHeight() - sliderButton.getHeight()) / 2);
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
	inline void GuiSlider<T>::notifyLeftMouseButtonPressOnSliderBackground()
	{
		// Bring sliderButton to the clicked position and switch mode s.t. the user can further move the button.
		mouseOffset = getMouseOffsetX();
		double percentage = static_cast<double>(mouseOffset - sliderButton.getWidth() / 2) / static_cast<double>(getWidth() - sliderButton.getWidth());
		T newValue = static_cast<T>(minValue + static_cast<double>(maxValue - minValue) * percentage);
		setValue(newValue);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		mouseOffset = mouseOffset - sliderButton.getPositionX();
		sliderButton.setIsBeingPressed(true);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::notifyLeftMouseButtonPressOnSliderButton()
	{
		// TODO
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		mouseOffset = getMouseOffsetX() - sliderButton.getPositionX();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::IsCollidingResult GuiSlider<T>::isColliding(const Vec2i& offset) const
	{
		return IsCollidingResult::COLLIDE_CHILD;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(CallbackType::MOUSE_MOTION);
			signOffEvent(CallbackType::MOUSE_BUTTON);
			computeValueFromSliderButtonPosition();
			computeSliderButtonPositionFromValue();
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::callbackMouseMotion(const Vec2d& position)
	{
		int newSliderButtonPositionX = getMouseOffsetX() - mouseOffset;
		if (newSliderButtonPositionX > getWidth() - sliderButton.getWidth()) newSliderButtonPositionX = getWidth() - sliderButton.getWidth();
		if (newSliderButtonPositionX < 0) newSliderButtonPositionX = 0;
		sliderButton.setPositionX(newSliderButtonPositionX);
		computeValueFromSliderButtonPosition();
	}
	//--------------------------------------------------------------------------------------------------
	using GuiSliderFloat = GuiSlider<float>;
	using GuiSliderDouble = GuiSlider<double>;
	using GuiSliderInt = GuiSlider<int>;
	using GuiSliderUnsignedInt = GuiSlider<unsigned>;
	//--------------------------------------------------------------------------------------------------
}