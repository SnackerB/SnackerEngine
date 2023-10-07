#pragma once

#include "Math/Vec.h"

#include <algorithm>
#include <vector>

namespace SnackerEngine
{

	// Computes tangents and bitangents for a given model and returns a pair of two vectors containing the tangents and bitangents.
	// If computeBitangent is false, the second vector returned will just be empty.
	std::pair<std::vector<Vec3f>, std::vector<Vec3f>> computeTangentBiTangent(const std::vector<Vec3f>& positions, const std::vector<Vec2f>& texCoords, const std::vector<unsigned int>& indices, const bool& computeBitangent = false);

}