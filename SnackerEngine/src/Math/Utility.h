#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
/// Index into a 2-dimensional array in row-major order
#define IDX(x, y, w) (((y)*(w))+(x))
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	/// Degrees to radians
	T degToRad(const T& angleDeg)
	{
		return angleDeg * T(M_PI) / T(180);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	/// Radians to degrees
	T radToDeg(const T& angleRad)
	{
		return angleRad / T(M_PI) * T(180);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	/// points (unit of measurement for font size) to meters
	T pointsToMeters(const T& points)
	{
		return points * T(0.000352778);
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	/// points (unit of measurement for font size) to inches
	T pointsToInches(const T& points)
	{
		return points / T(72);
	}
	//------------------------------------------------------------------------------------------------------
}