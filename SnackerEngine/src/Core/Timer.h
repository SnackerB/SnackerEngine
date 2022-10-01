#pragma once

#include <algorithm>

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
		/// Constructor directly specifying the timeStep
		explicit Timer(const double& timeStep = 0.0);
		/// Constructor implicitly computing the timeStep = 1.0 / framesPerSecond
		/// framesPerSeconds == 0 means the update function should be called as often as possible
		explicit Timer(const unsigned int& framesPerSecond = 0);
		/// Getters and setters
		double getTimeStep() const;
		double getFramesPerSecond() const;
		void setTimeStep(const double& timeStep);
		void setFramesPerSecond(const unsigned int& framesPerSecond);
		/// Returns true if the update method should be called.
		/// The second return value is the time with which the update method should be callled
		std::pair<bool, double> tick(const double& dt);
	};
	//------------------------------------------------------------------------------------------------------
}