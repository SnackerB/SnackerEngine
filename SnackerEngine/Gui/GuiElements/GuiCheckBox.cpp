#include "Gui\GuiElements\GuiCheckBox.h"
#include "Gui\GuiManager.h"
#include "Graphics\Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	int GuiCheckBox::defaultCheckBoxSize{};
	Color4f GuiCheckBox::defaultColorDefaultTrue = Color4f(0.4f, 0.4f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultColorHoverTrue = Color4f(0.35f, 0.35f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultColorPressedTrue = Color4f(0.3f, 0.3f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultColorHoverPressedTrue = Color4f(0.25f, 0.25f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultColorDefaultFalse = Color4f(0.4f, 0.4f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultColorHoverFalse = Color4f(0.35f, 0.35f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultColorPressedFalse = Color4f(0.3f, 0.3f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultColorHoverPressedFalse = Color4f(0.25f, 0.25f, 1.0f, 1.0f);
	Color4f GuiCheckBox::defaultCheckMarkColor = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	Texture GuiCheckBox::defaultCheckMarkTexture{};
	Shader GuiCheckBox::defaultCheckMarkShader{};
	bool GuiCheckBox::defaultDrawCheckMark = true;
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::onButtonPress()
	{
		checked.set(!checked);
		GuiButton::onButtonPress();
		updateButtonColor();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::updateButtonColor()
	{
		if (checked) {
			setDefaultColor(colorDefaultTrue);
			setHoverColor(colorHoverTrue);
			setPressedColor(colorPressedTrue);
			setPressedHoverColor(colorHoverPressedTrue);
		}
		else {
			setDefaultColor(colorDefaultFalse);
			setHoverColor(colorHoverFalse);
			setPressedColor(colorPressedFalse);
			setPressedHoverColor(colorHoverPressedFalse);
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::GuiCheckBox(int size)
		: GuiScaleButton(Vec2i(), Vec2i(size, size), "")
	{
		setSize(Vec2i{ size, size });
		setMinSize(Vec2i{ size, size });
		setPreferredSize(Vec2i{ size, size });
		setMaxSize(Vec2i{ size, size });
		setSizeHintModes({ SizeHintMode::ARBITRARY, SizeHintMode::ARBITRARY , SizeHintMode::ARBITRARY });
		updateButtonColor();
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::GuiCheckBox(const nlohmann::json& json, ParameterNameSet* parameterNameSet)
		: GuiScaleButton::GuiScaleButton(json, parameterNameSet)
	{
		bool temp = false;
		parseJson(temp, "checked", json, parameterNameSet);
		checked.set(temp);
		parseJson(colorDefaultTrue, "colorDefaultTrue", json, parameterNameSet);
		parseJson(colorHoverTrue, "colorHoverTrue", json, parameterNameSet);
		parseJson(colorPressedTrue, "colorPressedTrue", json, parameterNameSet);
		parseJson(colorHoverPressedTrue, "colorHoverPressedTrue", json, parameterNameSet);
		parseJson(colorDefaultFalse, "colorDefaultFalse", json, parameterNameSet);
		parseJson(colorHoverFalse, "colorHoverFalse", json, parameterNameSet);
		parseJson(colorPressedFalse, "colorPressedFalse", json, parameterNameSet);
		parseJson(colorHoverPressedFalse, "colorHoverPressedFalse", json, parameterNameSet);
		parseJson(checkMarkColor, "checkMarkColor", json, parameterNameSet);
		parseJson(checkMarkTexture, "checkMarkTexture", json, parameterNameSet);
		parseJson(checkMarkShader, "checkMarkShader", json, parameterNameSet);
		parseJson(drawCheckMark, "drawCheckMark", json, parameterNameSet);
		parseJson(checkMarkSize, "checkMarkSize", json, parameterNameSet);
		int size = defaultCheckBoxSize;
		parseJson(size, "size", json, parameterNameSet);
		setSize(Vec2i(size));
		setPreferredSize(Vec2i(size));
		setMinSize(Vec2i(size));
		setMaxSize(Vec2i(size));
		auto preferredSize = parseJson<int>("preferredSize", json, parameterNameSet);
		if (preferredSize.has_value()) setPreferredSize(Vec2i(preferredSize.value()));
		auto minSize = parseJson<int>("minSize", json, parameterNameSet);
		if (minSize.has_value()) setMinSize(Vec2i(minSize.value()));
		auto maxSize = parseJson<int>("maxSize", json, parameterNameSet);
		if (maxSize.has_value()) setMaxSize(Vec2i(maxSize.value()));
		if (!json.contains("sizeHintModeMinSize")) setSizeHintModeMinSize(SizeHintMode::ARBITRARY);
		if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(SizeHintMode::ARBITRARY);
		if (!json.contains("sizeHintModeMaxSize")) setSizeHintModeMaxSize(SizeHintMode::ARBITRARY);
		updateButtonColor();
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::GuiCheckBox(const GuiCheckBox& other) noexcept
		: GuiScaleButton(other), checked(other.checked), colorDefaultTrue(other.colorDefaultTrue),
		colorHoverTrue(other.colorHoverTrue), colorPressedTrue(other.colorPressedTrue),
		colorHoverPressedTrue(other.colorHoverPressedTrue), colorDefaultFalse(other.colorDefaultFalse),
		colorHoverFalse(other.colorHoverFalse), colorPressedFalse(other.colorPressedFalse),
		colorHoverPressedFalse(other.colorHoverPressedFalse), checkMarkColor(other.checkMarkColor),
		checkMarkTexture(other.checkMarkTexture), checkMarkShader(other.checkMarkShader), 
		drawCheckMark(other.drawCheckMark), checkMarkSize(other.checkMarkSize)
	{
		checked.parentElement = this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox& GuiCheckBox::operator=(const GuiCheckBox& other) noexcept
	{
		GuiButton::operator=(other);
		checked = other.checked;
		checked.parentElement = this;
		colorDefaultTrue = other.colorDefaultTrue;
		colorHoverTrue = other.colorHoverTrue; 
		colorPressedTrue = other.colorPressedTrue;
		colorHoverPressedTrue = other.colorHoverPressedTrue;
		colorDefaultFalse = other.colorDefaultFalse;
		colorHoverFalse = other.colorHoverFalse;  
		colorPressedFalse = other.colorPressedFalse;
		colorHoverPressedFalse = other.colorHoverPressedFalse;
		checkMarkColor = other.checkMarkColor;
		checkMarkTexture = other.checkMarkTexture;
		checkMarkShader = other.checkMarkShader;
		drawCheckMark = other.drawCheckMark;
		checkMarkSize = other.checkMarkSize;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::GuiCheckBox(GuiCheckBox&& other) noexcept
		: GuiScaleButton(std::move(other)), checked(std::move(other.checked)),
		colorDefaultTrue(std::move(other.colorDefaultTrue)), colorHoverTrue(std::move(other.colorHoverTrue)), 
		colorPressedTrue(std::move(other.colorPressedTrue)), colorHoverPressedTrue(std::move(other.colorHoverPressedTrue)), 
		colorDefaultFalse(std::move(other.colorDefaultFalse)), colorHoverFalse(std::move(other.colorHoverFalse)), 
		colorPressedFalse(std::move(other.colorPressedFalse)), colorHoverPressedFalse(std::move(other.colorHoverPressedFalse)),
		checkMarkColor(std::move(other.checkMarkColor)), checkMarkTexture(std::move(other.checkMarkTexture)),
		checkMarkShader(std::move(other.checkMarkShader)), drawCheckMark(std::move(other.drawCheckMark)), 
		checkMarkSize(std::move(other.checkMarkSize))
	{
		checked.parentElement = this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox& GuiCheckBox::operator=(GuiCheckBox&& other) noexcept
	{
		GuiScaleButton::operator=(std::move(other));
		checked = std::move(other.checked);
		checked.parentElement = this;
		colorDefaultTrue = std::move(other.colorDefaultTrue);
		colorHoverTrue = std::move(other.colorHoverTrue);
		colorPressedTrue = std::move(other.colorPressedTrue);
		colorHoverPressedTrue = std::move(other.colorHoverPressedTrue);
		colorDefaultFalse = std::move(other.colorDefaultFalse);
		colorHoverFalse = std::move(other.colorHoverFalse);
		colorPressedFalse = std::move(other.colorPressedFalse);
		colorHoverPressedFalse = std::move(other.colorHoverPressedFalse);
		checkMarkColor = std::move(other.checkMarkColor);
		checkMarkTexture = std::move(other.checkMarkTexture);
		checkMarkShader = std::move(other.checkMarkShader);
		drawCheckMark = std::move(other.drawCheckMark);
		checkMarkSize = std::move(other.checkMarkSize);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::setChecked(bool checked)
	{
		if (this->checked != checked) {
			this->checked.set(checked);
			updateButtonColor();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::draw(const Vec2i& worldPosition)
	{
		GuiScaleButton::draw(worldPosition);
		// Draw check mark
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (checked && drawCheckMark && checkMarkColor.alpha != 0.0f)
		{
			checkMarkShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(checkMarkShader);
			int trueCheckMarkSize = checkMarkSize;
			if (trueCheckMarkSize < 0) trueCheckMarkSize = std::min(getWidth(), getHeight());
			Mat4f modelMatrix = Mat4f::TranslateAndScale(
				Vec3f(static_cast<float>(getWidth() / 2 - trueCheckMarkSize / 2), static_cast<float>(-trueCheckMarkSize / 2 - getHeight() / 2), 0.0f),
				Vec3f(static_cast<float>(trueCheckMarkSize), static_cast<float>(trueCheckMarkSize), 0.0f));
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			checkMarkShader.setUniform<Mat4f>("u_model", translationMatrix * Mat4f::Translate(getTranslateBackground()) * modelMatrix * Mat4f::Scale(getScaleBackground()));
			checkMarkShader.setUniform<Color4f>("u_color", checkMarkColor);
			checkMarkShader.setUniform<float>("u_pxRange", 4.0);
			checkMarkShader.setUniform<int>("u_msdf", 0);
			checkMarkTexture.bind();
			Renderer::draw(guiManager->getModelSquare());
		}
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorDefaultTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorDefaultTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorDefaultTrue(currentVal); }
		public:
			GuiCheckBoxColorDefaultTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorDefaultTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorHoverTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverTrue(currentVal); }
		public:
			GuiCheckBoxColorHoverTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorHoverTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorPressedTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorPressedTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorPressedTrue(currentVal); }
		public:
			GuiCheckBoxColorPressedTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorPressedTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorHoverPressedTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverPressedTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverPressedTrue(currentVal); }
		public:
			GuiCheckBoxColorHoverPressedTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorHoverPressedTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorDefaultFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorDefaultFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorDefaultFalse(currentVal); }
		public:
			GuiCheckBoxColorDefaultFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorDefaultFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorHoverFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverFalse(currentVal); }
		public:
			GuiCheckBoxColorHoverFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorHoverFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorPressedFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorPressedFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorPressedFalse(currentVal); }
		public:
			GuiCheckBoxColorPressedFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorPressedFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateColorHoverPressedFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverPressedFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverPressedFalse(currentVal); }
		public:
			GuiCheckBoxColorHoverPressedFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxColorHoverPressedFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiCheckBox::animateCheckMarkColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxCheckMarkColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setCheckMarkColor(currentVal); }
		public:
			GuiCheckBoxCheckMarkColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiCheckBoxCheckMarkColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
}