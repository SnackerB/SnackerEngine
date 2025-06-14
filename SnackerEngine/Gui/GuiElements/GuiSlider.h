#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Utility\Keys.h"
#include "Utility/Conversions.h"
#include "Graphics/Renderer.h"
#include "Utility\Formatting.h"
#include "Gui\GuiManager.h"
#include "Utility\Handles\VariableHandle.h"

#include <algorithm>

#ifdef min
#undef min
#endif // min

#ifdef max
#undef max
#endif // max

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiSlider : public GuiTextBox
	{
	public:
		/// Static default Attributes
		static int defaultSliderButtonWidth;
		static Color4f defaultSliderButtonColor;
		static Color4f defaultBackgroundColor;
		static SizeHintModes defaultSizeHintModes;
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
		/// The formatter used to format the text
		std::unique_ptr<Formatter<T>> formatter = nullptr;
		/// X offset from the left of the variableBox to the left of the sliderButton
		int sliderButtonOffsetX = 0;
		/// Width of the slider button
		int sliderButtonWidth = defaultSliderButtonWidth;
		/// Color of the slider button
		Color4f sliderButtonColor = defaultSliderButtonColor;
		/// Shader used to draw the slider button
		Shader sliderButtonShader = defaultBackgroundShader;
		/// Model matrix of the slider button
		Mat4f sliderButtonModelMatrix{};
		/// The current value of the variable
		GuiSliderVariableHandle value{ this };
		/// min and max values the variable can take on
		T minValue{};
		T maxValue{};
		/// mouse offset when the moving of the slider button started
		int mouseOffset{};
		/// Transforms the value to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText(const T& value) { return std::to_string(value); }
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
		/// Helper function that updates the text
		void updateText(const T& value);
		/// Getters
		int getSliderButtonWidth() const { return sliderButtonWidth; }
		const Color4f& getSliderButtonColor() const { return sliderButtonColor; }
		const Shader& getSliderButtonShader() const { return shader; }
		const T& getValue() const { return value; }
		const T& getMinValue() const { return minValue; }
		const T& getMaxValue() const { return maxValue; }
		VariableHandle<T>& getVariableHandle() { return value; }
		/// Setters
		void setSliderButtonWidth(int sliderButtonWidth);
		void setSliderButtonColor(const Color4f& sliderButtonColor) { this->sliderButtonColor = sliderButtonColor; }
		void setSliderButtonShader(const Shader& sliderButtonShader) { this->shader = sliderButtonShader; }
		void setValue(const T& value);
		void setMinValue(const T& minValue);
		void setMaxValue(const T& maxValue);
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateSliderButtonWidth(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateSliderButtonColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateMinValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateMaxValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
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
		/// Computes the position of the slider button from the value
		void computeSliderButtonPositionFromValue();
		/// Computes the model matrix of the slider button
		void computeSliderButtonModelMatrix();
		/// Computes the value according to the slider position and updates
		/// the handle and the text in the variableBox
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
	inline int GuiSlider<T>::defaultSliderButtonWidth = 20;
	template<typename T>
	inline Color4f GuiSlider<T>::defaultSliderButtonColor = Color4f(1.0f, 0.0f, 0.0f, 0.5f);
	template<typename T>
	inline Color4f GuiSlider<T>::defaultBackgroundColor = Color4f(0.0f, 1.0f);
	template<typename T>
	inline GuiTextBox::SizeHintModes GuiSlider<T>::defaultSizeHintModes = { GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE, GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT, GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT };
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
	inline GuiSlider<T>::GuiSlider(const Vec2i& position, const Vec2i& size)
		: GuiTextBox(position, size, "")
	{
		setParseMode(StaticText::ParseMode::SINGLE_LINE);
		if (size.y <= 0) {
			setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setResizeMode(ResizeMode::RESIZE_RANGE);
			setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
			setMaxWidth(SIZE_HINT_ARBITRARY);
		}
		setAlignment(StaticText::Alignment::CENTER);
		setBackgroundColor(defaultBackgroundColor);
		setSizeHintModes(defaultSizeHintModes);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const T& minValue, const T& maxValue, const T& value)
		:GuiTextBox(Vec2i{}, Vec2i{}, ""), value(this), minValue(minValue), maxValue(maxValue)
	{
		this->value.set(value);
		setParseMode(StaticText::ParseMode::SINGLE_LINE);
		setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
		setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_SIZE);
		setResizeMode(ResizeMode::RESIZE_RANGE);
		setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
		setMaxWidth(SIZE_HINT_ARBITRARY);
		setAlignment(StaticText::Alignment::CENTER);
		setBackgroundColor(defaultBackgroundColor);
		setSizeHintModes(defaultSizeHintModes);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		if (!json.contains("parseMode")) setParseMode(StaticText::ParseMode::SINGLE_LINE);
		parseJsonOrReadFromData(sliderButtonWidth, "sliderButtonWidth", json, data, parameterNames);
		parseJsonOrReadFromData(sliderButtonColor, "sliderButtonColor", json, data, parameterNames);
		T temp{};
		parseJsonOrReadFromData(temp, "value", json, data, parameterNames);
		value.set(temp);
		parseJsonOrReadFromData(minValue, "minValue", json, data, parameterNames);
		parseJsonOrReadFromData(maxValue, "maxValue", json, data, parameterNames);
		if (!json.contains("size")) {
			setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			if (!json.contains("resizeMode")) setResizeMode(ResizeMode::RESIZE_RANGE);
			setPreferredWidth(SIZE_HINT_AS_LARGE_AS_POSSIBLE);
			setMaxWidth(SIZE_HINT_ARBITRARY);
		}
		if (!json.contains("alignment")) setAlignment(StaticText::Alignment::CENTER);
		if (!json.contains("backgroundColor")) setBackgroundColor(defaultBackgroundColor);
		if (!json.contains("sizeHintModeMinSize")) setSizeHintModeMinSize(defaultSizeHintModes.sizeHintModeMinSize);
		if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(defaultSizeHintModes.sizeHintModePreferredSize);
		if (!json.contains("sizeHintModeMaxSize")) setSizeHintModeMaxSize(defaultSizeHintModes.sizeHintModeMaxSize);
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const GuiSlider& other) noexcept
		: GuiTextBox(other),
		formatter(other.formatter == nullptr ? nullptr : std::make_unique<Formatter<T>>(*other.formatter)), 
		sliderButtonOffsetX(0), sliderButtonWidth(other.sliderButtonWidth), 
		sliderButtonColor(other.sliderButtonColor), sliderButtonShader(other.sliderButtonShader), 
		sliderButtonModelMatrix{}, value(other.value), minValue(other.minValue), 
		maxValue(other.maxValue), mouseOffset(0) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(const GuiSlider& other) noexcept
	{
		GuiTextBox::operator=(other);
		formatter = other.formatter == nullptr ? nullptr : std::make_unique<Formatter<T>>(*other.formatter);
		sliderButtonOffsetX = 0;
		sliderButtonWidth = other.sliderButtonWidth;
		sliderButtonColor = other.sliderButtonColor;
		sliderButtonShader = other.sliderButtonShader;
		sliderButtonModelMatrix = Mat4f();
		value = other.value;
		minValue = other.minValue;
		maxValue = other.maxValue;
		mouseOffset = 0;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(GuiSlider&& other) noexcept
		: GuiTextBox(std::move(other)),
		formatter(std::move(other.formatter)), sliderButtonOffsetX(other.sliderButtonOffsetX), 
		sliderButtonWidth(other.sliderButtonWidth), sliderButtonColor(other.sliderButtonColor),
		sliderButtonShader(std::move(other.sliderButtonShader)), 
		sliderButtonModelMatrix(other.sliderButtonModelMatrix), value(std::move(other.value)), 
		minValue(std::move(other.minValue)), maxValue(std::move(other.maxValue)), 
		mouseOffset(other.mouseOffset)
	{
		value.parentElement = this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(GuiSlider&& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		formatter = std::move(other.formatter);
		sliderButtonOffsetX = other.sliderButtonOffsetX;
		sliderButtonWidth = other.sliderButtonWidth;
		sliderButtonColor = other.sliderButtonColor;
		sliderButtonShader = std::move(other.sliderButtonShader);
		sliderButtonModelMatrix = other.sliderButtonModelMatrix;
		value = std::move(other.value);
		value.parentElement = this;
		minValue = std::move(other.minValue);
		maxValue = std::move(other.maxValue);
		mouseOffset = other.mouseOffset;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template <typename T>
	inline void GuiSlider<T>::updateText(const T& value)
	{
		if (!formatter) setText(to_string(value));
		else setText(formatter->to_string(value));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::setSliderButtonWidth(int sliderButtonWidth)
	{
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::setMinValue(const T& minValue) 
	{
		this->minValue = minValue;
		setValue(this->value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::setMaxValue(const T& maxValue)
	{
		this->maxValue = maxValue;
		setValue(this->value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::unique_ptr<GuiElementAnimatable> GuiSlider<T>::animateSliderButtonWidth(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiSliderSliderButtonWidthAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (GuiElementValueAnimatable<int>::element) static_cast<GuiSlider<T>*>(GuiElementValueAnimatable<int>::element)->setSliderButtonWidth(currentVal); };
		public:
			GuiSliderSliderButtonWidthAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiSliderSliderButtonWidthAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::unique_ptr<GuiElementAnimatable> GuiSlider<T>::animateSliderButtonColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiSliderSliderButtonColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (GuiElementValueAnimatable<Color4f>::element) static_cast<GuiSlider<T>*>(GuiElementValueAnimatable<Color4f>::element)->setSliderButtonColor(currentVal); };
		public:
			GuiSliderSliderButtonColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiSliderSliderButtonColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::unique_ptr<GuiElementAnimatable> GuiSlider<T>::animateValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiSliderValueAnimatable : public GuiElementValueAnimatable<T>
		{
			virtual void onAnimate(const T& currentVal) override { if (GuiElementValueAnimatable<T>::element) static_cast<GuiSlider<T>*>(GuiElementValueAnimatable<T>::element)->setValue(currentVal); };
		public:
			GuiSliderValueAnimatable(GuiElement& element, const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<T>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiSliderValueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::unique_ptr<GuiElementAnimatable> GuiSlider<T>::animateMinValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiSliderMinValueAnimatable : public GuiElementValueAnimatable<T>
		{
			virtual void onAnimate(const T& currentVal) override { if (GuiElementValueAnimatable<T>::element) static_cast<GuiSlider<T>*>(GuiElementValueAnimatable<T>::element)->setMinValue(currentVal); };
		public:
			GuiSliderMinValueAnimatable(GuiElement& element, const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<T>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiSliderMinValueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::unique_ptr<GuiElementAnimatable> GuiSlider<T>::animateMaxValue(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiSliderMaxValueAnimatable : public GuiElementValueAnimatable<T>
		{
			virtual void onAnimate(const T& currentVal) override { if (GuiElementValueAnimatable<T>::element) static_cast<GuiSlider<T>*>(GuiElementValueAnimatable<T>::element)->setMaxValue(currentVal); };
		public:
			GuiSliderMaxValueAnimatable(GuiElement& element, const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<T>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiSliderMaxValueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::draw(const Vec2i& worldPosition)
	{
		// Draw text box
		GuiTextBox::draw(worldPosition);
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		pushClippingBox(worldPosition);
		// Draw Slider Button
		sliderButtonShader.bind();
		guiManager->setUniformViewAndProjectionMatrices(sliderButtonShader);
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		sliderButtonShader.setUniform<Mat4f>("u_model", translationMatrix * sliderButtonModelMatrix);
		sliderButtonShader.setUniform<Color4f>("u_color", sliderButtonColor);
		Renderer::draw(guiManager->getModelSquare());
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onRegister()
	{
		GuiTextBox::onRegister();
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		computeSliderButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onSizeChange()
	{
		GuiTextBox::onSizeChange();
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeSliderButtonPositionFromValue()
	{
		updateText(value);
		T temp = std::max(minValue, std::min(maxValue, value.get()));
		double percentage = static_cast<double>(temp - minValue) / (static_cast<double>(maxValue - minValue));
		sliderButtonOffsetX = std::max(0, static_cast<int>(std::lround(static_cast<double>(getWidth() - sliderButtonWidth) * percentage)));
		computeSliderButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeSliderButtonModelMatrix()
	{
		sliderButtonModelMatrix = Mat4f::TranslateAndScale(Vec3i(sliderButtonOffsetX, -getHeight(), 0), Vec3i(sliderButtonWidth, getHeight(), 0));
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeValueFromSliderButtonPosition()
	{
		double percentage = static_cast<double>(sliderButtonOffsetX) / (static_cast<double>(getWidth() - sliderButtonWidth));
		T temp = interpolate(minValue, maxValue, percentage);
		temp = std::max(minValue, std::min(maxValue, temp));
		if (value != temp) {
			value.set(temp);
			updateText(value);
		}
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::IsCollidingResult GuiSlider<T>::isColliding(const Vec2i& offset) const
	{
		return isCollidingBoundingBox(offset) ? IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
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
		int newSliderButtonOffset = getMouseOffset().x - mouseOffset;
		if (newSliderButtonOffset > getWidth() - static_cast<int>(sliderButtonWidth)) newSliderButtonOffset = getWidth() - sliderButtonWidth;
		if (newSliderButtonOffset < 0) newSliderButtonOffset = 0;
		sliderButtonOffsetX = newSliderButtonOffset;
		computeSliderButtonModelMatrix();
		computeValueFromSliderButtonPosition();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			// First check if we are colliding with the slider button or if the slider button needs to be moved
			int mouseOffsetX = getMouseOffset().x;
			if (mouseOffsetX >= sliderButtonOffsetX && mouseOffsetX <= sliderButtonOffsetX + static_cast<int>(sliderButtonWidth)) {
				mouseOffset = getMouseOffset().x - sliderButtonOffsetX;
			}
			else {
				mouseOffset = sliderButtonWidth / 2;
				callbackMouseMotion(Vec2d());
				mouseOffset = getMouseOffset().x - sliderButtonOffsetX;
			}
			signUpEvent(CallbackType::MOUSE_MOTION);
			signUpEvent(CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	using GuiSliderFloat = GuiSlider<float>;
	using GuiSliderDouble = GuiSlider<double>;
	using GuiSliderInt = GuiSlider<int>;
	using GuiSliderUnsignedInt = GuiSlider<unsigned>;
	//--------------------------------------------------------------------------------------------------
}