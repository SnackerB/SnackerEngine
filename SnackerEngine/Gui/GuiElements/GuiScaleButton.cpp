#include "GuiScaleButton.h"
#include "Utility\Keys.h"
#include "Gui\GuiManager.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	/// Helper functions for parsing JSON
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiScaleButton::ButtonScaleMode> tag)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "GuiScaleButton option \"ButtonScaleMode\" has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "SCALE_PROPORTIONAL" ||
			json == "SCALE_PIXELS") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid ButtonScaleMode." << LOGGER::ENDL;
		return false;
	}
	template<> GuiScaleButton::ButtonScaleMode parseJSON(const nlohmann::json& json, JsonTag<GuiScaleButton::ButtonScaleMode> tag)
	{
		if (json == "SCALE_PROPORTIONAL") return GuiScaleButton::ButtonScaleMode::SCALE_PROPORTIONAL;
		if (json == "SCALE_PIXELS") return GuiScaleButton::ButtonScaleMode::SCALE_PIXELS;
		return GuiScaleButton::ButtonScaleMode::SCALE_PROPORTIONAL;
	}
	template<> std::string to_string(const GuiScaleButton::ButtonScaleMode& buttonScaleMode)
	{
		switch (buttonScaleMode)
		{
		case GuiScaleButton::ButtonScaleMode::SCALE_PROPORTIONAL: return "SCALE_PROPORTIONAL";
		case GuiScaleButton::ButtonScaleMode::SCALE_PIXELS: return "SCALE_PIXELS";
		default: return "SCALE_PROPORTIONAL";
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiScaleButton::computeTransformMatrixAndClippingBox()
	{
		double textScaleFactor = 1.0f;
		switch (buttonScaleMode) {
		case ButtonScaleMode::SCALE_PROPORTIONAL: {
			textScaleFactor = currentButtonScale;
			translateBackground = Vec2f(-static_cast<float>(getWidth()) * (currentButtonScale - 1.0f) / 2.0f,
				-static_cast<float>(getHeight()) * (currentButtonScale - 1.0f) / 2.0f);
			scaleBackground = Vec2f(currentButtonScale, currentButtonScale);
			clippingBoxOffset = Vec2i(static_cast<int>(std::round(translateBackground.x)), static_cast<int>(std::round(translateBackground.y)));
			clippingBoxSize = Vec2i(static_cast<int>(std::round(static_cast<float>(getWidth()) * currentButtonScale)), static_cast<int>(std::round(static_cast<float>(getHeight()) * currentButtonScale)));
			break;
		}
		case ButtonScaleMode::SCALE_PIXELS:
		{
			float currentButtonScale = static_cast<float>(this->currentButtonScale);
			translateBackground = Vec2f(-currentButtonScale, -currentButtonScale);
			clippingBoxOffset = Vec2i(static_cast<int>(std::round(translateBackground.x)), static_cast<int>(std::round(translateBackground.y)));
			clippingBoxSize = getSize() + Vec2i(static_cast<int>(std::round(2*currentButtonScale)));
			double widthScale = (static_cast<double>(getWidth()) + 2.0 * static_cast<double>(currentButtonScale)) / static_cast<double>(getWidth());
			double heightScale = (static_cast<double>(getHeight()) + 2.0 * static_cast<double>(currentButtonScale)) / static_cast<double>(getHeight());
			scaleBackground = Vec2f(static_cast<float>(widthScale), static_cast<float>(heightScale));
			textScaleFactor = std::min(widthScale, heightScale);
			//infoLogger << LOGGER::BEGIN <<
			//	"widthScale: " << widthScale <<
			//	", heightScale: " << heightScale <<
			//	", textScaleFactor: " << textScaleFactor << LOGGER::ENDL;
			//switch (getAlignmentHorizontal()) {
			//case AlignmentHorizontal::LEFT: translateText.x = translateBackground.x; break;
			//case AlignmentHorizontal::CENTER: translateText.x = -getTextWidth() * (textScaleFactor - 1.0) / 2.0; break;
			//case AlignmentHorizontal::RIGHT: translateText.x = translateBackground.x - getTextWidth() * (textScaleFactor - 1.0); break;
			//}
			//switch (getAlignmentVertical()) {
			//case AlignmentVertical::TOP: translateText.y = translateBackground.y; break;
			//case AlignmentVertical::CENTER: translateText.y = getScaledTextVerticalMidpoint() * (textScaleFactor - 1.0) / 2.0; break;
			//case AlignmentVertical::BOTTOM: translateText.y = -translateBackground.y; break;
			//}
			//scaleText = Vec2f(textScaleFactor, textScaleFactor);
			//infoLogger << LOGGER::BEGIN <<
			//	"offset: " << translateBackground <<
			//	", textOffset: " << translateText << LOGGER::ENDL;
			//break;
		}
		}
		

		switch (getAlignmentHorizontal()) {
		case AlignmentHorizontal::LEFT: translateText.x = translateBackground.x; break;
		case AlignmentHorizontal::CENTER: translateText.x = -getTextWidth() * (textScaleFactor - 1.0f) / 2.0f; break;
		case AlignmentHorizontal::RIGHT: translateText.x = -translateBackground.x - getTextWidth() * (textScaleFactor - 1.0); break;
		}
		switch (getAlignmentVertical()) {
		case AlignmentVertical::TOP: translateText.y = static_cast<float>(getTextHeight() + getScaledBottom()) * (textScaleFactor - 1.0f); break;
		case AlignmentVertical::CENTER: translateText.y = -getScaledTextVerticalMidpoint() * (textScaleFactor - 1.0f); break;
		case AlignmentVertical::BOTTOM: translateText.y = translateBackground.y - getScaledBottom() * (textScaleFactor - 1.0f); break;
		}
		scaleText = Vec2f(textScaleFactor, textScaleFactor);
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

	GuiScaleButton::GuiScaleButton(const nlohmann::json& json, ParameterNameSet* parameterNameSet)
		: GuiButton(json, parameterNameSet)
	{
		parseJson(buttonScaleMode, "buttonScaleMode", json, parameterNameSet);
		parseJson(hoverButtonScale, "hoverButtonScale", json, parameterNameSet);
		parseJson(pressedButtonScale, "pressedButtonScale", json, parameterNameSet);
		parseJson(pressedHoverButtonScale, "pressedHoverButtonScale", json, parameterNameSet);
		parseJson(scaleAnimationDuration, "scaleAnimationDuration", json, parameterNameSet);
		parseJson(animationFunctionIncreaseScale, "animationFunctionIncreaseScale", json, parameterNameSet);
		parseJson(animationFunctionDecreaseScale, "animationFunctionDecreaseScale", json, parameterNameSet);
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

	void GuiScaleButton::setIsBeingPressed(bool isBeingPressed)
	{
		this->isBeingPressed = isBeingPressed;
		if (isBeingHovered) {
			setBackgroundColor(getPressedColor());
			setBorderColor(getPressedBorderColor());
			animateButtonScale(pressedButtonScale);
		}
		else {
			setBackgroundColor(getpressedHoverColor());
			setBorderColor(getpressedHoverBorderColor());
			animateButtonScale(pressedHoverButtonScale);
		}
	}

	void GuiScaleButton::setButtonScaleMode(ButtonScaleMode buttonScaleMode)
	{
		if (this->buttonScaleMode != buttonScaleMode) {
			this->buttonScaleMode = buttonScaleMode;
			computeTransformMatrixAndClippingBox();
		}
	}

	void GuiScaleButton::onSizeChange()
	{
		GuiButton::onSizeChange();
		computeTransformMatrixAndClippingBox();
	}

	void GuiScaleButton::draw(const Vec2i& worldPosition)
	{
		switch (buttonScaleMode) {
		case ButtonScaleMode::SCALE_PROPORTIONAL: 
		{
			//GuiButton::draw(worldPosition, translateBackground, scaleBackground, clippingBoxOffset, clippingBoxSize);
			GuiButton::draw(worldPosition, translateBackground, scaleBackground, translateText, scaleText, clippingBoxOffset, clippingBoxSize);
			break;
		}
		case ButtonScaleMode::SCALE_PIXELS: 
		{
			GuiButton::draw(worldPosition, translateBackground, scaleBackground, translateText, scaleText, clippingBoxOffset, clippingBoxSize);
			break;
		}
		}
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
				switch (buttonScaleMode) {
				case SCALE_PROPORTIONAL: animateButtonScale(1.0f); break;
				case SCALE_PIXELS: animateButtonScale(0.0f); break;
				}
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
				switch (buttonScaleMode) {
				case SCALE_PROPORTIONAL: animateButtonScale(1.0f); break;
				case SCALE_PIXELS: animateButtonScale(0.0f); break;
				}
			}
		}
	}
}