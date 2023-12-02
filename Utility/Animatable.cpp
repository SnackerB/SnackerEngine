#include "Animatable.h"

namespace SnackerEngine
{


	Animatable::Animatable(double duration, std::function<double(double)> animationFunction)
		: duration{ duration }, time{ 0.0 }, animationFunction{ animationFunction } {}

	void Animatable::update(double dt)
	{
		time = std::min(time + dt, duration);
		onAnimate();
	}
	
	void Animatable::setDuration(double duration)
	{
		this->duration = duration;
		update(0.0);
	}

	void Animatable::setTime(double time)
	{
		this->time = time;
		update(0.0);
	}

	void Animatable::setAnimationFunction(std::function<double(double)> animationFunction)
	{
		this->animationFunction = animationFunction;
		update(0.0);
	}

}