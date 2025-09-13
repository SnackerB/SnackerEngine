#pragma once

#include "Gui\GuiElements\GuiButton.h"

namespace SnackerEngine
{
	class GuiScaleButton : public GuiButton
	{
	private:
		/// Button scale: 1.0f is 100% scale (and the default scale when
		/// the user is not hovering or pressing)
		float currentButtonScale = 1.0f;
		float hoverButtonScale = 1.1f;
		float pressedButtonScale = 1.0f;
		float pressedHoverButtonScale = 0.9f;
		/// Transform matrix used to rescale the button when drawing
		Mat4f transformMatrix{};
		Vec2i clippingBoxOffset{};
		Vec2i clippingBoxSize{};
		/// Computes the transform matrix and the clippingBox offset and size
		void computeTransformMatrixAndClippingBox();
		/// Sets the current button scale and recomputes transform matrix
		void setCurrentButtonScale(float currentButtonScale);
		/// Animation duration in seconds
		double scaleAnimationDuration = 0.05;
		/// Animation functions
		AnimationFunction::AnimationFunction animationFunctionIncreaseScale = AnimationFunction::linear;
		AnimationFunction::AnimationFunction animationFunctionDecreaseScale = AnimationFunction::linear;
		/// Animatable for animating currentButtonScale, used internally
		friend class GuiScaleButtonCurrentButtonScaleAnimatable;
		class GuiScaleButtonCurrentButtonScaleAnimatable : public GuiElementValueAnimatable<float> {
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiScaleButton*>(element)->setCurrentButtonScale(currentVal); }
		public:
			GuiScaleButtonCurrentButtonScaleAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		/// Helper function that returns the correct animation function for the given goalButtonScale
		AnimationFunction::AnimationFunction selectAnimationFunction(float goalButtonScale);
		/// Helper function that starts the button animation
		void animateButtonScale(float goalButtonScale);
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SCALE_BUTTON";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiScaleButton(const Vec2i& position = Vec2i{}, const Vec2i& size = Vec2i{}, const std::string& label = "");
		GuiScaleButton(const std::string& label = "");
		/// Constructor from JSON
		GuiScaleButton(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiScaleButton() {}
		/// Setters
		void setHoverButtonScale(float hoverButtonScale);
		void setPressedButtonScale(float pressedButtonScale);
		void setPressedHoverButtonScale(float pressedHoverButtonScale);
		void setScaleAnimationDuration(double scaleAnimationDuration) { this->scaleAnimationDuration = scaleAnimationDuration; }
		void setAnimationFunctionIncreaseScale(AnimationFunction::AnimationFunction animationFunctionIncreaseScale) { this->animationFunctionIncreaseScale = animationFunctionIncreaseScale; }
		void setAnimationFunctionDecreaseScale(AnimationFunction::AnimationFunction animationFunctionDecreaseScale) { this->animationFunctionDecreaseScale = animationFunctionDecreaseScale; }
		/// Getters
		float getHoverButtonScale() const { return hoverButtonScale; }
		float getPressedButtonScale() const { return pressedButtonScale; }
		float getPressedHoverButtonScale() const { return pressedHoverButtonScale; }
		double getScaleAnimationDuration() const { return scaleAnimationDuration; }
		const AnimationFunction::AnimationFunction& getAnimationFunctionIncreaseScale() { return animationFunctionIncreaseScale; }
		const AnimationFunction::AnimationFunction& getAnimationFunctionDecreaseScale() { return animationFunctionDecreaseScale; }
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for the mouse entering the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseEnter(const Vec2d& position) override;
		/// Callback function for the mouse leaving the element. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseLeave(const Vec2d& position) override;
	};	
}