#include "Gui/GuiElements/GuiButton.h"
#include "Utility\Keys.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	Color4f GuiButton::defaultDefaultColor = Color4f(0.5f, 0.5f, 0.7f, 1.0f);
	Color4f GuiButton::defaultHoverColor = scaleRGB(GuiButton::defaultDefaultColor, 0.9f);
	Color4f GuiButton::defaultPressedColor = scaleRGB(GuiButton::defaultDefaultColor, 0.8f); 
	Color4f GuiButton::defaultPressedHoverColor = scaleRGB(GuiButton::defaultDefaultColor, 0.7f); 
	Color4f GuiButton::defaultLockedColor = Color4f(0.5f, 1.0f);
	Color4f GuiButton::defaultDefaultBorderColor = GuiButton::defaultDefaultBorderColor;
	Color4f GuiButton::defaultHoverBorderColor = GuiButton::defaultHoverBorderColor;
	Color4f GuiButton::defaultPressedBorderColor = GuiButton::defaultPressedBorderColor;
	Color4f GuiButton::defaultPressedHoverBorderColor = GuiButton::defaultPressedHoverBorderColor;
	Color4f GuiButton::defaultLockedBorderColor = GuiButton::defaultLockedBorderColor;
	//--------------------------------------------------------------------------------------------------
	void GuiButton::onButtonPress()
	{
		observableButtonPressed.trigger();
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const Vec2i& position, const Vec2i& size, const std::string& text)
		: GuiTextBox(position, size, text)
	{
		if (size.y == 0 && !text.empty()) {
			setSizeHintModePreferredSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setSizeHintModeMinSize(SizeHintMode::SET_TO_TEXT_HEIGHT);
			setResizeMode(ResizeMode::RESIZE_RANGE);
		}
		setAlignmentHorizontal(AlignmentHorizontal::CENTER);
		setAlignmentVertical(AlignmentVertical::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const std::string& text)
		: GuiButton(Vec2i(), Vec2i(), text) {}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames)
	{
		parseJsonOrReadFromData(defaultColor, "defaultColor", json, data, parameterNames);
		parseJsonOrReadFromData(hoverColor, "hoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(pressedColor, "pressedColor", json, data, parameterNames);
		parseJsonOrReadFromData(pressedHoverColor, "pressedHoverColor", json, data, parameterNames);
		parseJsonOrReadFromData(lockedColor, "lockedColor", json, data, parameterNames);

		parseJsonOrReadFromData(defaultBorderColor, "defaultBorderColor", json, data, parameterNames);
		parseJsonOrReadFromData(hoverBorderColor, "hoverBorderColor", json, data, parameterNames);
		parseJsonOrReadFromData(pressedBorderColor, "pressedBorderColor", json, data, parameterNames);
		parseJsonOrReadFromData(pressedHoverBorderColor, "pressedHoverBorderColor", json, data, parameterNames);
		parseJsonOrReadFromData(lockedBorderColor, "lockedBorderColor", json, data, parameterNames);

		parseJsonOrReadFromData(locked, "locked", json, data, parameterNames);
		if (!json.contains("alignmentHorizontal")) setAlignmentHorizontal(AlignmentHorizontal::CENTER);
		if (!json.contains("alignmentVertical")) setAlignmentVertical(AlignmentVertical::CENTER);
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::~GuiButton() {}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(const GuiButton& other) noexcept
		: GuiTextBox(other), observableButtonPressed{}, defaultColor(other.defaultColor),
		hoverColor(other.hoverColor), pressedColor(other.pressedColor), pressedHoverColor(other.pressedHoverColor), 
		lockedColor(other.lockedColor), defaultBorderColor(other.defaultBorderColor), hoverBorderColor(other.hoverBorderColor), 
		pressedBorderColor(other.pressedBorderColor), pressedHoverBorderColor(other.pressedHoverBorderColor),
		lockedBorderColor(other.lockedBorderColor), isBeingPressed(false), isBeingHovered(false), locked(other.locked) {}
	//--------------------------------------------------------------------------------------------------
	GuiButton& GuiButton::operator=(const GuiButton& other) noexcept
	{
		GuiTextBox::operator=(other);
		observableButtonPressed = EventHandle::Observable();
		defaultColor = other.defaultColor;
		hoverColor = other.hoverColor;
		pressedColor = other.pressedColor;
		pressedHoverColor = other.pressedHoverColor;
		lockedColor = other.lockedColor;
		defaultBorderColor = other.defaultBorderColor;
		hoverBorderColor = other.hoverBorderColor;
		pressedBorderColor = other.pressedBorderColor;
		pressedHoverBorderColor = other.pressedHoverBorderColor;
		lockedBorderColor = other.lockedBorderColor;
		isBeingPressed = false;
		isBeingHovered = false;
		locked = other.locked;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiButton::GuiButton(GuiButton&& other) noexcept
		: GuiTextBox(std::move(other)), observableButtonPressed(std::move(other.observableButtonPressed)), defaultColor(std::move(other.defaultColor)),
		hoverColor(std::move(other.hoverColor)), pressedColor(std::move(other.pressedColor)), pressedHoverColor(std::move(other.pressedHoverColor)), lockedColor(std::move(other.lockedColor)),
		defaultBorderColor(std::move(other.defaultBorderColor)), hoverBorderColor(std::move(other.hoverBorderColor)), pressedBorderColor(std::move(other.pressedBorderColor)), 
		pressedHoverBorderColor(std::move(other.pressedHoverBorderColor)), lockedBorderColor(std::move(other.lockedBorderColor)),
		isBeingPressed(std::move(other.isBeingPressed)), isBeingHovered(std::move(other.isBeingHovered)), locked(std::move(other.locked)) {}
	//--------------------------------------------------------------------------------------------------
	GuiButton& GuiButton::operator=(GuiButton&& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		observableButtonPressed = std::move(other.observableButtonPressed);
		defaultColor = std::move(other.defaultColor);
		hoverColor = std::move(other.hoverColor);
		pressedColor = std::move(other.pressedColor);
		pressedHoverColor = std::move(other.pressedHoverColor);
		lockedColor = std::move(other.lockedColor);
		defaultBorderColor = std::move(other.defaultBorderColor);
		hoverBorderColor = std::move(other.hoverBorderColor);
		pressedBorderColor = std::move(other.pressedBorderColor);
		pressedHoverBorderColor = std::move(other.pressedHoverBorderColor);
		lockedBorderColor = std::move(other.lockedBorderColor);
		isBeingPressed = std::move(other.isBeingPressed);
		isBeingHovered = std::move(other.isBeingHovered);
		locked = std::move(other.locked);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::subscribeToEventButtonPress(EventHandle& eventHandle)
	{
		observableButtonPressed.subscribe(eventHandle);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::lock()
	{
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		isBeingPressed = false;
		isBeingHovered = false;
		locked = true;
		setBackgroundColor(lockedColor);
		setBorderColor(lockedBorderColor);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::unlock()
	{
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		locked = false;
		if (isBeingHovered) {
			setBackgroundColor(hoverColor);
			setBorderColor(hoverBorderColor);
		}
		else {
			setBackgroundColor(defaultColor);
			setBorderColor(defaultBorderColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setDefaultColor(const Color4f& defaultColor)
	{
		this->defaultColor = defaultColor;
		if (locked == false && isBeingHovered == false && isBeingPressed == false) {
			setBackgroundColor(this->defaultColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setHoverColor(const Color4f& hoverColor)
	{
		this->hoverColor = hoverColor;
		if (locked == false && isBeingHovered == true && isBeingPressed == false) {
			setBackgroundColor(this->hoverColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setPressedColor(const Color4f& pressedColor)
	{
		this->pressedColor = pressedColor;
		if (locked == false && isBeingHovered == false && isBeingPressed == true) {
			setBackgroundColor(this->pressedColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setPressedHoverColor(const Color4f& pressedHoverColor)
	{
		this->pressedHoverColor = pressedHoverColor;
		if (locked == false && isBeingHovered == true && isBeingPressed == true) {
			setBackgroundColor(this->pressedHoverColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setLockedColor(const Color4f& lockedColor)
	{
		this->lockedColor = lockedColor;
		if (locked) {
			setBackgroundColor(this->lockedColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setDefaultBorderColor(const Color4f& defaultBorderColor)
	{
		this->defaultBorderColor = defaultBorderColor;
		if (locked == false && isBeingHovered == false && isBeingPressed == false) {
			setBorderColor(this->defaultBorderColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setHoverBorderColor(const Color4f& hoverBorderColor)
	{
		this->hoverBorderColor = hoverBorderColor;
		if (locked == false && isBeingHovered == true && isBeingPressed == false) {
			setBorderColor(this->hoverBorderColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setPressedBorderColor(const Color4f& pressedBorderColor)
	{
		this->pressedBorderColor = pressedBorderColor;
		if (locked == false && isBeingHovered == false && isBeingPressed == true) {
			setBorderColor(this->pressedBorderColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setPressedHoverBorderColor(const Color4f& pressedHoverBorderColor)
	{
		this->pressedHoverBorderColor = pressedHoverBorderColor;
		if (locked == false && isBeingHovered == true && isBeingPressed == true) {
			setBorderColor(this->pressedHoverBorderColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::setLockedBorderColor(const Color4f& lockedBorderColor)
	{
		this->lockedBorderColor = lockedBorderColor;
		if (locked) {
			setBorderColor(this->lockedBorderColor);
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animateDefaultColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonDefaultColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setDefaultColor(currentVal); }
		public:
			GuiButtonDefaultColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonDefaultColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animateHoverColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonHoverColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setHoverColor(currentVal); }
		public:
			GuiButtonHoverColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonHoverColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animatePressedColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonPressedColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setPressedColor(currentVal); }
		public:
			GuiButtonPressedColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonPressedColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animatePressedHoverColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonPressedHoverColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setPressedHoverColor(currentVal); }
		public:
			GuiButtonPressedHoverColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonPressedHoverColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animateLockedColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonLockedColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setLockedColor(currentVal); }
		public:
			GuiButtonLockedColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonLockedColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animateDefaultBorderColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonDefaultBorderColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setDefaultBorderColor(currentVal); }
		public:
			GuiButtonDefaultBorderColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonDefaultBorderColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animateHoverBorderColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonHoverBorderColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setHoverBorderColor(currentVal); }
		public:
			GuiButtonHoverBorderColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonHoverBorderColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animatePressedBorderColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonPressedBorderColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setPressedBorderColor(currentVal); }
		public:
			GuiButtonPressedBorderColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonPressedBorderColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animatePressedHoverBorderColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonPressedHoverBorderColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setPressedHoverBorderColor(currentVal); }
		public:
			GuiButtonPressedHoverBorderColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonPressedHoverBorderColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiButton::animateLockedBorderColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiButtonLockedBorderColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiButton*>(element)->setLockedBorderColor(currentVal); }
		public:
			GuiButtonLockedBorderColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiButtonLockedBorderColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::onRegister()
	{
		GuiTextBox::onRegister();
		setBackgroundColor(defaultColor);
		setBorderColor(defaultBorderColor);
		if (!locked) {
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		}
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE)
		{
			if (isBeingHovered)
			{
				onButtonPress();
				setBackgroundColor(hoverColor);
				setBorderColor(hoverBorderColor);
			}
			else
			{
				setBackgroundColor(defaultColor);
				setBorderColor(defaultBorderColor);
			}
			isBeingPressed = false;
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			isBeingPressed = true;
			setBackgroundColor(pressedHoverColor);
			setBorderColor(pressedHoverBorderColor);
			signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseEnter(const Vec2d& position)
	{
		isBeingHovered = true;
		if (!locked) {
			if (isBeingPressed) {
				setBackgroundColor(pressedHoverColor);
				setBorderColor(pressedHoverBorderColor);
			}
			else {
				setBackgroundColor(hoverColor);
				setBorderColor(hoverBorderColor);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiButton::callbackMouseLeave(const Vec2d& position)
	{
		isBeingHovered = false;
		if (!locked) {
			if (isBeingPressed) {
				setBackgroundColor(pressedColor);
				setBorderColor(pressedBorderColor);
			}
			else {
				setBackgroundColor(defaultColor);
				setBorderColor(defaultBorderColor);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
}