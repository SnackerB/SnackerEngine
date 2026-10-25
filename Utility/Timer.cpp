#include "Timer.h"

#include <math.h>
#include <chrono>
#include <format>
#include <string>

namespace SnackerEngine
{
	Timer::Timer(double timeStep)
		: timeStep(timeStep), timeSinceLastUpdate(0.0) {}

	Timer Timer::fromFramesPerSecond(unsigned int framesPerSecond)
	{
		return Timer(framesPerSecond == 0 ? 0.0 : 1.0 / double(framesPerSecond));
	}

	void Timer::reset()
	{
		timeSinceLastUpdate = 0.0;
	}

	double Timer::getTimeStep() const
	{
		return timeStep;
	}

	double Timer::getFramesPerSecond() const
	{
		return timeStep == 0.0 ? 0.0 : 1.0 / timeStep;
	}

	void Timer::setTimeStep(double timeStep)
	{
		this->timeStep = timeStep;
	}

	void Timer::setFramesPerSecond(unsigned int framesPerSecond)
	{
		this->timeStep = framesPerSecond == 0 ? 0.0 : 1.0 / double(framesPerSecond);
	}

	std::pair<bool, double> Timer::tick(double dt)
	{
		if (timeStep == 0.0) {
			return std::make_pair<>(true, dt);
		}
		timeSinceLastUpdate += dt;
		if (timeSinceLastUpdate >= timeStep) {
			// If we're lagging behind we're running updates as fast as possible until we caught up. 
			// TODO: Maybe add additional modes, e.g. `timeSinceLastUpdate = fmod(timeSinceLastUpdate, timeStep);`
			timeSinceLastUpdate -= timeStep;
			return std::make_pair<>(true, timeStep);
		}
		return std::make_pair<>(false, 0.0);
	}

	std::string getCurrentTimeAsString()
	{
		const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
		const std::chrono::zoned_time localTime{ std::chrono::current_zone(), now };
		return std::format("{:%F %T}", localTime);
	}
}
