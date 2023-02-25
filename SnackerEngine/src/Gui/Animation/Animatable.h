#pragma once

#include "AnimationFunc.h"

namespace SnackerEngine
{

	class GuiManager;

	class Animatable
	{
	private:
		friend class GuiManager;
		/// The time (in seconds) that passed since the animation was started
		double timePassed;
		/// The time (in seconds) when the animation should be finished
		double timeFinal;
		/// Performs the animation (eg. setting position/color etc.)
		/// This function has to be overloaded by child classes actually implementing animations
		virtual void animate(const float& percentage) {}
	protected:
		/// Protected constructor
		Animatable(const double& duration, AnimationFuncT animationFunction = animationFunctionLinear);
		/// The animation function (map from linear to the goal animation)
		AnimationFuncT animationFunction;
	public:
		/// Advances the animation by the given time. Returns true if the animation is finished.
		bool tick(const float& dt);
	};

	template<typename T>
	class ValueAnimatable : public Animatable
	{
	protected:
		/// The starting value
		T initial;
		/// The final value
		T final;
		/// Protected constructor
		ValueAnimatable(const T& initial, const T & final, const double& duration, AnimationFuncT animationFunction = animationFunctionLinear);
	};

	template<typename T>
	inline ValueAnimatable<T>::ValueAnimatable(const T& initial, const T & final, const double& duration, AnimationFuncT animationFunction)
		: Animatable(duration, animationFunction), initial(initial), final(final) {}

}