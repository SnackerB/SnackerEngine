#pragma once

#include "Gui/GuiElements/GuiTextBox.h"
#include "Core/Keys.h"
#include "Math/Conversions.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	class GuiSlider : public GuiTextBox
	{
	public:
		/// Default values for some of the member variables
		static Vec2i defaultSize;
		static unsigned defaultSliderButtonWidth;
		static Color4f defaultSliderButtonColor;
	private:
		/// The handle to the variable that is shown in this slider
		GuiVariableHandle<T>* variableHandle = nullptr;
		/// X offset from the left of the variableBox to the left of the sliderButton
		int sliderButtonOffsetX = 0;
		/// Width of the slider button
		unsigned sliderButtonWidth = defaultSliderButtonWidth;
		/// Color of the slider button
		Color4f sliderButtonColor = defaultSliderButtonColor;
		/// Shader used to draw the slider button
		Shader sliderButtonShader = Shader("shaders/gui/simpleAlphaColor.shader");
		/// Model matrix of the slider button
		Mat4f sliderButtonModelMatrix;
		/// The current value of the variable
		T value;
		/// min and max values the variable can take on
		T minValue;
		T maxValue;
		/// mouse offset when the moving of the slider button started
		int mouseOffset;
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
		GuiSlider(const Vec2i& position = Vec2i(), const Vec2i& size = defaultSize);
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
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle<T>& variableHandle);

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
	inline Vec2i GuiSlider<T>::defaultSize = Vec2i(250, 50);
	template<typename T>
	inline unsigned GuiSlider<T>::defaultSliderButtonWidth = 20;
	template<typename T>
	inline Color4f GuiSlider<T>::defaultSliderButtonColor = Color4f(1.0f, 0.0f, 0.0f, 0.5f);
	template<typename T>
	//--------------------------------------------------------------------------------------------------
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
		: GuiTextBox(position, size, toText(T{}))
	{
		setParseMode(StaticText::ParseMode::SINGLE_LINE);
		if (size.y == 0) {
			setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setResizeMode(ResizeMode::RESIZE_RANGE);
		}
		setAlignment(StaticText::Alignment::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		if (!json.contains("parseMode")) setParseMode(StaticText::ParseMode::SINGLE_LINE);
		parseJsonOrReadFromData(sliderButtonWidth, "sliderButtonWidth", json, data, parameterNames);
		parseJsonOrReadFromData(sliderButtonColor, "sliderButtonColor", json, data, parameterNames);
		parseJsonOrReadFromData(minValue, "minValue", json, data, parameterNames);
		parseJsonOrReadFromData(maxValue, "maxValue", json, data, parameterNames);
		if (!json.contains("size")) {
			setSize(defaultSize);
			setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			if (!json.contains("resizeMode")) setResizeMode(ResizeMode::RESIZE_RANGE);
		}
		if (!json.contains("alignment")) setAlignment(StaticText::Alignment::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(const GuiSlider& other) noexcept
		: GuiTextBox(other), variableHandle(nullptr), sliderButtonOffsetX(0), 
		sliderButtonWidth(other.sliderButtonWidth), sliderButtonColor(other.sliderButtonColor),
		sliderButtonShader(other.sliderButtonShader), sliderButtonModelMatrix{}, minValue(other.minValue), 
		maxValue(other.maxValue), mouseOffset(0) {}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(const GuiSlider& other) noexcept
	{
		GuiTextBox::operator=(other);
		if (variableHandle) signOffHandle(*variableHandle);
		variableHandle = nullptr;
		sliderButtonOffsetX = 0;
		sliderButtonWidth = other.sliderButtonWidth;
		sliderButtonColor = other.sliderButtonColor;
		sliderButtonShader = other.sliderButtonShader;
		sliderButtonModelMatrix = Mat4f();
		minValue = other.minValue;
		maxValue = other.maxValue;
		mouseOffset = 0;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	template<typename T>
	inline GuiSlider<T>::GuiSlider(GuiSlider&& other) noexcept
		: GuiTextBox(std::move(other)), variableHandle(std::move(other.variableHandle)),
		sliderButtonOffsetX(other.sliderButtonOffsetX), sliderButtonWidth(other.sliderButtonWidth), sliderButtonColor(other.sliderButtonColor),
		sliderButtonShader(std::move(other.sliderButtonShader)), sliderButtonModelMatrix(other.sliderButtonModelMatrix), minValue(other.minValue), maxValue(other.maxValue),
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
		sliderButtonOffsetX = other.sliderButtonOffsetX;
		sliderButtonWidth = other.sliderButtonWidth;
		sliderButtonColor = other.sliderButtonColor;
		sliderButtonShader = std::move(other.sliderButtonShader);
		sliderButtonModelMatrix = other.sliderButtonModelMatrix;
		minValue = other.minValue;
		maxValue = other.maxValue;
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
		computeValueFromSliderButtonPosition();
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
		setText(toText(value));
		value = std::max(minValue, std::min(maxValue, value));
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
		else {
			setText(toText(value));
		}
		setText(toText(value));
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
		const Vec2i& mySize = getSize();
		if (offset.x > 0 && offset.x < mySize.x
			&& offset.y > 0 && offset.y < mySize.y) {
			return IsCollidingResult::COLLIDE_STRONG;
		}
		return IsCollidingResult::NOT_COLLIDING;
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
}