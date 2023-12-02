#pragma once
#include "Utility/Animatable.h"

namespace SnackerEngine
{

	/// Forward declaration of GuiElement
	class GuiElement;

	class GuiElementAnimatable : public Animatable
	{
	protected:
		friend class GuiElement;
		/// Pointer to the element we want to animate an attriubte of
		GuiElement* element;
		/// Creates a new GuiElementAnimatable for the given element
		GuiElementAnimatable(GuiElement& element, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
	public:
		/// Copy constructor and assignment operator
		GuiElementAnimatable(const GuiElementAnimatable& other) noexcept;
		GuiElementAnimatable& operator=(const GuiElementAnimatable& other) noexcept;
		/// Move constructor and assignment operator
		GuiElementAnimatable(GuiElementAnimatable&& other) noexcept;
		GuiElementAnimatable& operator=(GuiElementAnimatable& other) noexcept;
		/// Returns true if element is nullptr
		bool isNull() { return element == nullptr; }
		/// Destructor
		~GuiElementAnimatable();
	};

	template<typename T>
	class GuiElementValueAnimatable : public GuiElementAnimatable
	{
	private:
		T startVal;
		T stopVal;
	protected:
		/// This function is called whenever the current value changes. Can be overwritten
		virtual void onAnimate(const T& currentVal) {};
	public:
		GuiElementValueAnimatable(GuiElement& element, const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		/// Updates the animation with timestep dt. Can be overwritten for custom animation procedure.
		virtual void update(double dt) override;
		/// Getters
		const T& getStartVal() const { return startVal; }
		const T& getStopVal() const { return stopVal; }
		/// Setters
		void setStartVal(const T& startVal);
		void setStopVal(const T& stopVal);
	};

	template<typename T>
	GuiElementValueAnimatable<T>::GuiElementValueAnimatable(GuiElement& element, const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
		: GuiElementAnimatable(element, duration, animationFunction), startVal{ startVal }, stopVal{ stopVal } {}

	template<typename T>
	void GuiElementValueAnimatable<T>::update(double dt)
	{
		time = std::min(time + dt, duration);
		double percentage = animationFunction(time / duration);
		onAnimate(startVal + (stopVal - startVal) * percentage);
	}

	template<typename T>
	inline void GuiElementValueAnimatable<T>::setStartVal(const T& startVal)
	{
		this->startVal = startVal;
		update(0.0);
	}

	template<typename T>
	inline void GuiElementValueAnimatable<T>::setStopVal(const T& stopVal)
	{
		this->stopVal = stopVal;
		update(0.0);
	}

}