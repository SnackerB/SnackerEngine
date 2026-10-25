#pragma once

#include <utility>
#include <string>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class Timer
	{
		/// Time that should pass between two updates
		double timeStep;
		/// Time since last update
		double timeSinceLastUpdate;
	public:
		/// Constructor using time step between updates. timeStep == 0.0 means the update function
		/// should be called as often as possible.
		explicit Timer(double timeStep = 0.0);
		/// Constructor implicitly computing the timeStep = 1.0 / framesPerSecond
		/// framesPerSeconds == 0 means the update function should be called as often as possible
		static Timer fromFramesPerSecond(unsigned int framesPerSecond = 0);
		/// Resets the timer to zero
		void reset();
		/// Getters and setters
		double getTimeStep() const;
		double getFramesPerSecond() const;
		void setTimeStep(double timeStep);
		void setFramesPerSecond(unsigned int framesPerSecond);
		/// Returns true if the update method should be called.
		/// The second return value is the time with which the update method should be callled
		std::pair<bool, double> tick(double dt);
	};
	//------------------------------------------------------------------------------------------------------
	std::string getCurrentTimeAsString();
	//------------------------------------------------------------------------------------------------------
}