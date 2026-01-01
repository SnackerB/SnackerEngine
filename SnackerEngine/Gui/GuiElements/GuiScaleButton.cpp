#include "GuiScaleButton.h"
#include "Utility\Keys.h"
#include "Gui\GuiManager.h"

namespace SnackerEngine
{
	void GuiScaleButton::computeTransformMatrixAndClippingBox()
	{
		Vec2f offset = Vec2f(-static_cast<float>(getWidth()) * (currentButtonScale - 1.0f) / 2.0f,
			static_cast<float>(getHeight()) * (currentButtonScale - 1.0f) / 2.0f);
		clippingBoxOffset = Vec2i(static_cast<int>(std::round(offset.x)), static_cast<int>(std::round(-offset.y)));
		clippingBoxSize = Vec2i(static_cast<int>(std::round(static_cast<float>(getWidth()) * currentButtonScale)), static_cast<int>(std::round(static_cast<float>(getHeight()) * currentButtonScale)));
		transformMatrix = Mat4f::TranslateAndScale(
			Vec3f(offset.x, offset.y, 0),
			Vec3f(currentButtonScale));
	}

	void GuiScaleButton::setCurrentButtonScale(float currentButtonScale)
	{
		this->currentButtonScale = currentButtonScale;
		computeTransformMatrixAndClippingBox();
	}

	AnimationFunction::AnimationFunction GuiScaleButton::selectAnimationFunction(float goalButtonScale)
	{
		if (goalButtonScale > currentButtonScale) return animationFunctionIncreaseScale;
		else return animationFunctionDecreaseScale;
	}

	void GuiScaleButton::animateButtonScale(float goalButtonScale)
	{
		getGuiManager()->signUpAnimatable(std::make_unique<GuiScaleButtonCurrentButtonScaleAnimatable>(*this, currentButtonScale, goalButtonScale, scaleAnimationDuration, selectAnimationFunction(goalButtonScale)));
	}

	GuiScaleButton::GuiScaleButton(const Vec2i& position, const Vec2i& size, const std::string& label)
		: GuiButton(position, size, label)
	{
		computeTransformMatrixAndClippingBox();
	}

	GuiScaleButton::GuiScaleButton(const std::string& label)
		: GuiButton(label)
	{
		computeTransformMatrixAndClippingBox();
	}

	GuiScaleButton::GuiScaleButton(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiButton(json, data, parameterNames)
	{
		parseJsonOrReadFromData(hoverButtonScale, "hoverButtonScale", json, data, parameterNames);
		parseJsonOrReadFromData(pressedButtonScale, "pressedButtonScale", json, data, parameterNames);
		parseJsonOrReadFromData(pressedHoverButtonScale, "pressedHoverButtonScale", json, data, parameterNames);
		parseJsonOrReadFromData(scaleAnimationDuration, "scaleAnimationDuration", json, data, parameterNames);
		parseJsonOrReadFromData(animationFunctionIncreaseScale, "animationFunctionIncreaseScale", json, data, parameterNames);
		parseJsonOrReadFromData(animationFunctionDecreaseScale, "animationFunctionDecreaseScale", json, data, parameterNames);
		computeTransformMatrixAndClippingBox();
	}

	void GuiScaleButton::setHoverButtonScale(float hoverButtonScale)
	{
		this->hoverButtonScale = hoverButtonScale;
		if (isBeingHovered && !isBeingPressed) {
			deleteAnimationsOnElement();
			setCurrentButtonScale(hoverButtonScale);
		}
	}

	void GuiScaleButton::setPressedButtonScale(float pressedButtonScale)
	{
		this->pressedButtonScale = pressedButtonScale;
		if (!isBeingHovered && isBeingPressed) {
			deleteAnimationsOnElement();
			setCurrentButtonScale(pressedButtonScale);
		}
	}

	void GuiScaleButton::setPressedHoverButtonScale(float pressedHoverButtonScale)
	{
		this->pressedHoverButtonScale = pressedHoverButtonScale;
		if (isBeingHovered && isBeingPressed) {
			deleteAnimationsOnElement();
			setCurrentButtonScale(pressedHoverButtonScale);
		}
	}

	void GuiScaleButton::onSizeChange()
	{
		GuiButton::onSizeChange();
		computeTransformMatrixAndClippingBox();
	}

	void GuiScaleButton::draw(const Vec2i& worldPosition)
	{
		GuiButton::draw(worldPosition, transformMatrix, clippingBoxOffset, clippingBoxSize);
	}

	void GuiScaleButton::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE)
		{
			if (isBeingHovered)
			{
				onButtonPress();
				setBackgroundColor(getHoverColor());
				setBorderColor(getHoverBorderColor());
				animateButtonScale(hoverButtonScale);
			}
			else
			{
				setBackgroundColor(getDefaultColor());
				setBorderColor(getDefaultBorderColor());
				animateButtonScale(1.0f);
			}
			isBeingPressed = false;
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	void GuiScaleButton::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			isBeingPressed = true;
			setBackgroundColor(getpressedHoverColor());
			setBorderColor(getpressedHoverBorderColor());
			animateButtonScale(pressedHoverButtonScale);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	void GuiScaleButton::callbackMouseEnter(const Vec2d& position)
	{
		isBeingHovered = true;
		if (!locked) {
			if (isBeingPressed) {
				setBackgroundColor(getpressedHoverColor());
				setBorderColor(getpressedHoverBorderColor());
				animateButtonScale(pressedHoverButtonScale);
			}
			else {
				setBackgroundColor(getHoverColor());
				setBorderColor(getHoverBorderColor());
				animateButtonScale(hoverButtonScale);
			}
		}
	}

	void GuiScaleButton::callbackMouseLeave(const Vec2d& position)
	{
		isBeingHovered = false;
		if (!locked) {
			if (isBeingPressed) {
				setBackgroundColor(getPressedColor());
				setBorderColor(getPressedBorderColor());
				animateButtonScale(pressedButtonScale);
			}
			else {
				setBackgroundColor(getDefaultColor());
				setBorderColor(getDefaultBorderColor());
				animateButtonScale(1.0f);
			}
		}
	}
}