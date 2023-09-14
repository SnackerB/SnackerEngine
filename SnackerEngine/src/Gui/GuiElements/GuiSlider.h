#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Core/Keys.h"
#include "Math/Conversions.h"
#include "Graphics/Renderer.h"
#include "Utility\Formatting.h"
#include "Gui\GuiManager.h"

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
		/// The handle to the variable that is shown in this slider
		GuiVariableHandle<T>* variableHandle = nullptr;
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
		T value{};
		/// min and max values the variable can take on
		T minValue{};
		T maxValue{};
		/// mouse offset when the moving of the slider button started
		int mouseOffset{};
		/// Transforms the value to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText(const T& value) { return std::to_string(value); }
		/// Sets the value in the variable handle to a given value and updates the text
		void setVariable(const T& value);
		/// Returns a const reference to the value of the variable handle
		const T& getVariable() const { return value; }
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SLIDER";
		/// Default constructor
		GuiSlider(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i());
		GuiSlider(const T& minValue, const T& maxValue, const T& value);
		/// Constructor from JSON.
		GuiSlider(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiSlider();
		/// Copy constructor and assignment operator
		GuiSlider(const GuiSlider& other) noexcept;
		GuiSlider& operator=(const GuiSlider& other) noexcept;
		/// Move constructor and assignment operator
		GuiSlider(GuiSlider&& other) noexcept;
		GuiSlider& operator=(GuiSlider&& other) noexcept;
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle<T>& variableHandle);
		/// Helper function that updates the text
		void updateText(const T& value);
		/// Getters
		int getSliderButtonWidth() const { return sliderButtonWidth; }
		const Color4f& getSliderButtonColor() const { return sliderButtonColor; }
		const Shader& getSliderButtonShader() const { return shader; }
		const T& getValue() const { return value; }
		const T& getMinValue() const { return minValue; }
		const T& getMaxValue() const { return maxValue; }
		/// Setters
		void setSliderButtonWidth(int sliderButtonWidth);
		void setSliderButtonColor(const Color4f& sliderButtonColor) { this->sliderButtonColor = sliderButtonColor; }
		void setSliderButtonShader(const Shader& sliderButtonShader) { this->shader = sliderButtonShader; }
		void setValue(const T& value);
		void setMinValue(const T& minValue);
		void setMaxValue(const T& maxValue);
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
		/// Computes the Slider button position from the current value of the variableHandle.
		/// Also clips the variableHandle to the allowed range
		void computeSliderButtonPositionFromVariableHandle(bool clipVariableHandle = false);
		
		//==============================================================================================
		// GuiHandles
		//==============================================================================================

		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle);
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle);

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
	inline void GuiSlider<T>::setVariable(const T& value)
	{
		if (variableHandle) variableHandle->set(value);
		else {
			this->value = value;
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
		:GuiTextBox(Vec2i{}, Vec2i{}, ""), value(value), minValue(minValue), maxValue(maxValue)
	{
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
		parseJsonOrReadFromData(value, "value", json, data, parameterNames);
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
	inline GuiSlider<T>::~GuiSlider()
	{
		if (variableHandle) signOffHandle(*variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const GuiSlider& other) noexcept
		: GuiTextBox(other), variableHandle(nullptr), 
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
		if (variableHandle) signOffHandle(*variableHandle);
		variableHandle = nullptr;
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
		: GuiTextBox(std::move(other)), variableHandle(std::move(other.variableHandle)), 
		formatter(std::move(other.formatter)), sliderButtonOffsetX(other.sliderButtonOffsetX), 
		sliderButtonWidth(other.sliderButtonWidth), sliderButtonColor(other.sliderButtonColor),
		sliderButtonShader(std::move(other.sliderButtonShader)), 
		sliderButtonModelMatrix(other.sliderButtonModelMatrix), value(std::move(other.value)), 
		minValue(std::move(other.minValue)), maxValue(std::move(other.maxValue)), 
		mouseOffset(other.mouseOffset)
	{
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(GuiSlider&& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		if (variableHandle) signOffHandle(*variableHandle);
		variableHandle = std::move(other.variableHandle);
		formatter = std::move(other.formatter);
		sliderButtonOffsetX = other.sliderButtonOffsetX;
		sliderButtonWidth = other.sliderButtonWidth;
		sliderButtonColor = other.sliderButtonColor;
		sliderButtonShader = std::move(other.sliderButtonShader);
		sliderButtonModelMatrix = other.sliderButtonModelMatrix;
		value = std::move(other.value);
		minValue = std::move(other.minValue);
		maxValue = std::move(other.maxValue);
		mouseOffset = other.mouseOffset;
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::setVariableHandle(GuiVariableHandle<T>& variableHandle)
	{
		if (!this->variableHandle) {
			this->variableHandle = &variableHandle;
			signUpHandle(variableHandle, 0);
			computeSliderButtonPositionFromVariableHandle(false);
		}
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
	inline void GuiSlider<T>::setValue(const T& value)
	{
		setVariable(value);
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
		value = std::max(minValue, std::min(maxValue, value));
		updateText(value);
		double percentage = static_cast<double>(value - minValue) / (static_cast<double>(maxValue - minValue));
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
		value = interpolate(minValue, maxValue, percentage);
		value = std::max(minValue, std::min(maxValue, value));
		if (variableHandle) {
			if (variableHandle->get() != value) {
				setVariableHandleValue(*variableHandle, value);
			}
		}
		updateText(value);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::computeSliderButtonPositionFromVariableHandle(bool clipVariableHandle)
	{
		if (!variableHandle) return;
		this->value = variableHandle->get();
		computeSliderButtonPositionFromValue();
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle<T>*>(&guiHandle);
	}
	//--------------------------------------------------------------------------------------------------	
	template<typename T>
	inline void GuiSlider<T>::onHandleDestruction(GuiHandle& guiHandle)
	{
		variableHandle = nullptr;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline void GuiSlider<T>::onHandleUpdate(GuiHandle& guiHandle)
	{
		computeSliderButtonPositionFromVariableHandle(false);
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