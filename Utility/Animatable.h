#pragma once

#include <functional>
#include "AnimationFunctions.h"

namespace SnackerEngine
{

	template<typename T>
	class Animatable
	{
	private:
		T startVal;
		T stopVal;
		double duration;
		double time;
		std::function<double(double)> animationFunction;
	protected:
		/// This function is called whenever the current value changes. Can be overwritten
		virtual void onAnimate(const T& currentVal) {};
	public:
		Animatable(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		/// Updates the animation with timestep dt. Can be overwritten for custom animation procedure.
		virtual void update(double dt);
		/// Returns true when the animation has fhinished
		bool isFinished() const { return time >= duration; }
		/// Getters
		const T& getStartVal() const { return startVal; }
		const T& getStopVal() const { return stopVal; }
		double getDuration() const { return duration; }
		double getTime() const { return time; }
		const std::function<double(double)>& getAnimationFunction() const { return animationFunction; }
		/// Setters
		void setStartVal(const T& startVal);
		void setStopVal(const T& stopVal);
		void setDuration(double duration);
		void setTime(double time);
		void setAnimationFunction(std::function<double(double)> animationFunction);
	};

	template<typename T>
	inline Animatable<T>::Animatable(const T& startVal, const T& stopVal, double duration, std::function<double(double)> animationFunction)
		: startVal{ startVal }, stopVal{ stopVal }, duration{ duration }, time{ 0.0 }, animationFunction{ animationFunction }
	{
	}

	template<typename T>
	inline void Animatable<T>::update(double dt)
	{
		time = std::min(time + dt, duration);
		double percentage = animationFunction(duration - time);
		onAnimate(startVal + (stopVal - startVal) * percentage);
	}

	template<typename T>
	inline void Animatable<T>::setStartVal(const T& startVal)
	{
		this->startVal = startVal;
		update(0.0);
	}

	template<typename T>
	inline void Animatable<T>::setStopVal(const T& stopVal)
	{
		this->stopVal = stopVal;
		update(0.0);
	}

	template<typename T>
	inline void Animatable<T>::setDuration(double duration)
	{
		this->duration = duration;
		update(0.0);
	}

	template<typename T>
	inline void Animatable<T>::setTime(double time)
	{
		this->time = time;
		update(0.0);
	}

	template<typename T>
	inline void Animatable<T>::setAnimationFunction(std::function<double(double)> animationFunction)
	{
		this->animationFunction = animationFunction;
		update(0.0);
	}

}