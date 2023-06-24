#pragma once

#include "Gui/GuiElement.h"
#include "Graphics/Color.h"
#include "Gui/GuiElements/GuiTextBox.h"
#include "Math/Conversions.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Gui/GuiStyle.h"
#include "Core/Keys.h"

#include <optional>

namespace SnackerEngine
{

	template<typename T>
	class GuiSlider : public GuiElement
	{
	private:
		/// The handle to the variable that is shown in this slider
		GuiVariableHandle<T>* variableHandle;
		/// The GuiTextBox that shows the label
		std::unique_ptr<GuiDynamicTextBox> label;
		/// The GuiTextBox that shows the variable
		std::unique_ptr<GuiDynamicTextBox> variableBox;
		/// X offset from the left of the variableBox to the left of the sliderButton
		int sliderButtonOffsetX;
		/// Width of the slider button
		double sliderButtonWidth;
		/// Color of the slider button
		Color4f sliderButtonColor;
		/// Shader used to draw the slider button
		Shader sliderButtonShader;
		/// Model matrix of the slider button
		Mat4f sliderButtonModelMatrix;
		/// min and max values the variable can take on
		T minValue;
		T maxValue;
		/// mouse offset when the moving of the slider button started
		int mouseOffset;
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) override;
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle) override;
		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle) override;
		/// Transforms the value to a UTF8 encoded string. May apply additional rounding operation.
		virtual std::string toText(const T& value);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle) override;
		/// Sets the value in the variable handle to a given value and updates the text
		void setVariable(const T& value);
		/// Returns a const reference to the value of the variable handle
		const T& getVariable() const;
		/// Computes the model matrix of the slider button
		void computeSliderButtonModelMatrix();
		/// Computes the value according to the slider position and updates
		/// the handle and the text in the variableBox
		void computeValueFromSliderButtonPosition();
		/// Computes the Slider button position from the current value of the variableHandle.
		/// Also clips the variableHandle to the allowed range
		void computeSliderButtonPositionFromVariableHandle(bool clipVariableHandle = false);

		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse button input on this guiElement. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
	public:
		/// Constructor
		GuiSlider(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode,
			const std::string& label, const T& minValue, const T& maxValue, const Font& font, const double& fontSize,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 0.0f }, const Color4f& labelBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const Color4f& sliderBoxTextColor = { 1.0f, 1.0f, 1.0f, 0.0f }, const Color4f& SliderBoxBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f },
			const Color4f& sliderButtonColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const double& sliderButtonWidth = 5.0,
			const int& border = 0);
		/// Constructor that already registers variable handle
		GuiSlider(GuiVariableHandle<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode,
			const std::string& label, const T& minValue, const T& maxValue, const Font& font, const double& fontSize,
			const Color4f& labelTextColor = { 1.0f, 1.0f, 1.0f, 0.0f }, const Color4f& labelBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f },
			const Color4f& sliderBoxTextColor = { 1.0f, 1.0f, 1.0f, 0.0f }, const Color4f& SliderBoxBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f },
			const Color4f& sliderButtonColor = Color4f(0.0f, 0.0f, 0.0f, 1.0f), const double& sliderButtonWidth = 5.0,
			const int& border = 0);
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first!
		void setVariableHandle(GuiVariableHandle<T>& variableHandle);
		/// Constructors using GuiStyle
		GuiSlider(const std::string& label, const T& minValue, const T& maxValue, GuiVariableHandle<T>& handle, const GuiStyle& style);
		GuiSlider(const std::string& label, const T& minValue, const T& maxValue, const GuiStyle& style);
		/// Copy constructor and assignment operator
		GuiSlider(const GuiSlider<T>& other) noexcept;
		GuiSlider& operator=(const GuiSlider<T>& other) noexcept;
		/// Move constructor and assignment operator
		GuiSlider(GuiSlider<T>&& other) noexcept;
		GuiSlider& operator=(GuiSlider<T>&& other) noexcept;
		/// Destructor
		~GuiSlider();
	};

	template<typename T>
	inline void GuiSlider<T>::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		pushClippingBox(worldPosition);
		drawElement(label->getGuiID(), worldPosition + label->getPosition());
		drawElement(variableBox->getGuiID(), worldPosition + variableBox->getPosition());
		// Draw Slider Button
		sliderButtonShader.bind();
		guiManager->setUniformViewAndProjectionMatrices(sliderButtonShader);
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		sliderButtonShader.setUniform<Mat4f>("u_model", translationMatrix * sliderButtonModelMatrix);
		sliderButtonShader.setUniform<Color4f>("u_color", sliderButtonColor);
		Renderer::draw(guiManager->getModelSquare());
		// Draw children
		GuiElement::draw(worldPosition);
		popClippingBox();
	}

	template<typename T>
	inline void GuiSlider<T>::onRegister()
	{
		registerElementAsChild(*label);
		registerElementAsChild(*variableBox);
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}

	template<typename T>
	inline void GuiSlider<T>::onSizeChange()
	{
		int variableBoxWidth = getWidth() - label->getWidth();
		if (variableBoxWidth < 0) variableBoxWidth = 0;
		variableBox->setPositionAndSize(Vec2i(label->getWidth(), 0), Vec2i(variableBoxWidth, label->getHeight()));
		computeSliderButtonPositionFromVariableHandle(false);
	}

	template<typename T>
	inline GuiSlider<T>::IsCollidingResult GuiSlider<T>::isColliding(const Vec2i& offset)
	{
		const Vec2i& mySize = getSize();
		if (offset.x > 0 && offset.x < mySize.x
			&& offset.y > 0 && offset.y < mySize.y) {
			// Check if variableBox was hit
			if (offset.x > label->getWidth()) {
				return IsCollidingResult::COLLIDE_STRONG;
			}
			return IsCollidingResult::COLLIDE_CHILD;
		}
		return IsCollidingResult::NOT_COLLIDING;
	}

	template<typename T>
	inline void GuiSlider<T>::onHandleDestruction(GuiHandle& guiHandle)
	{
		variableHandle = nullptr;
	}

	template<typename T>
	inline void GuiSlider<T>::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		variableHandle = static_cast<GuiVariableHandle<T>*>(&guiHandle);
	}

	template<typename T>
	inline std::string GuiSlider<T>::toText(const T& value)
	{
		return std::to_string(value);
	}

	template<typename T>
	inline void GuiSlider<T>::onHandleUpdate(GuiHandle& guiHandle)
	{
		computeSliderButtonPositionFromVariableHandle(false);
	}

	template<typename T>
	inline void GuiSlider<T>::setVariable(const T& value)
	{
		if (variableHandle) {
			variableHandle->set(value);
		}
	}

	template<typename T>
	inline const T& GuiSlider<T>::getVariable() const
	{
		if (variableHandle) return variableHandle->get();
		return T{};
	}

	template<typename T>
	inline void GuiSlider<T>::computeSliderButtonModelMatrix()
	{
		sliderButtonModelMatrix = Mat4f::TranslateAndScale(Vec3i(label->getWidth() + sliderButtonOffsetX, -getHeight(), 0), Vec3i(sliderButtonWidth, getHeight(), 0));
	}

	template<typename T>
	inline void GuiSlider<T>::computeValueFromSliderButtonPosition()
	{
		double percentage = static_cast<double>(sliderButtonOffsetX) / (static_cast<double>(variableBox->getWidth() - sliderButtonWidth));
		T value = interpolate(minValue, maxValue, percentage);
		value = std::max(minValue, std::min(maxValue, value));
		if (variableHandle) {
			if (variableHandle->get() != value) {
				setVariableHandleValue(*variableHandle, value);
				variableBox->setText(toText(value));
			}
		}
		else {
			variableBox->setText(toText(value));
		}
	}

	template<typename T>
	inline void GuiSlider<T>::computeSliderButtonPositionFromVariableHandle(bool clipVariableHandle)
	{
		if (!variableHandle) return;
		T value = variableHandle->get();
		variableBox->setText(toText(value));
		value = std::max(minValue, std::min(maxValue, value));
		if (value != variableHandle->get() && clipVariableHandle) {
			setVariableHandleValue(*variableHandle, value);
		}
		double percentage = static_cast<double>(value - minValue) / (static_cast<double>(maxValue - minValue));
		sliderButtonOffsetX = std::max(0, static_cast<int>(std::lround(static_cast<double>(variableBox->getWidth() - sliderButtonWidth) * percentage)));
		computeSliderButtonModelMatrix();
	}

	template<typename T>
	inline void GuiSlider<T>::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(CallbackType::MOUSE_MOTION);
			signOffEvent(CallbackType::MOUSE_BUTTON);
			computeValueFromSliderButtonPosition();
			computeSliderButtonPositionFromVariableHandle(true);
		}
	}

	template<typename T>
	inline void GuiSlider<T>::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			// First check if we are colliding with the slider button or if the slider button needs to be moved
			int mouseOffsetX = getMouseOffset(getGuiID()).x - label->getWidth();
			if (mouseOffsetX >= sliderButtonOffsetX && mouseOffsetX <= sliderButtonOffsetX + sliderButtonWidth) {
				mouseOffset = getMouseOffset(getGuiID()).x - label->getWidth() - sliderButtonOffsetX;
			}
			else {
				mouseOffset = sliderButtonWidth / 2;
				callbackMouseMotion(Vec2d());
				mouseOffset = getMouseOffset(getGuiID()).x - label->getWidth() - sliderButtonOffsetX;
			}
			signUpEvent(CallbackType::MOUSE_MOTION);
			signUpEvent(CallbackType::MOUSE_BUTTON);
		}
	}

	template<typename T>
	inline void GuiSlider<T>::callbackMouseMotion(const Vec2d& position)
	{
		int newSliderButtonOffset = getMouseOffset(getGuiID()).x - mouseOffset - label->getWidth();
		if (newSliderButtonOffset > getWidth() - sliderButtonWidth - label->getWidth()) newSliderButtonOffset = getWidth() - sliderButtonWidth - label->getWidth();
		if (newSliderButtonOffset < 0) newSliderButtonOffset = 0;
		sliderButtonOffsetX = newSliderButtonOffset;
		computeSliderButtonModelMatrix();
		computeValueFromSliderButtonPosition();
	}

	template<typename T>
	inline GuiSlider<T>::GuiSlider(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const T& minValue, const T& maxValue, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& labelBackgroundColor, const Color4f& sliderBoxTextColor, const Color4f& SliderBoxBackgroundColor, const Color4f& sliderButtonColor, const double& sliderButtonWidth, const int& border)
		: GuiElement(position, size, resizeMode), variableHandle(nullptr), 
		label(std::make_unique<GuiDynamicTextBox>(Vec2i(0, 0), size, ResizeMode::DO_NOT_RESIZE, label, font, fontSize, labelTextColor, labelBackgroundColor, StaticText::ParseMode::SINGLE_LINE, StaticText::Alignment::LEFT, border, GuiDynamicTextBox::TextScaleMode::DONT_SCALE, GuiDynamicTextBox::SizeHintMode(GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_SIZE), false)),
		variableBox(std::make_unique<GuiDynamicTextBox>(Vec2i(this->label->getMinSize().x, 0), Vec2i(this->label->getMinSize().x < size.x ? size.x - this->label->getMinSize().x : 0, this->label->getMinSize().y), ResizeMode::DO_NOT_RESIZE, toText(minValue), font, fontSize, sliderBoxTextColor, SliderBoxBackgroundColor, StaticText::ParseMode::SINGLE_LINE, StaticText::Alignment::CENTER, border, GuiDynamicTextBox::TextScaleMode::DONT_SCALE, GuiDynamicTextBox::SizeHintMode(GuiDynamicTextBox::SizeHintMode::ARBITRARY), false)),
		sliderButtonOffsetX(0), sliderButtonWidth(sliderButtonWidth), sliderButtonColor(sliderButtonColor), 
		sliderButtonShader(Shader("shaders/gui/simpleTransparentColor.shader")), sliderButtonModelMatrix{}, minValue(minValue), maxValue(maxValue),
		mouseOffset(0)
	{
		this->label->setSize(this->label->getMinSize());
		setSizeInternal(Vec2i(size.x, this->label->getMinSize().y));
		setMinSize(this->label->getMinSize());
		computeSliderButtonModelMatrix();
	}

	template<typename T>
	inline GuiSlider<T>::GuiSlider(GuiVariableHandle<T>& handle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const T& minValue, const T& maxValue, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& labelBackgroundColor, const Color4f& sliderBoxTextColor, const Color4f& SliderBoxBackgroundColor, const Color4f& sliderButtonColor, const double& sliderButtonWidth, const int& border)
		: GuiSlider<T>(position, size, resizeMode, label, minValue, maxValue, font, fontSize, labelTextColor, labelBackgroundColor, sliderBoxTextColor, SliderBoxBackgroundColor, sliderButtonColor, sliderButtonWidth, border)
	{
		setVariableHandle(handle);
	}

	template<typename T>
	inline void GuiSlider<T>::setVariableHandle(GuiVariableHandle<T>& variableHandle)
	{
		if (!this->variableHandle) {
			this->variableHandle = &variableHandle;
			signUpHandle(variableHandle, 0);
			computeSliderButtonPositionFromVariableHandle(false);
		}
	}

	template<typename T>
	inline GuiSlider<T>::GuiSlider(const std::string& label, const T& minValue, const T& maxValue, GuiVariableHandle<T>& handle, const GuiStyle& style)
		: GuiSlider(handle, Vec2i(), style.guiSliderSize, style.guiSliderResizeMode, label, minValue, maxValue, style.defaultFont, style.fontSizeNormal, style.guiSliderLabelTextColor, style.guiSliderLabelBackgroundColor, style.guiSliderVariableBoxTextColor, style.guiSliderVariableBoxBackgroundColor, style.guiSliderButtonColor, style.guiSliderButtonWidth, style.guiTextBoxBorder) {}

	template<typename T>
	inline GuiSlider<T>::GuiSlider(const std::string& label, const T& minValue, const T& maxValue, const GuiStyle& style)
		: GuiSlider(Vec2i(), style.guiSliderSize, style.guiSliderResizeMode, label, minValue, maxValue, style.defaultFont, style.fontSizeNormal, style.guiSliderLabelTextColor, style.guiSliderLabelBackgroundColor, style.guiSliderVariableBoxTextColor, style.guiSliderVariableBoxBackgroundColor, style.guiSliderButtonColor, style.guiSliderButtonWidth, style.guiTextBoxBorder) {}

	template<typename T>
	inline GuiSlider<T>::GuiSlider(const GuiSlider<T>& other) noexcept
		: GuiElement(other), variableHandle(nullptr), label(std::make_unique<GuiDynamicTextBox>(*other.label)),
		variableBox(std::make_unique<GuiDynamicTextBox>(*other.variableBox)), sliderButtonOffsetX(0),
		sliderButtonWidth(other.sliderButtonWidth), sliderButtonColor(other.sliderButtonColor),
		sliderButtonShader(other.sliderButtonShader), sliderButtonModelMatrix{}, minValue(other.minValue),
		maxValue(other.maxValue), mouseOffset(Vec2i())
	{
		computeSliderButtonModelMatrix();
	}

	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(const GuiSlider<T>& other) noexcept
	{
		GuiElement::operator=(other);
		if (variableHandle) signOffHandle(variableHandle);
		variableHandle = nullptr;
		label = std::make_unique<GuiDynamicTextBox>(*other.label);
		variableBox = std::make_unique<GuiDynamicTextBox>(*other.variableBox);
		sliderButtonOffsetX = 0;
		sliderButtonWidth = other.sliderButtonWidth;
		sliderButtonColor = other.sliderButtonColor;
		sliderButtonShader = other.sliderButtonShader;
		minValue = other.minValue;
		maxValue = other.maxValue;
		mouseOffset = Vec2i();
		computeSliderButtonModelMatrix();
	}

	template<typename T>
	inline GuiSlider<T>::GuiSlider(GuiSlider<T>&& other) noexcept
		: GuiElement(std::move(other)), variableHandle(other.variableHandle), label(std::move(other.label)),
		variableBox(std::move(other.variableBox)), sliderButtonOffsetX(other.sliderButtonOffsetX),
		sliderButtonWidth(other.sliderButtonWidth), sliderButtonColor(other.sliderButtonColor),
		sliderButtonShader(other.sliderButtonShader), sliderButtonModelMatrix(other.sliderButtonModelMatrix),
		minValue(other.minValue), maxValue(other.maxValue), mouseOffset(other.mouseOffset)
	{
		// Leave other in a well defined state
		other.variableHandle = nullptr;
		other.label = nullptr;
		other.variableBox = nullptr;
		// update variable handle
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
	}

	template<typename T>
	inline GuiSlider<T>& GuiSlider<T>::operator=(GuiSlider<T>&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		if (variableHandle) signOffHandle(*variableHandle);
		variableHandle = nullptr;
		label = std::move(other.label);
		variableBox = std::move(other.variableBox);
		sliderButtonOffsetX = other.sliderButtonOffsetX;
		sliderButtonWidth = other.sliderButtonWidth;
		sliderButtonColor = other.sliderButtonColor;
		sliderButtonShader = other.sliderButtonShader;
		sliderButtonModelMatrix = other.sliderButtonModelMatrix;
		minValue = other.minValue;
		maxValue = other.maxValue;
		mouseOffset = other.mouseOffset;
		// Leave other in a well defined state
		other.variableHandle = nullptr;
		other.label = nullptr;
		other.variableBox = nullptr;
		// update variable handle
		if (variableHandle) notifyHandleOnGuiElementMove(&other, *variableHandle);
		return *this;
	}

	template<typename T>
	inline GuiSlider<T>::~GuiSlider()
	{
		if (label) signOffWithoutNotifyingParents(label->getGuiID());
		label = nullptr;
		if (variableBox) signOffWithoutNotifyingParents(variableBox->getGuiID());
		variableBox = nullptr;
		if (variableHandle) signOffHandle(*variableHandle);
	}

}