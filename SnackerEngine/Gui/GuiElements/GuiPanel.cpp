#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	Color4f GuiPanel::defaultBackgroundColor = Color4f(0.0f, 1.0f);
	Color4f GuiPanel::defaultBorderColor = Color4f(1.0f, 1.0f);
	Shader GuiPanel::defaultBackgroundShader{};
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const Color4f& backgroundColor,
		const Color4f& borderColor, float roundedCorners, float borderThickness)
		: GuiElement(position, size, resizeMode), backgroundColor{ backgroundColor },
		borderColor{ borderColor }, roundedCorners { roundedCorners }, borderThickness{ borderThickness }
	{
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiElement(json, data, parameterNames)
	{
		parseJsonOrReadFromData(backgroundColor, "backgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(borderColor, "borderColor", json, data, parameterNames);
		std::optional<Vec4f> roundedCornersVecf = parseJsonOrReadFromData<Vec4f>("roundedCorners", json, data, parameterNames);
		if (roundedCornersVecf.has_value()) roundedCorners = roundedCornersVecf.value();
		std::optional<Vec4i> roundedCornersVeci = parseJsonOrReadFromData<Vec4i>("roundedCorners", json, data, parameterNames);
		if (roundedCornersVeci.has_value()) roundedCorners = static_cast<Vec4f>(roundedCornersVeci.value());
		std::optional<float> roundedCornersFloat = parseJsonOrReadFromData<float>("roundedCorners", json, data, parameterNames);
		if (roundedCornersFloat.has_value()) roundedCorners = Vec4f(roundedCornersFloat.value());
		parseJsonOrReadFromData(roundedCorners[0], "roundedCornersTopLeft", json, data, parameterNames);
		parseJsonOrReadFromData(roundedCorners[1], "roundedCornersTopRight", json, data, parameterNames);
		parseJsonOrReadFromData(roundedCorners[2], "roundedCornersBottomRight", json, data, parameterNames);
		parseJsonOrReadFromData(roundedCorners[3], "roundedCornersBottomLeft", json, data, parameterNames);
		parseJsonOrReadFromData(borderThickness, "borderThickness", json, data, parameterNames);
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(const GuiPanel& other) noexcept
		: GuiElement(other), backgroundColor(other.backgroundColor), borderColor(other.borderColor),
		modelMatrix(other.modelMatrix), shader(other.shader), 
		roundedCorners(other.roundedCorners), borderThickness(other.borderThickness)
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel& GuiPanel::operator=(const GuiPanel& other) noexcept
	{
		GuiElement::operator=(other);
		backgroundColor = other.backgroundColor;
		borderColor = other.borderColor;
		modelMatrix = other.modelMatrix;
		shader = other.shader;
		roundedCorners = other.roundedCorners;
		borderThickness = other.borderThickness;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel::GuiPanel(GuiPanel&& other) noexcept
		: GuiElement(std::move(other)), backgroundColor(std::move(other.backgroundColor)), 
		borderColor(std::move(other.borderColor)), modelMatrix(std::move(other.modelMatrix)), 
		shader(std::move(other.shader)), roundedCorners(std::move(other.roundedCorners)), 
		borderThickness(std::move(other.borderThickness))
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiPanel& GuiPanel::operator=(GuiPanel&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		backgroundColor = std::move(other.backgroundColor);
		borderColor = std::move(other.borderColor);
		modelMatrix = std::move(other.modelMatrix);
		shader = std::move(other.shader);
		roundedCorners = std::move(other.roundedCorners);
		borderThickness = std::move(other.borderThickness);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setBorderColor(const Color4f& borderColor)
	{
		this->borderColor = borderColor;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setRoundedCorners(float roundedCorners)
	{
		this->roundedCorners = Vec4f(roundedCorners);
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setRoundedCorners(const Vec4f& roundedCorners)
	{
		this->roundedCorners = roundedCorners;
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setRoundedCornersTopLeft(float roundedCornersTopLeft)
	{
		this->roundedCorners[0] = roundedCornersTopLeft;
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setRoundedCornersTopRight(float roundedCornersTopRight)
	{
		this->roundedCorners[1] = roundedCornersTopRight;
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setRoundedCornersBottomRight(float roundedCornersBottomRight)
	{
		this->roundedCorners[2] = roundedCornersBottomRight;
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setRoundedCornersBottomLeft(float roundedCornersBottomLeft)
	{
		this->roundedCorners[2] = roundedCornersBottomLeft;
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::setBorderThickness(float borderThickness)
	{
		this->borderThickness = borderThickness;
		selectShader();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::draw(const Vec2i& worldPosition, const Mat4f& transformMatrix)
	{
		drawBackground(worldPosition, transformMatrix);
		GuiElement::draw(worldPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::draw(const Vec2i& worldPosition)
	{
		drawBackground(worldPosition);
		GuiElement::draw(worldPosition);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::onRegister()
	{
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::onPositionChange()
	{
		GuiElement::onPositionChange();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::onSizeChange()
	{
		GuiElement::onSizeChange();
		computeModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiPanel::isColliding(const Vec2i& offset) const
	{
		IsCollidingResult result = GuiElement::isColliding(offset);
		if (!roundedCorners.isZero() && result == IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT) {
			const Vec2i& size = getSize();
			if (offset.x < roundedCorners[0] && offset.y < roundedCorners[0]) {
				// Top left
				if ((offset - Vec2i(roundedCorners[0], roundedCorners[0])).squaredMagnitude() > roundedCorners[0] * roundedCorners[0]) 
					return IsCollidingResult::NOT_COLLIDING;
			}
			if (result == IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT && offset.x > size.x - roundedCorners[1] && offset.y < roundedCorners[1]) {
				// Top right
				if ((offset- Vec2i(size.x - roundedCorners[1], roundedCorners[1])).squaredMagnitude() > roundedCorners[1] * roundedCorners[1])
					return IsCollidingResult::NOT_COLLIDING;
			}
			if (result == IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT && offset.x > size.x - roundedCorners[2] && offset.y > size.y - roundedCorners[2]) {
				// Bottom right
				if ((offset - Vec2i(size.x - roundedCorners[2], size.y - roundedCorners[2])).squaredMagnitude() > roundedCorners[2] * roundedCorners[2])
					return IsCollidingResult::NOT_COLLIDING;
			}
			if (result == IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT && offset.x < roundedCorners[3] && offset.y > size.y - roundedCorners[3]) {
				// Bottom left
				if ((offset - Vec2i(roundedCorners[3], size.y - roundedCorners[3])).squaredMagnitude() > roundedCorners[3] * roundedCorners[3])
					return IsCollidingResult::NOT_COLLIDING;
			}
		}
		if (result == IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT && getBackgroundColor().alpha == 0) return IsCollidingResult::COLLIDE_CHILD;
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelBackgroundColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setBackgroundColor(currentVal); };
		public:
			GuiPanelBackgroundColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelBackgroundColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateBorderColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelborderColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setBorderColor(currentVal); };
		public:
			GuiPanelborderColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelborderColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateRoundedCorners(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelRoundedCornersAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setRoundedCorners(currentVal); };
		public:
			GuiPanelRoundedCornersAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelRoundedCornersAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateRoundedCorners(const Vec4f& startVal, const Vec4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelRoundedCornersAnimatable : public GuiElementValueAnimatable<Vec4f>
		{
			virtual void onAnimate(const Vec4f& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setRoundedCorners(currentVal); };
		public:
			GuiPanelRoundedCornersAnimatable(GuiElement& element, const Vec4f& startVal, const Vec4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Vec4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelRoundedCornersAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateRoundedCornersTopLeft(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelRoundedCornersTopLeftAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setRoundedCornersTopLeft(currentVal); };
		public:
			GuiPanelRoundedCornersTopLeftAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelRoundedCornersTopLeftAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateRoundedCornersTopRight(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelRoundedCornersTopRightAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setRoundedCornersTopRight(currentVal); };
		public:
			GuiPanelRoundedCornersTopRightAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelRoundedCornersTopRightAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateRoundedCornersBottomRight(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelRoundedCornersBottomRightAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setRoundedCornersBottomRight(currentVal); };
		public:
			GuiPanelRoundedCornersBottomRightAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelRoundedCornersBottomRightAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateRoundedCornersBottomLeft(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelRoundedCornersBottomLeftAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setRoundedCornersBottomLeft(currentVal); };
		public:
			GuiPanelRoundedCornersBottomLeftAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelRoundedCornersBottomLeftAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiPanel::animateBorderThickness(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiPanelborderThicknessAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiPanel*>(element)->setBorderThickness(currentVal); };
		public:
			GuiPanelborderThicknessAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiPanelborderThicknessAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::selectShader()
	{
		if (roundedCorners.isZero()) {
			if (borderThickness == 0.0f) this->shader = defaultBackgroundShader;
			else this->shader = Shader("shaders/gui/simpleAlphaColorBorders.shader");
		}
		else if (roundedCorners.allEqual()) {
			if (borderThickness == 0.0f) this->shader = Shader("shaders/gui/simpleAlphaColorRoundedCorners.shader");
			else this->shader = Shader("shaders/gui/simpleAlphaColorRoundedCornersBorders.shader");
		}
		else {
			if (borderThickness == 0.0f) this->shader = Shader("shaders/gui/simpleAlphaColorDifferentRoundedCorners.shader");
			else this->shader = Shader("shaders/gui/simpleAlphaColorDifferentRoundedCornersBorders.shader");
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::computeModelMatrix()
	{
		const Vec2i& position = getPosition();
		const Vec2i& size = getSize();
		modelMatrix = Mat4f::TranslateAndScale(
			Vec3f(0.0f, static_cast<float>(-size.y), 0.0f),
			Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::drawBackground(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (backgroundColor.alpha != 0.0f || borderColor.alpha != 0.0f)
		{
			shader.bind();
			guiManager->setUniformViewAndProjectionMatrices(shader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			shader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrix);
			shader.setUniform<Color4f>("u_color", backgroundColor);
			if (roundedCorners != 0.0f || borderThickness != 0.0f) {
				shader.setUniform<Vec2f>("u_size", getSize());
				if (!roundedCorners.isZero()) {
					if (roundedCorners.allEqual()) shader.setUniform<float>("u_roundedCorners", roundedCorners[0]);
					else shader.setUniform<Vec4f>("u_roundedCorners", roundedCorners);
				}
				if (borderThickness != 0.0f) {
					shader.setUniform<float>("u_borderThickness", borderThickness);
					shader.setUniform<Color4f>("u_borderColor", borderColor);
				}
			}
			Renderer::draw(guiManager->getModelSquare());
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiPanel::drawBackground(const Vec2i& worldPosition, const Mat4f& transformMatrix)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (backgroundColor.alpha != 0.0f || borderColor.alpha != 0.0f)
		{
			shader.bind();
			guiManager->setUniformViewAndProjectionMatrices(shader);
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			shader.setUniform<Mat4f>("u_model", translationMatrix * transformMatrix * modelMatrix);
			shader.setUniform<Color4f>("u_color", backgroundColor);
			if (roundedCorners != 0.0f || borderThickness != 0.0f) {
				shader.setUniform<Vec2f>("u_size", getSize());
				if (!roundedCorners.isZero()) {
					if (roundedCorners.allEqual()) shader.setUniform<float>("u_roundedCorners", roundedCorners[0]);
					else shader.setUniform<Vec4f>("u_roundedCorners", roundedCorners);
				}
				if (borderThickness != 0.0f) {
					shader.setUniform<float>("u_borderThickness", borderThickness);
					shader.setUniform<Color4f>("u_borderColor", borderColor);
				}
			}
			Renderer::draw(guiManager->getModelSquare());
		}
	}
	//--------------------------------------------------------------------------------------------------
}