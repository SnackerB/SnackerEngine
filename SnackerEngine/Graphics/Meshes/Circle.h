#pragma once

#include "Graphics/Mesh.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// Helper function that creates a circle mesh. Should be called only by the meshManager.
	/// Subdivisions should be at least 3.
	Mesh createMeshCircle(unsigned int subdivisions = 10);
	//------------------------------------------------------------------------------------------------------
}