#pragma once

#include <functional>
#include "AnimationFunctions.h"

namespace SnackerEngine
{

	class Animatable
	{
	protected:
		double duration;
		double time;
		std::function<double(double)> animationFunction;
		/// This function is called whenever the current value changes. Can be overwritten
		virtual void onAnimate() {};
	public:
		Animatable(double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		/// Updates the animation with timestep dt. Can be overwritten for custom animation procedure.
		virtual void update(double dt);
		/// Returns true when the animation has fhinished
		bool isFinished() const { return time >= duration; }
		/// Getters
		double getDuration() const { return duration; }
		double getTime() const { return time; }
		const std::function<double(double)>& getAnimationFunction() const { return animationFunction; }
		/// Setters
		void setDuration(double duration);
		void setTime(double time);
		void setAnimationFunction(std::function<double(double)> animationFunction);
	};

	template<typename T>
	class ValueAnimatable : public Animatable
	{
	private:
		T startVal;
		T stopVal;
	protected:
		/// This function is called whenever the current value changes. Can be overwritten
		virtual void onAnimate(const T& currentVal) {};
	public:
		ValueAnimatable(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
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
	inline ValueAnimatable<T>::ValueAnimatable(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
		: Animatable(duration, animationFunction), startVal{ startVal }, stopVal{ stopVal } {}

	template<typename T>
	inline void ValueAnimatable<T>::update(double dt)
	{
		time = std::min(time + dt, duration);
		double percentage = animationFunction(time / duration);
		onAnimate(startVal + (stopVal - startVal) * percentage);
	}

	template<typename T>
	inline void ValueAnimatable<T>::setStartVal(const T& startVal)
	{
		this->startVal = startVal;
		update(0.0);
	}

	template<typename T>
	inline void ValueAnimatable<T>::setStopVal(const T& stopVal)
	{
		this->stopVal = stopVal;
		update(0.0);
	}


}