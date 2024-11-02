#include "Timer.h"

#include <math.h>
#include <chrono>
#include <format>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	Timer::Timer(const double& timeStep)
		: timeStep(timeStep), timeSinceLastUpdate(0.0) {}
	//------------------------------------------------------------------------------------------------------
	Timer::Timer(const unsigned int& framesPerSecond)
		: timeStep(framesPerSecond == 0 ? 0.0 : 1.0 / double(framesPerSecond)), timeSinceLastUpdate(0.0) {}
	//------------------------------------------------------------------------------------------------------
	void Timer::reset()
	{
		timeSinceLastUpdate = 0.0;
	}
	//------------------------------------------------------------------------------------------------------
	double Timer::getTimeStep() const
	{
		return timeStep;
	}
	//------------------------------------------------------------------------------------------------------
	double Timer::getFramesPerSecond() const
	{
		return timeStep == 0.0 ? 0.0 : 1.0 / timeStep;
	}
	//------------------------------------------------------------------------------------------------------
	void Timer::setTimeStep(const double& timeStep)
	{
		this->timeStep = timeStep;
	}
	//------------------------------------------------------------------------------------------------------
	void Timer::setFramesPerSecond(const unsigned int& framesPerSecond)
	{
		this->timeStep = framesPerSecond == 0 ? 0.0 : 1.0 / double(framesPerSecond);
	}
	//------------------------------------------------------------------------------------------------------
	std::pair<bool, double> Timer::tick(const double& dt)
	{
		if (timeStep == 0.0) {
			return std::make_pair<>(true, dt);
		}
		timeSinceLastUpdate += dt;
		if (timeSinceLastUpdate >= timeStep) {
			// If we're lagging behind we're just skipping steps. TODO: Maybe add additional modes
			// timeSinceLastUpdate = fmod(timeSinceLastUpdate, timeStep);
			timeSinceLastUpdate -= timeStep;
			return std::make_pair<>(true, timeStep);
		}
		return std::make_pair<>(false, 0.0);
	}
	//------------------------------------------------------------------------------------------------------
	std::string getCurrentTimeAsString()
	{
		return std::format("{:%F %T}", std::chrono::system_clock::now());
	}
	//------------------------------------------------------------------------------------------------------
}
