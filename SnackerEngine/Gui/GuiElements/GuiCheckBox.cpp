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
		checked = !checked;
		if (boolHandle) boolHandle->set(checked);
		updateButtonColor();
		GuiButton::onButtonPress();
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
		: GuiButton(Vec2i(), Vec2i(size, size), "")
	{
		updateButtonColor();
		setSize(Vec2i{ size, size });
		setMinSize(Vec2i{ size, size });
		setPreferredSize(Vec2i{ size, size });
		setMaxSize(Vec2i{ size, size });
		setSizeHintModes({ SizeHintMode::ARBITRARY, SizeHintMode::ARBITRARY , SizeHintMode::ARBITRARY });
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::GuiCheckBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		parseJsonOrReadFromData(checked, "checked", json, data, parameterNames);
		parseJsonOrReadFromData(colorDefaultTrue, "colorDefaultTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverTrue, "colorHoverTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorPressedTrue, "colorPressedTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverPressedTrue, "colorHoverPressedTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorDefaultFalse, "colorDefaultFalse", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverFalse, "colorHoverFalse", json, data, parameterNames);
		parseJsonOrReadFromData(colorPressedFalse, "colorPressedFalse", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverPressedFalse, "colorHoverPressedFalse", json, data, parameterNames);
		parseJsonOrReadFromData(checkMarkColor, "checkMarkColor", json, data, parameterNames);
		parseJsonOrReadFromData(checkMarkTexture, "checkMarkTexture", json, data, parameterNames);
		parseJsonOrReadFromData(checkMarkShader, "checkMarkShader", json, data, parameterNames);
		parseJsonOrReadFromData(drawCheckMark, "drawCheckMark", json, data, parameterNames);
		int size = defaultCheckBoxSize;
		parseJsonOrReadFromData(size, "size", json, data, parameterNames);
		setSize(Vec2i(size));
		setPreferredSize(Vec2i(size));
		setMinSize(Vec2i(size));
		setMaxSize(Vec2i(size));
		auto preferredSize = parseJsonOrReadFromData<int>("preferredSize", json, data, parameterNames);
		if (preferredSize.has_value()) setPreferredSize(Vec2i(preferredSize.value()));
		auto minSize = parseJsonOrReadFromData<int>("minSize", json, data, parameterNames);
		if (minSize.has_value()) setMinSize(Vec2i(minSize.value()));
		auto maxSize = parseJsonOrReadFromData<int>("maxSize", json, data, parameterNames);
		if (maxSize.has_value()) setMaxSize(Vec2i(maxSize.value()));
		updateButtonColor();
		if (!json.contains("sizeHintModeMinSize")) setSizeHintModeMinSize(SizeHintMode::ARBITRARY);
		if (!json.contains("sizeHintModePreferredSize")) setSizeHintModePreferredSize(SizeHintMode::ARBITRARY);
		if (!json.contains("sizeHintModeMaxSize")) setSizeHintModeMaxSize(SizeHintMode::ARBITRARY);
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::~GuiCheckBox()
	{
		if (boolHandle) signOffHandle(*boolHandle);
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::GuiCheckBox(const GuiCheckBox& other) noexcept
		: GuiButton(other), checked(other.checked), boolHandle(nullptr), colorDefaultTrue(other.colorDefaultTrue),
		colorHoverTrue(other.colorHoverTrue), colorPressedTrue(other.colorPressedTrue),
		colorHoverPressedTrue(other.colorHoverPressedTrue), colorDefaultFalse(other.colorDefaultFalse),
		colorHoverFalse(other.colorHoverFalse), colorPressedFalse(other.colorPressedFalse),
		colorHoverPressedFalse(other.colorHoverPressedFalse), checkMarkColor(other.checkMarkColor),
		checkMarkTexture(other.checkMarkTexture), checkMarkShader(other.checkMarkShader), 
		drawCheckMark(other.drawCheckMark) {}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox& GuiCheckBox::operator=(const GuiCheckBox& other) noexcept
	{
		GuiButton::operator=(other);
		checked = other.checked; 
		if (boolHandle) signOffHandle(*boolHandle);
		boolHandle = nullptr;
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
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox::GuiCheckBox(GuiCheckBox&& other) noexcept
		: GuiButton(std::move(other)), checked(std::move(other.checked)), boolHandle(std::move(other.boolHandle)), 
		colorDefaultTrue(std::move(other.colorDefaultTrue)), colorHoverTrue(std::move(other.colorHoverTrue)), 
		colorPressedTrue(std::move(other.colorPressedTrue)), colorHoverPressedTrue(std::move(other.colorHoverPressedTrue)), 
		colorDefaultFalse(std::move(other.colorDefaultFalse)), colorHoverFalse(std::move(other.colorHoverFalse)), 
		colorPressedFalse(std::move(other.colorPressedFalse)), colorHoverPressedFalse(std::move(other.colorHoverPressedFalse)),
		checkMarkColor(std::move(other.checkMarkColor)), checkMarkTexture(std::move(other.checkMarkTexture)),
		checkMarkShader(std::move(other.checkMarkShader)), drawCheckMark(std::move(other.drawCheckMark))
	{
		other.boolHandle = nullptr;
		if (boolHandle) notifyHandleOnGuiElementMove(&other, *boolHandle);
	}
	//--------------------------------------------------------------------------------------------------
	GuiCheckBox& GuiCheckBox::operator=(GuiCheckBox&& other) noexcept
	{
		GuiButton::operator=(std::move(other));
		checked = std::move(other.checked);
		if (boolHandle) signOffHandle(*boolHandle);
		boolHandle = std::move(other.boolHandle);
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
		other.boolHandle = nullptr;
		if (boolHandle) notifyHandleOnGuiElementMove(&other, *boolHandle);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::setChecked(bool checked)
	{
		if (this->checked != checked) {
			this->checked = checked;
			updateButtonColor();
			if (boolHandle) boolHandle->set(checked);
		}
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiCheckBox::setBoolHandle(GuiVariableHandle<bool>& boolHandle)
	{
		if (this->boolHandle) return false;
		this->boolHandle = &boolHandle;
		signUpHandle(boolHandle, 1);
		onHandleUpdate(boolHandle);
		return true;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::draw(const Vec2i& worldPosition)
	{
		GuiButton::draw(worldPosition);
		// Draw check mark
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (checked && drawCheckMark && checkMarkColor.alpha != 0.0f)
		{
			checkMarkShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(checkMarkShader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			checkMarkShader.setUniform<Mat4f>("u_model", translationMatrix * GuiPanel::getModelMatrix());
			checkMarkShader.setUniform<Color4f>("u_color", checkMarkColor);
			checkMarkShader.setUniform<float>("u_pxRange", 4.0);
			checkMarkShader.setUniform<int>("u_msdf", 0);
			checkMarkTexture.bind();
			Renderer::draw(guiManager->getModelSquare());
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::onHandleMove(GuiHandle& guiHandle)
	{
		auto result = guiHandle.getHandleID(*this);
		if (result.has_value() && result.value() == 1) boolHandle = static_cast<GuiVariableHandle<bool>*>(&guiHandle);
		else GuiButton::onHandleMove(guiHandle);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::onHandleDestruction(GuiHandle& guiHandle)
	{
		auto result = guiHandle.getHandleID(*this);
		if (result.has_value() && result.value() == boolHandle->getHandleID(*this)) boolHandle = nullptr;
		else GuiButton::onHandleDestruction(guiHandle);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::onHandleUpdate(GuiHandle& guiHandle)
	{
		if (boolHandle) {
			checked = boolHandle->get();
			updateButtonColor();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorDefaultTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorDefaultTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorDefaultTrue(currentVal); }
		public:
			GuiCheckBoxColorDefaultTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorDefaultTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorHoverTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverTrue(currentVal); }
		public:
			GuiCheckBoxColorHoverTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorHoverTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorPressedTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorPressedTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorPressedTrue(currentVal); }
		public:
			GuiCheckBoxColorPressedTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorPressedTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorHoverPressedTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverPressedTrueAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverPressedTrue(currentVal); }
		public:
			GuiCheckBoxColorHoverPressedTrueAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorHoverPressedTrueAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorDefaultFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorDefaultFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorDefaultFalse(currentVal); }
		public:
			GuiCheckBoxColorDefaultFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorDefaultFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorHoverFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverFalse(currentVal); }
		public:
			GuiCheckBoxColorHoverFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorHoverFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorPressedFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorPressedFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorPressedFalse(currentVal); }
		public:
			GuiCheckBoxColorPressedFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorPressedFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateColorHoverPressedFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxColorHoverPressedFalseAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setColorHoverPressedFalse(currentVal); }
		public:
			GuiCheckBoxColorHoverPressedFalseAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxColorHoverPressedFalseAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiCheckBox::animateCheckMarkColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiCheckBoxCheckMarkColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiCheckBox*>(element)->setCheckMarkColor(currentVal); }
		public:
			GuiCheckBoxCheckMarkColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiCheckBoxCheckMarkColorAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
}